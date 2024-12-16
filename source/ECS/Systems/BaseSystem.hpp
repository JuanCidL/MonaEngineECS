#pragma once
#ifndef BASE_ECS_SYSTEM_HPP
#define BASE_ECS_SYSTEM_HPP
#include "../ComponentManager.hpp"
#include "../EventManager.hpp"

namespace MonaECS {
    
    class BaseSystem {
    public:
        virtual ~BaseSystem() = default;
        virtual void StartUp(ComponentManager& componentManager, EventManager& eventManager) noexcept = 0;
        virtual void Update(ComponentManager& componentManager, EventManager& eventManager, float deltaTime) noexcept = 0;
        virtual void ShutDown(ComponentManager& componentManager, EventManager& eventManager) noexcept = 0;
    };
}

#endif