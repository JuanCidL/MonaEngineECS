#pragma once
#ifndef ENTT_COMPONENT_MANAGER_H
#define ENTT_COMPONENT_MANAGER_H
#include "../World/ComponentTypes.hpp"
#include <entt/entt.hpp>

namespace Mona
{

    class EnTTComponentManager
    {
    public:
        EnTTComponentManager(entt::registry &registry) : m_registry(registry) {}

        template <typename ComponentType, typename... Args>
        ComponentType &AddComponent(entt::entity entity, Args &&...args)
        {
            return m_registry.emplace<ComponentType>(entity, std::forward<Args>(args)...);
        }

        template <typename ComponentType>
        void RemoveComponent(entt::entity entity)
        {
            m_registry.remove<ComponentType>(entity);
        }

        template <typename ComponentType>
        bool HasComponent(entt::entity entity)
        {
            return m_registry.has<ComponentType>(entity);
        }

        template <typename ComponentType>
        ComponentType &GetComponent(entt::entity entity)
        {
            return m_registry.get<ComponentType>(entity);
        }

        template <typename... ComponentTypes, typename Func>
        void ForEach(std::function<void(Func &)> func)
        {
            m_registry.view<ComponentTypes...>().each(func);
        }

    private:
        entt::registry &m_registry;
    };
}

#endif