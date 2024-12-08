#pragma once
#ifndef ENTT_SYSTEMMANAGER_HPP
#define ENTT_SYSTEMMANAGER_HPP
#include <entt/entt.hpp>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include "./Systems/BaseSystem.hpp"

namespace Mona {
    class EnTTSystemManager {
    public :
        EnTTSystemManager(entt::registry& registry, entt::dispatcher& dispatcher) :
            m_registry(registry),
            m_dispatcher(dispatcher)
        {}

        template<typename SystemType, typename... Args>
        SystemType& AddSystem(Args&&... args) {
            // Check if SystemType derives from BaseSystem
            static_assert(std::is_base_of<BaseSystem, SystemType>::value, "SystemType must derive from BaseSystem");
            // Check if the system is already added
            std::type_index typeIndex = typeid(SystemType);
            if (m_systems.find(typeIndex) != m_systems.end()) {
                return *static_cast<SystemType*>(m_systems[typeid(SystemType)].get());
            }
            auto system = std::make_unique<SystemType>(std::forward<Args>(args)...);
            SystemType& systemRef = *system;
            m_systems[typeIndex] = std::move(system);
            return systemRef;
        }

        template<typename SystemType>
        SystemType& GetSystem() {
            return *static_cast<SystemType*>(m_systems[typeid(SystemType)].get());
        }

        template<typename SystemType>
        void RemoveSystem() {
            m_systems.erase(typeid(SystemType));
        }

        void StartUp() {
            for (auto& [typeIndex, system] : m_systems) {
                system->StartUp(m_registry, m_dispatcher);
            }
        }

        void Update(float deltaTime) {
            for (auto& [typeIndex, system] : m_systems) {
                system->Update(m_registry, m_dispatcher, deltaTime);
            }
        }

        void ShutDown() {
            for (auto& [typeIndex, system] : m_systems) {
                system->ShutDown(m_registry, m_dispatcher);
            }
        }
    
    private:
        entt::registry& m_registry;
        entt::dispatcher& m_dispatcher;
        std::unordered_map<std::type_index, std::unique_ptr<BaseSystem>> m_systems;
    };
}

#endif