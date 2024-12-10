#include "RenderingSystem.hpp"
#include "../EnTTWorld.hpp"

namespace Mona
{
    void RenderingSystem::StartUp(entt::registry &registry, entt::dispatcher &dispatcher)
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

        dispatcher.sink<WindowResizeEvent>().connect<&RenderingSystem::OnWindowResizeEvent>(this);
        m_currentMatrixPalette.resize(NUM_MAX_BONES, glm::mat4(1.0f));
        glEnable(GL_DEPTH_TEST);

        glGenBuffers(1, &m_lightDataUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, m_lightDataUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(Lights), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_lightDataUBO);
    }

    void RenderingSystem::Update(entt::registry &registry, entt::dispatcher &dispatcher, float deltaTime)
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

        glClearColor(m_backgroundColor[0], m_backgroundColor[1], m_backgroundColor[2], m_backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
        glm::vec3 cameraPosition(0.0f);

        Mona::EnTTWorld *world = nullptr;
        entt::entity cameraEntity{entt::null};

        registry.view<Mona::EnTTWorld>().each(
            [&world, &cameraEntity](auto entity)
            {
                world = &registry.get<Mona::EnTTWorld>(entity);
                cameraEntity = world->cameraEntity;
            });

        if (cameraEntity != entt::null)
        {
            registry.view<CameraComponent, TransformComponent, GameObject>().each(
                [&cameraEntity, &viewMatrix, &projectionMatrix, &cameraPosition](auto entity, auto &camera, auto &cameraTransform)
                {
                    if (cameraEntity == entity)
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
        lights.ambientLight = world->m_ambientLight;
    }

    void RenderingSystem::ShutDown(entt::registry &registry, entt::dispatcher &dispatcher)
    {
        // eventManager.Unsubscribe(m_onWindowResizeSubscription);
        // Lo de arriba seria en torno al dispatcher
        glDeleteBuffers(1, &m_lightDataUBO);
    }

    void RenderingSystem::OnWindowResizeEvent(const WindowResizeEvent &event)
    {
        if (event.width == 0 || event.height == 0)
            return;
        glViewport(0, 0, event.width, event.height);
    }
}