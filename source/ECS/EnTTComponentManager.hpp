#pragma once
#ifndef ENTT_COMPONENT_MANAGER_HPP
#define ENTT_COMPONENT_MANAGER_HPP
#include <entt/entt.hpp>

namespace Mona
{
    class EnTTComponentManager
    {
    public:
        EnTTComponentManager() = default;
        ~EnTTComponentManager() = default;

        entt::entity CreateEntity() noexcept
        {
            return m_registry.create();
        }

        void DestroyEntity(entt::entity entity) noexcept
        {
            m_registry.destroy(entity);
        }

    private:
        entt::registry m_registry;
    };   
}


#endif