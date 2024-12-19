#pragma once
#ifndef ECS_COLLISION_SYSTEM_HPP
#define ECS_COLLISION_SYSTEM_HPP
#include <vector>
#include "./BaseSystem.hpp"
#include "../Components/CollisionComponents.hpp"

namespace MonaECS
{
    class CollisionSystem : public BaseSystem
    {
    public:
        CollisionSystem() = default;
        ~CollisionSystem() = default;
        void Update(ComponentManager& componentManager, EventManager& eventManager, float deltaTime) noexcept;
    private:
        struct CollisionPair
        {
            entt::entity entity1;
            entt::entity entity2;
            glm::vec3 normal;
        };

        void UpdatePositions(ComponentManager& componentManager, float deltaTime) noexcept;
        std::vector<CollisionPair> CheckCollisions(ComponentManager& componentManager) noexcept;
        bool CheckAABB(const TransformComponent& transform1, const ColliderComponent& collider1, const TransformComponent& transform2, const ColliderComponent& collider2) noexcept;
        glm::vec3 GetCollisionNormal(const TransformComponent& transform1, const TransformComponent& transform2) noexcept;
    };
}

#endif