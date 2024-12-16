#pragma once
#ifndef COLLISION_ENTT_SYSTEM_HPP
#define COLLISION_ENTT_SYSTEM_HPP
#include <btBulletDynamicsCommon.h>
#include "./BaseEnTTSystem.hpp"

namespace Mona
{
    class CollisionEnTTSystem : public BaseEnTTSystem
    {
    public:
        CollisionEnTTSystem() = default;
        ~CollisionEnTTSystem() = default;

        void StartUp(EnTTComponentManager& componentManager, EnTTEventManager& eventManager) noexcept;
        void Update(EnTTComponentManager& componentManager, EnTTEventManager& eventManager, float deltaTime) noexcept;
        void ShutDown(EnTTComponentManager& componentManager, EnTTEventManager& eventManager) noexcept;

    private:
        btBroadphaseInterface* broadphase;
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* collisionDispatcher;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* dynamicsWorld;

    };
}

#endif