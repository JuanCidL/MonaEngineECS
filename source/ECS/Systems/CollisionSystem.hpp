#pragma once
#ifndef ECS_COLLISION_SYSTEM_HPP
#define ECS_COLLISION_SYSTEM_HPP
#include <btBulletDynamicsCommon.h>
#include "./BaseSystem.hpp"

namespace MonaECS
{
    class CollisionSystem : public BaseSystem
    {
    public:
        CollisionSystem() = default;
        ~CollisionSystem() = default;

        void StartUp(ComponentManager& componentManager, EventManager& eventManager) noexcept;
        void Update(ComponentManager& componentManager, EventManager& eventManager, float deltaTime) noexcept;
        void ShutDown(ComponentManager& componentManager, EventManager& eventManager) noexcept;

    private:
        btBroadphaseInterface* broadphase;
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* collisionDispatcher;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* dynamicsWorld;

    };
}

#endif