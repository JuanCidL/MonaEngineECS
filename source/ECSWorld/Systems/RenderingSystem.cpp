#include "RenderingSystem.hpp"

namespace Mona
{
    void RenderingSystem::StartUp(EnTTComponentManager &componentManager, EnTTEventManager &eventManager) noexcept
    {
        auto &config = Mona::Config::GetInstance();
        const unsigned int offset = static_cast<unsigned int>(MaterialType::MaterialTypeCount);
        // Construction of all the shaders that the engine supports.
        m_shaders[static_cast<unsigned int>(MaterialType::UnlitFlat)] = ShaderProgram(config.getPathOfEngineAsset("Shaders/UnlitFlat.vs"), config.getPathOfEngineAsset("Shaders/UnlitFlat.ps"));
        m_shaders[static_cast<unsigned int>(MaterialType::UnlitTextured)] = ShaderProgram(config.getPathOfEngineAsset("Shaders/UnlitTextured.vs"), config.getPathOfEngineAsset("Shaders/UnlitTextured.ps"));
        m_shaders[static_cast<unsigned int>(MaterialType::DiffuseFlat)] = ShaderProgram(config.getPathOfEngineAsset("Shaders/DiffuseFlat.vs"), config.getPathOfEngineAsset("Shaders/DiffuseFlat.ps"));
        m_shaders[static_cast<unsigned int>(MaterialType::DiffuseTextured)] = ShaderProgram(config.getPathOfEngineAsset("Shaders/DiffuseTextured.vs"), config.getPathOfEngineAsset("Shaders/DiffuseTextured.ps"));
        m_shaders[static_cast<unsigned int>(MaterialType::PBRFlat)] = ShaderProgram(config.getPathOfEngineAsset("Shaders/PBRFlat.vs"), config.getPathOfEngineAsset("Shaders/PBRFlat.ps"));
        m_shaders[static_cast<unsigned int>(MaterialType::PBRTextured)] = ShaderProgram(config.getPathOfEngineAsset("Shaders/PBRTextured.vs"), config.getPathOfEngineAsset("Shaders/PBRTextured.ps"));
        m_shaders[static_cast<unsigned int>(MaterialType::UnlitFlat) + offset] = ShaderProgram(config.getPathOfEngineAsset("Shaders/UnlitFlatSkinning.vs"), config.getPathOfEngineAsset("Shaders/UnlitFlat.ps"));
        m_shaders[static_cast<unsigned int>(MaterialType::UnlitTextured) + offset] = ShaderProgram(config.getPathOfEngineAsset("Shaders/UnlitTexturedSkinning.vs"), config.getPathOfEngineAsset("Shaders/UnlitTextured.ps"));
        m_shaders[static_cast<unsigned int>(MaterialType::DiffuseFlat) + offset] = ShaderProgram(config.getPathOfEngineAsset("Shaders/DiffuseFlatSkinning.vs"), config.getPathOfEngineAsset("Shaders/DiffuseFlat.ps"));
        m_shaders[static_cast<unsigned int>(MaterialType::DiffuseTextured) + offset] = ShaderProgram(config.getPathOfEngineAsset("Shaders/DiffuseTexturedSkinning.vs"), config.getPathOfEngineAsset("Shaders/DiffuseTextured.ps"));
        m_shaders[static_cast<unsigned int>(MaterialType::PBRFlat) + offset] = ShaderProgram(config.getPathOfEngineAsset("Shaders/PBRFlatSkinning.vs"), config.getPathOfEngineAsset("Shaders/PBRFlat.ps"));
        m_shaders[static_cast<unsigned int>(MaterialType::PBRTextured) + offset] = ShaderProgram(config.getPathOfEngineAsset("Shaders/PBRTexturedSkinning.vs"), config.getPathOfEngineAsset("Shaders/PBRTextured.ps"));

        eventManager.Subscribe(this, &RenderingSystem::OnWindowResizeEvent);
        m_currentMatrixPalette.resize(NUM_MAX_BONES, glm::mat4(1.0f));
        glEnable(GL_DEPTH_TEST);

        glGenBuffers(1, &m_lightDataUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, m_lightDataUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(Lights), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_lightDataUBO);
    }

    void RenderingSystem::Update(EnTTComponentManager &componentManager, EnTTEventManager &eventManager, float deltaTime) noexcept
    {
        /**
         * El código original recibe:
         * - eventManager (parámetro del world)
         * - camerHandle (parámetro del world)
         * - ambientLight (parámetro del world)
         * - staticMeshDataManager
         * - skeletalMeshDataManager
         * - transformDataManager
         * - cameraDataManager
         * - directionalLightDataManager
         * - spotLightDataManager
         * - pointLightDataManager
         * */
        entt::registry &registry = componentManager.GetRegistry();
        glClearColor(m_backgroundColor[0], m_backgroundColor[1], m_backgroundColor[2], m_backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
        glm::vec3 cameraPosition(0.0f);

        Mona::EnTTWorld* pworld = nullptr;
        entt::entity *cameraEntity{nullptr};

        registry.view<Mona::EnTTWorld *>().each(
            [&](entt::entity entity)
            {
                Mona::EnTTWorld& world = registry.get<Mona::EnTTWorld>(entity);
                pworld = &world;
                cameraEntity = pworld->GetCurrentCamera();
            });

        if (cameraEntity != nullptr)
        {
            registry.view<CameraComponent, TransformComponent, GameObject>().each(
                [&cameraEntity, &viewMatrix, &projectionMatrix, &cameraPosition](entt::entity entity, auto &camera, auto &cameraTransform)
                {
                    if (*cameraEntity == entity)
                    {
                        viewMatrix = cameraTransform.GetViewMatrixFromTransform();
                        projectionMatrix = camera.GetProjectionMatrix();
                        cameraPosition = cameraTransform.GetLocalTranslation();
                    }
                });
        }
        else
        {
            MONA_LOG_INFO("Render Info: No camera has been set, using defaults transformations");
            viewMatrix = glm::mat4(1.0f);
            projectionMatrix = glm::perspective(glm::radians(50.0f), 16.0f / 9.0f, 0.1f, 100.0f);
        }

        Lights lights;
        lights.ambientLight = pworld->m_ambientLight;

        uint32_t directionalLightsCount = std::min(static_cast<uint32_t>(NUM_HALF_MAX_DIRECTIONAL_LIGHTS * 2), componentManager.GetComponentCount<DirectionalLightComponent>());
        lights.directionalLightsCount = static_cast<int>(directionalLightsCount);
        uint32_t i = 0;
        registry.view<DirectionalLightComponent>()
            .each([&](entt::entity entity, auto &dirLight)
                  {
                        if (i >= directionalLightsCount) return; 

                        if (registry.all_of<TransformComponent>(entity)) {
                            const auto& lightTransform = registry.get<TransformComponent>(entity);

                            lights.directionalLights[i].colorIntensity = dirLight.GetLightColor();
                            lights.directionalLights[i].direction = glm::rotate(
                                dirLight.GetLightDirection(),
                                lightTransform.GetFrontVector()
                            );
                        }

                        ++i; });

        uint32_t spotLightsCount = std::min(static_cast<uint32_t>(NUM_HALF_MAX_SPOT_LIGHTS * 2), componentManager.GetComponentCount<SpotLightComponent>());
        lights.spotLightsCount = static_cast<int>(spotLightsCount);
        i = 0;
        registry.view<SpotLightComponent, GameObject, TransformComponent>()
            .each([&](entt::entity entity, auto &spotLight, auto &lightTransform)
                  {
                    if(i >= spotLightsCount) return;

                    lights.spotLights[i].colorIntensity = spotLight.GetLightColor();
                    lights.spotLights[i].direction = glm::rotate(spotLight.GetLightDirection(), lightTransform->GetFrontVector());
                    lights.spotLights[i].position = lightTransform->GetLocalTranslation();
                    lights.spotLights[i].cosPenumbraAngle = glm::cos(spotLight.GetPenumbraAngle());
                    lights.spotLights[i].cosUmbraAngle = glm::cos(spotLight.GetUmbraAngle());
                    lights.spotLights[i].maxRadius = spotLight.GetMaxRadius(); 

                    ++i; });

        uint32_t pointLightsCount = std::min(static_cast<uint32_t>(NUM_HALF_MAX_POINT_LIGHTS * 2), componentManager.GetComponentCount<PointLightComponent>());
        lights.pointLightsCount = static_cast<int>(pointLightsCount);
        i = 0;
        registry.view<PointLightComponent, TransformComponent, GameObject>()
            .each([&](entt::entity entity, auto &pointLight, auto &lightTransform)
                  {
                    if(i >= pointLightsCount) return ;

                    lights.pointLights[i].colorIntensity = pointLight.GetLightColor();
                    lights.pointLights[i].position = lightTransform->GetLocalTranslation();
                    lights.pointLights[i].maxRadius = pointLight.GetMaxRadius();
                    ++i; });

        glBindBuffer(GL_UNIFORM_BUFFER, m_lightDataUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Lights), &lights);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        componentManager.ForEach<StaticMeshComponent, TransformComponent>(
            [&](entt::entity entity, StaticMeshComponent &staticMesh, TransformComponent &transform)
            {
                glBindVertexArray(staticMesh.GetMeshVAOID());
                staticMesh.m_materialPtr->SetUniforms(projectionMatrix, viewMatrix, transform.GetModelMatrix(), cameraPosition);
            }
        );

        componentManager.ForEach<SkeletalMeshComponent, TransformComponent>(
            [&](entt::entity entity, SkeletalMeshComponent &skeletalMesh, TransformComponent &transform)
            {
                SkinnedMesh& skinnedMesh = *skeletalMesh.m_skinnedMeshPtr;
                glBindVertexArray(skinnedMesh.GetVertexArrayID());
                
                auto &animController = skeletalMesh.GetAnimationController();
                skeletalMesh.m_materialPtr->SetUniforms(projectionMatrix, viewMatrix, transform.GetModelMatrix(), cameraPosition);
			    glUniformMatrix4fv(ShaderProgram::BoneTransformShaderLocation, skeletalMesh.GetSkeleton()->JointCount(), GL_FALSE, (GLfloat*) m_currentMatrixPalette.data());
			    glDrawElements(GL_TRIANGLES, skinnedMesh.GetIndexBufferCount(), GL_UNSIGNED_INT, 0);
            }
        );
    }

    void RenderingSystem::ShutDown(EnTTComponentManager &componentManager, EnTTEventManager &eventManager) noexcept
    {
        eventManager.Unsubscribe(this, &RenderingSystem::OnWindowResizeEvent);
        glDeleteBuffers(1, &m_lightDataUBO);
    }

    void RenderingSystem::OnWindowResizeEvent(const WindowResizeEvent &event)
    {
        if (event.width == 0 || event.height == 0)
            return;
        glViewport(0, 0, event.width, event.height);
    }

    std::shared_ptr<Material> RenderingSystem::CreateMaterial(MaterialType type, bool isForSkinning)
    {
        unsigned int offset = static_cast<unsigned int>(type);
        offset = isForSkinning ? offset + static_cast<unsigned int>(MaterialType::MaterialTypeCount) : offset;

        switch (type)
        {
        case MaterialType::UnlitFlat:
            return std::make_shared<Material>(m_shaders[offset]);
            break;
        case MaterialType::UnlitTextured:
            return std::make_shared<Material>(m_shaders[offset]);
            break;
        case MaterialType::DiffuseFlat:
            return std::make_shared<Material>(m_shaders[offset]);
            break;
        case MaterialType::DiffuseTextured:
            return std::make_shared<Material>(m_shaders[offset]);
            break;
        case MaterialType::PBRFlat:
            return std::make_shared<Material>(m_shaders[offset]);
            break;
        case MaterialType::PBRTextured:
            return std::make_shared<Material>(m_shaders[offset]);
            break;
        case MaterialType::MaterialTypeCount:
            return std::make_shared<Material>(m_shaders[offset]);
            break;
        default:
            return nullptr;
            break;
        }

    }

    void RenderingSystem::SetBackgroundColor(float r, float g, float b, float alpha)
    {
        m_backgroundColor = glm::vec4(r, g, b, alpha);
    }
}