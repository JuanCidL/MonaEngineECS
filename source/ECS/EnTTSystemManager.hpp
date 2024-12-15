#pragma once
#ifndef ENTT_SYSTEM_MANAGER_HPP
#define ENTT_SYSTEM_MANAGER_HPP
#include <unordered_map>
#include <typeindex>
#include "./Systems/BaseEnTTSystem.hpp"

namespace Mona
{
    class EnTTSystemManager
    {
    public:
        EnTTSystemManager() = default;
        ~EnTTSystemManager() = default;

        template<typename SystemType, typename... Args>
        SystemType RegisterSystem(Args&&... args)
        {
            static_assert(std::is_base_of<BaseEnTTSystem, SystemType>::value, "SystemType must derive from BaseEnTTSystem");
            std::type_index typeIndex = typeid(SystemType);
            if (m_systems.find(typeIndex) != m_systems.end())
            {
                return *static_cast<SystemType *>(m_systems[typeIndex].get());
            }
            auto system = std::make_unique<SystemType>(std::forward<Args>(args)...);
            SystemType &systemRef = *system;
            m_systems[typeIndex] = std::move(system);
            return systemRef;
        }

        template<typename SystemType>
        void UnregisterSystem()
        {
            assert(m_systems.find(typeid(SystemType)) != m_systems.end() && "System does not exist");
            delete m_systems[typeid(SystemType)];
            m_systems.erase(typeid(SystemType));
        }

        template<typename SystemType>
        SystemType& GetSystem()
        {
            assert(m_systems.find(typeid(SystemType)) != m_systems.end() && "System does not exist");
            return *static_cast<SystemType*>(m_systems[typeid(SystemType)]);
        }

        void StartUpSystems(EnTTComponentManager& componentManager, EnTTEventManager& eventManager)
        {
            for (auto& system : m_systems)
            {
                system.second->StartUp(componentManager, eventManager);
            }
        }

        void UpdateSystems(EnTTComponentManager& componentManager, EnTTEventManager& eventManager, float deltaTime)
        {
            for (auto& system : m_systems)
            {
                system.second->Update(componentManager, eventManager, deltaTime);
            }
        }

        void ShutDownSystems(EnTTComponentManager& componentManager, EnTTEventManager& eventManager)
        {
            for (auto& system : m_systems)
            {
                system.second->ShutDown(componentManager, eventManager);
            }
        }
    
    private:
        std::unordered_map<std::type_index, std::unique_ptr<BaseEnTTSystem>> m_systems;
    };   
}

#endif