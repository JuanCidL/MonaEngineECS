#pragma once
#ifndef ENTT_WORLD_H
#define ENTT_WORLD_H
#include <entt/entt.hpp>
#include "ComponentTypes.hpp"
#include "EnTTComponentManager.hpp"
#include "EnTTSystemManager.hpp"
#include "EnTTEventManager.hpp"
#include "./systems/AudioEnttSystem.hpp"
#include "../Rendering/CameraComponent.hpp"

namespace Mona
{

    class EnTTWorld
    {
    public:
        EnTTWorld() : m_registry(),
                      m_dispatcher(),
                      m_componentManager(m_registry),
                      m_entityCount(0),
                      m_eventManager(m_dispatcher),
                      m_systemManager(m_componentManager, m_eventManager),
                      m_currentCamera(nullptr)
        {
            auto worldEntity = m_registry.create();
            m_registry.emplace<EnTTWorld *>(worldEntity, this);
        }
        EnTTWorld(const EnTTWorld &world) = delete;
        EnTTWorld &operator=(const EnTTWorld &world) = delete;

        // EnTT Handling

        entt::registry &GetRegistry()
        {
            return m_registry;
        }

        entt::dispatcher &GetDispatcher()
        {
            return m_dispatcher;
        }

        size_t GetGameObjectCount()
        {
            return m_entityCount;
        }

        // Game Object Handling

        entt::entity CreateGameObject()
        {
            return m_registry.create();
            m_entityCount++;
        }

        void DestroyGameObject(entt::entity entity)
        {
            m_registry.destroy(entity);
            m_entityCount--;
        }

        // Component Handling

        template <typename ComponentType, typename... Args>
        void AddComponent(entt::entity entity, Args &&...args)
        {
            m_componentManager.AddComponent<ComponentType>(entity, std::forward<Args>(args)...);
        }

        template <typename ComponentType>
        void RemoveComponent(entt::entity entity)
        {
            m_componentManager.RemoveComponent<ComponentType>(entity);
        }

        template <typename ComponentType>
        bool HasComponent(entt::entity entity)
        {
            return m_componentManager.HasComponent<ComponentType>(entity);
        }

        template <typename ComponentType>
        ComponentType &GetComponent(entt::entity entity)
        {
            return m_componentManager.GetComponent<ComponentType>(entity);
        }

        template<typename... ComponentTypes, typename Func>
        void ForEachComponents(Func func) {
            m_componentManager.ForEach<ComponentTypes...>(func);
        }

        // System Handling

        template <typename SystemType>
        void AddSystem()
        {
            m_systemManager.AddSystem<SystemType>();
        }

        template <typename SystemType>
        void RemoveSystem()
        {
            m_systemManager.RemoveSystem<SystemType>();
        }

        void UpdateSystems(float timeStep)
        {
            m_systemManager.Update(timeStep);
        }

        // Custom Component and Property Handling

        void SetCurrentCamera(CameraComponent *camera)
        {
            m_currentCamera = camera;
        }

        CameraComponent *GetCurrentCamera()
        {
            return m_currentCamera;
        }

    private:
        entt::registry m_registry;
        entt::dispatcher m_dispatcher;
        EnTTComponentManager m_componentManager;
        size_t m_entityCount;
        EnTTSystemManager m_systemManager;
        EnTTEventManager m_eventManager;
        glm::fquat m_audioListenerOffsetRotation = glm::fquat(1.0f, 0.0f, 0.0f, 0.0f);
        CameraComponent* m_currentCamera;

        friend class Mona::EnttAudioSystem;
    };
}

#endif