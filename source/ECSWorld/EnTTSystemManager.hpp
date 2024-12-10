#pragma once
#ifndef ENTT_SYSTEMMANAGER_HPP
#define ENTT_SYSTEMMANAGER_HPP
#include <entt/entt.hpp>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include "./EnTTComponentManager.hpp"
#include "./EnTTEventManager.hpp"
#include "./Systems/BaseSystem.hpp"

namespace Mona {
    class EnTTSystemManager {
    public :
        EnTTSystemManager(EnTTComponentManager& componentManager, EnTTEventManager& eventManager) :
            m_componentManager(componentManager),
            m_eventManager(eventManager)
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
                system->StartUp(m_componentManager, m_eventManager);
            }
        }

        void Update(float timeStep) {
            for (auto& [typeIndex, system] : m_systems) {
                system->Update(m_componentManager, m_eventManager, timeStep);
            }
        }

        void ShutDown() {
            for (auto& [typeIndex, system] : m_systems) {
                system->ShutDown(m_componentManager, m_eventManager);
            }
        }
    
    private:
        EnTTComponentManager& m_componentManager;
        EnTTEventManager& m_eventManager;
        std::unordered_map<std::type_index, std::unique_ptr<BaseSystem>> m_systems;
    };
}

#endif