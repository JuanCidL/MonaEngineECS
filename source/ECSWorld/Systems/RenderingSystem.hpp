#pragma once
#ifndef RENDERING_SYSTEM_H
#define RENDERING_SYSTEM_H
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include "./BaseSystem.hpp"
#include "../EnTTWorld.hpp"
#include "../../Event/Events.hpp"
#include "../../World/ComponentTypes.hpp"
#include "../../World/TransformComponent.hpp"
#include "../../Animation/SkeletalMeshComponent.hpp"
#include "../../Animation/SkinnedMesh.hpp"
#include "../../Rendering/StaticMeshComponent.hpp"
#include "../../Rendering/CameraComponent.hpp"
#include "../../Rendering/ShaderProgram.hpp"
#include "../../Rendering/DirectionalLightComponent.hpp"
#include "../../Rendering/SpotLightComponent.hpp"
#include "../../Rendering/PointLightComponent.hpp"
#include "../../Rendering/Material.hpp"
#include "../../Core/Config.hpp"

namespace Mona
{

	class RenderingSystem : public BaseSystem
	{

	public:
		static constexpr int NUM_HALF_MAX_DIRECTIONAL_LIGHTS = 1;
		static constexpr int NUM_HALF_MAX_POINT_LIGHTS = 3;
		static constexpr int NUM_HALF_MAX_SPOT_LIGHTS = 3;
		static constexpr int NUM_MAX_BONES = 70;
		RenderingSystem() = default;
		void StartUp(EnTTComponentManager &componentManager, EnTTEventManager &systemManager) noexcept;
		void Update(EnTTComponentManager &componentManager, EnTTEventManager &systemManager, float deltaTime) noexcept;
		void ShutDown(EnTTComponentManager &componentManager, EnTTEventManager &systemManager) noexcept; // En este caso el render es el update

		std::shared_ptr<Material> CreateMaterial(MaterialType type, bool isForSkinning);
		void OnWindowResizeEvent(const WindowResizeEvent &event);

	private:
		std::shared_ptr<Material> CreateMaterial(MaterialType type, bool isForSkinning);
		void SetBackgroundColor(float r, float g, float b, float alpha = 0.0f);

		struct DirectionalLight
		{
			glm::vec3 colorIntensity; // 12
			float padding04;		  // 16
			glm::vec3 direction;	  // 28
			float padding08;		  // 32
		};

		struct PointLight
		{
			glm::vec3 colorIntensity; // 12
			float padding04;		  // 16
			glm::vec3 position;		  // 28
			float maxRadius;		  // 32
		};

		struct SpotLight
		{
			glm::vec3 colorIntensity; // 12
			float maxRadius;		  // 16
			glm::vec3 position;		  // 28
			float cosPenumbraAngle;	  // 32
			glm::vec3 direction;
			float cosUmbraAngle; // 48
		};

		struct Lights
		{
			SpotLight spotLights[2 * NUM_HALF_MAX_SPOT_LIGHTS];
			PointLight pointLights[2 * NUM_HALF_MAX_POINT_LIGHTS];
			DirectionalLight directionalLights[2 * NUM_HALF_MAX_DIRECTIONAL_LIGHTS];
			glm::vec3 ambientLight;
			int spotLightsCount;
			int pointLightsCount;
			int directionalLightsCount;
		};
		std::array<ShaderProgram, 2 * static_cast<unsigned int>(MaterialType::MaterialTypeCount)> m_shaders;
		std::vector<glm::mat4> m_currentMatrixPalette;
		unsigned int m_lightDataUBO = 0;
		glm::vec4 m_backgroundColor = {0.0f, 0.0f, 0.0f, 0.0f};
	};

}

#endif