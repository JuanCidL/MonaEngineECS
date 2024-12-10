#pragma once
#ifndef ENTT_COMPONENT_MANAGER_H
#define ENTT_COMPONENT_MANAGER_H
#include "../World/ComponentTypes.hpp"
#include <entt/entt.hpp>
#include "unordered_map"
#include <any>

namespace Mona {

    class EnTTComponentManager {
    public:
        EnTTComponentManager(entt::registry& registry) : m_registry(registry) {}

        template<typename ComponentType, typename... Args>
        ComponentType& AddComponent(entt::entity entity, Args&&... args) {
            if (m_componentCount.find(ComponentType) == m_componentCount.end()) {
                m_componentCount[ComponentType] = 1;
            }
            else {
                m_componentCount[ComponentType]++;
            }
            return m_registry.emplace<ComponentType>(entity, std::forward<Args>(args)...);
        }

        template<typename ComponentType>
        void RemoveComponent(entt::entity entity) {
            m_registry.remove<ComponentType>(entity);
            m_componentCount[ComponentType]--;
        }

        template<typename ComponentType>
        bool HasComponent(entt::entity entity) {
            return m_registry.has<ComponentType>(entity);
        }

        template<typename ComponentType>
        ComponentType& GetComponent(entt::entity entity) {
            return m_registry.get<ComponentType>(entity);
        }

        template<typename ComponentType>
        uint32_t GetComponentCount() {
            return m_componentCount[ComponentType];
        }

        template<typename... ComponentTypes, typename Func>
        void ForEach(Func func) {
            m_registry.view<ComponentTypes...>().each(func);
        }

        entt::registry& GetRegistry() {
            return m_registry;
        }

    private:
        entt::registry& m_registry;
        std::unordered_map<std::any, uint32_t> m_componentCount{};

    };
}

#endif