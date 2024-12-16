#include "./CollisionSystem.hpp"

namespace MonaECS
{
    void CollisionSystem::StartUp(ComponentManager& componentManager, EventManager& eventManager) noexcept
    {
        broadphase = new btDbvtBroadphase();
        collisionConfiguration = new btDefaultCollisionConfiguration();
        collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
        solver = new btSequentialImpulseConstraintSolver();
        dynamicsWorld = new btDiscreteDynamicsWorld(collisionDispatcher, broadphase, solver, collisionConfiguration);
        dynamicsWorld->setGravity(btVector3(0, -10, 0));
    }

    void CollisionSystem::Update(ComponentManager& componentManager, EventManager& eventManager, float deltaTime) noexcept
    {
        dynamicsWorld->stepSimulation(deltaTime, 10);
    }

    void CollisionSystem::ShutDown(ComponentManager& componentManager, EventManager& eventManager) noexcept
    {
        delete dynamicsWorld;
        delete solver;
        delete collisionDispatcher;
        delete collisionConfiguration;
        delete broadphase;
    }
}