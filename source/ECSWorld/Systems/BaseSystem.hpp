#pragma once
#ifndef BASESYSTEM_HPP
#define BASESYSTEM_HPP
#include <entt/entt.hpp>

namespace Mona {
    class BaseSystem {
    public:
        BaseSystem() = default;
        virtual void StartUp(entt::registry& registry) = 0;
        virtual void Update(entt::registry& registry, float deltaTime) = 0;
        virtual void ShutDown(entt::registry& registry) = 0;
    };
}

#endif