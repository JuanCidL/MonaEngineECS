#pragma once
#ifndef MOVEMENT_SYSTEM_HPP
#define MOVEMENT_SYSTEM_HPP
#include "./BaseSystem.hpp"
#include "../Components/CollisionComponents.hpp"
#include "../Events./InputEvents.hpp"

namespace MonaECS {
    class MovementSystem : public BaseSystem
    {
    public:
        MovementSystem() = default;
        ~MovementSystem() = default;
        void Update(ComponentManager& componentManager, EventManager& eventManager, float deltaTime) noexcept;
    private:
        void HandleContinuousMovement(TransformComponent& transform, BodyComponent& body, float deltaTime);
    };
}

#endif
