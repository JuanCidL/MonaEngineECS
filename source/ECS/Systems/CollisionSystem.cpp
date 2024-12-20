#include "./CollisionSystem.hpp"
#include "../../MonaEngine.hpp"
#include <iostream>

namespace MonaECS
{

    void CollisionSystem::Update(ComponentManager &componentManager, EventManager &eventManager, float deltaTime) noexcept
    {
        // UpdatePositions(componentManager, deltaTime);
        auto collisions = CheckCollisions(componentManager);
        for (auto &collision : collisions)
        {
            eventManager.Publish<CollisionEvent>({collision.entity1, collision.entity2, collision.normal});
        }
    }

    void CollisionSystem::UpdatePositions(ComponentManager &componentManager, float deltaTime) noexcept
    {
        componentManager.ForEach<TransformComponent, BodyComponent>(
            [&](entt::entity entity, TransformComponent &transform, BodyComponent &body)
            {
                body.velocity += body.acceleration * deltaTime;
                glm::vec3 translation = (*transform.tHandle)->GetLocalTranslation();
                translation += body.velocity * deltaTime;
                (*transform.tHandle)->SetTranslation(translation);
            });
    }

    std::vector<CollisionEvent> CollisionSystem::CheckCollisions(ComponentManager &componentManager) noexcept
    {
        std::vector<CollisionEvent> collisions;

        auto tc = componentManager.ComponentQuery<TransformComponent, ColliderComponent>();

        for (auto itA = tc.begin(); itA != tc.end(); ++itA)
        {
            entt::entity entityA = *itA;
            ColliderComponent colliderA = componentManager.GetComponent<ColliderComponent>(entityA);
            TransformComponent transformA = componentManager.GetComponent<TransformComponent>(entityA);
            for (auto itB = std::next(itA); itB != tc.end(); ++itB)
            {
                entt::entity entityB = *itB;
                ColliderComponent colliderB = componentManager.GetComponent<ColliderComponent>(entityB);
                TransformComponent transformB = componentManager.GetComponent<TransformComponent>(entityB);
                if (CheckAABB(transformA, colliderA, transformB, colliderB))
                {
                    auto normal = GetCollisionNormal(transformA, transformB);
                    collisions.push_back({entityA, entityB, normal});
                }
            }
        }
        return collisions;
    }

    bool CollisionSystem::CheckAABB(const TransformComponent &transform1, const ColliderComponent &collider1, const TransformComponent &transform2, const ColliderComponent &collider2) noexcept
    {
        glm::vec3 min1 = (*transform1.tHandle)->GetLocalTranslation() - collider1.size;
        glm::vec3 max1 = (*transform1.tHandle)->GetLocalTranslation() + collider1.size;
        glm::vec3 min2 = (*transform2.tHandle)->GetLocalTranslation() - collider2.size;
        glm::vec3 max2 = (*transform2.tHandle)->GetLocalTranslation() + collider2.size;

        if (max1.x < min2.x || min1.x > max2.x)
            return false;
        if (max1.y < min2.y || min1.y > max2.y)
            return false;
        if (max1.z < min2.z || min1.z > max2.z)
            return false;

        return true;
    }

    glm::vec3 CollisionSystem::GetCollisionNormal(const TransformComponent &transform1, const TransformComponent &transform2) noexcept
    {
        glm::vec3 pos1 = (*transform1.tHandle)->GetLocalTranslation();
        glm::vec3 pos2 = (*transform2.tHandle)->GetLocalTranslation();
        return glm::normalize(pos1 - pos2);
    }
}