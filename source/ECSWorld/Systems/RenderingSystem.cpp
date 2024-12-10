#include "RenderingSystem.hpp"

namespace Mona {
    void RenderingSystem::StartUp(entt::registry& registry, entt::dispatcher& dispatcher) {
        auto& config = Mona::Config::GetInstance();
        const unsigned int offset = static_cast<unsigned int>(MaterialType::MaterialTypeCount);
        // Construction of all the shaders that the engine supports.
        m_shaders[static_cast<unsigned int>(MaterialType::UnlitFlat)]                = ShaderProgram(config.getPathOfEngineAsset("Shaders/UnlitFlat.vs")               , config.getPathOfEngineAsset("Shaders/UnlitFlat.ps"));
		m_shaders[static_cast<unsigned int>(MaterialType::UnlitTextured)]            = ShaderProgram(config.getPathOfEngineAsset("Shaders/UnlitTextured.vs")           , config.getPathOfEngineAsset("Shaders/UnlitTextured.ps"));
		m_shaders[static_cast<unsigned int>(MaterialType::DiffuseFlat)]              = ShaderProgram(config.getPathOfEngineAsset("Shaders/DiffuseFlat.vs")             , config.getPathOfEngineAsset("Shaders/DiffuseFlat.ps"));
		m_shaders[static_cast<unsigned int>(MaterialType::DiffuseTextured)]          = ShaderProgram(config.getPathOfEngineAsset("Shaders/DiffuseTextured.vs")         , config.getPathOfEngineAsset("Shaders/DiffuseTextured.ps"));
		m_shaders[static_cast<unsigned int>(MaterialType::PBRFlat)]                  = ShaderProgram(config.getPathOfEngineAsset("Shaders/PBRFlat.vs")                 , config.getPathOfEngineAsset("Shaders/PBRFlat.ps"));
		m_shaders[static_cast<unsigned int>(MaterialType::PBRTextured)]              = ShaderProgram(config.getPathOfEngineAsset("Shaders/PBRTextured.vs")             , config.getPathOfEngineAsset("Shaders/PBRTextured.ps"));
		m_shaders[static_cast<unsigned int>(MaterialType::UnlitFlat) + offset]       = ShaderProgram(config.getPathOfEngineAsset("Shaders/UnlitFlatSkinning.vs")       , config.getPathOfEngineAsset("Shaders/UnlitFlat.ps"));
		m_shaders[static_cast<unsigned int>(MaterialType::UnlitTextured) + offset]   = ShaderProgram(config.getPathOfEngineAsset("Shaders/UnlitTexturedSkinning.vs")   , config.getPathOfEngineAsset("Shaders/UnlitTextured.ps"));
		m_shaders[static_cast<unsigned int>(MaterialType::DiffuseFlat) + offset]     = ShaderProgram(config.getPathOfEngineAsset("Shaders/DiffuseFlatSkinning.vs")     , config.getPathOfEngineAsset("Shaders/DiffuseFlat.ps"));
		m_shaders[static_cast<unsigned int>(MaterialType::DiffuseTextured) + offset] = ShaderProgram(config.getPathOfEngineAsset("Shaders/DiffuseTexturedSkinning.vs") , config.getPathOfEngineAsset("Shaders/DiffuseTextured.ps"));
		m_shaders[static_cast<unsigned int>(MaterialType::PBRFlat) + offset]         = ShaderProgram(config.getPathOfEngineAsset("Shaders/PBRFlatSkinning.vs")         , config.getPathOfEngineAsset("Shaders/PBRFlat.ps"));
		m_shaders[static_cast<unsigned int>(MaterialType::PBRTextured) + offset]     = ShaderProgram(config.getPathOfEngineAsset("Shaders/PBRTexturedSkinning.vs")     , config.getPathOfEngineAsset("Shaders/PBRTextured.ps"));

        dispatcher.sink<WindowResizeEvent>().connect<&RenderingSystem::OnWindowResizeEvent>(this);
        m_currentMatrixPalette.resize(NUM_MAX_BONES, glm::mat4(1.0f));
        glEnable(GL_DEPTH_TEST);

        glGenBuffers(1, &m_lightDataUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, m_lightDataUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(Lights), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_lightDataUBO);
    }

    void RenderingSystem::Update(entt::registry& registry, entt::dispatcher& dispatcher, float deltaTime) {
        
    }

    void RenderingSystem::ShutDown(entt::registry& registry, entt::dispatcher& dispatcher) {
    }

    void RenderingSystem::OnWindowResizeEvent(const WindowResizeEvent& event) {
        if (event.width == 0 || event.height == 0)
            return;
        glViewport(0, 0, event.width, event.height);
    }
}