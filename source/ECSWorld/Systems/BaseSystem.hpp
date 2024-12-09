#pragma once
#ifndef BASESYSTEM_HPP
#define BASESYSTEM_HPP
#include <entt/entt.hpp>

namespace Mona {
    class BaseSystem {
    public:
        BaseSystem() = default;
        virtual void StartUp(entt::registry& registry, entt::dispatcher& dispatcher) = 0;
        virtual void Update(entt::registry& registry, entt::dispatcher& dispatcher, float deltaTime) = 0;
        virtual void ShutDown(entt::registry& registry, entt::dispatcher& dispatcher) = 0;
    };
}

#endif