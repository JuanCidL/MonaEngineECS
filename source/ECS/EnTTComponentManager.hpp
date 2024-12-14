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

        entt::registry& GetRegistry() noexcept
        {
            return m_registry;
        }

        entt::entity CreateEntity() noexcept
        {
            return m_registry.create();
        }

        void DestroyEntity(entt::entity entity) noexcept
        {
            m_registry.destroy(entity);
        }

        template<typename ComponentType, typename... Args>
        ComponentType& AddComponent(entt::entity entity, Args&&... args)
        {
            return m_registry.emplace<ComponentType>(entity, std::forward<Args>(args)...);
        }

        template<typename... ComponentTypes, typename... ExcludeTypes>
        auto ComponentQuery(entt::exclude_t<ExcludeTypes...> exclude = entt::exclude_t<>()) noexcept
        {
            return m_registry.view<ComponentTypes...>(exclude);
        }

    private:
        entt::registry m_registry;
    };   
}


#endif