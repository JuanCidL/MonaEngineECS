#include "./CollisionEnTTSystem.hpp"

namespace Mona
{
    void CollisionEnTTSystem::StartUp(EnTTComponentManager& componentManager, EnTTEventManager& eventManager) noexcept
    {
        broadphase = new btDbvtBroadphase();
        collisionConfiguration = new btDefaultCollisionConfiguration();
        collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
        solver = new btSequentialImpulseConstraintSolver();
        dynamicsWorld = new btDiscreteDynamicsWorld(collisionDispatcher, broadphase, solver, collisionConfiguration);
        dynamicsWorld->setGravity(btVector3(0, -10, 0));
    }

    void CollisionEnTTSystem::Update(EnTTComponentManager& componentManager, EnTTEventManager& eventManager, float deltaTime) noexcept
    {
        dynamicsWorld->stepSimulation(deltaTime, 10);
    }

    void CollisionEnTTSystem::ShutDown(EnTTComponentManager& componentManager, EnTTEventManager& eventManager) noexcept
    {
        delete dynamicsWorld;
        delete solver;
        delete collisionDispatcher;
        delete collisionConfiguration;
        delete broadphase;
    }
}