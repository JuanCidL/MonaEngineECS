#include "./CollisionSystem.hpp"
#include "../Components/CollisionComponents.hpp"
#include "../../MonaEngine.hpp"

namespace MonaECS
{
    void CollisionSystem::StartUp(ComponentManager &componentManager, EventManager &eventManager) noexcept
    {
        broadphase = new btDbvtBroadphase();
        collisionConfiguration = new btDefaultCollisionConfiguration();
        collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
        solver = new btSequentialImpulseConstraintSolver();
        dynamicsWorld = new btDiscreteDynamicsWorld(collisionDispatcher, broadphase, solver, collisionConfiguration);
        dynamicsWorld->setGravity(btVector3(0, -10, 0));
    }

    void CollisionSystem::Update(ComponentManager &componentManager, EventManager &eventManager, float deltaTime) noexcept
    {
        // dynamicsWorld->stepSimulation(deltaTime, 10);

        componentManager.ForEach<TransformComponent>(
            [&](entt::entity entity, TransformComponent transformComponent)
            {
                (*transformComponent.tHandle)->GetLocalTranslation().y -= deltaTime
                                                                              std::cout
                                                                          << (*transformComponent.tHandle)->GetLocalTranslation().y << std::endl;
            });
    }

    void CollisionSystem::ShutDown(ComponentManager &componentManager, EventManager &eventManager) noexcept
    {
        delete dynamicsWorld;
        delete solver;
        delete collisionDispatcher;
        delete collisionConfiguration;
        delete broadphase;
    }
}