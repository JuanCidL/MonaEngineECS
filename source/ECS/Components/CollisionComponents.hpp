#pragma once
#ifndef COLLISION_COMPONENTS_HPP
#define COLLISION_COMPONENTS_HPP
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace MonaECS
{
    struct CollisionShapeComponent
    {
        btCollisionShape* shape;
    };

    struct RigidBodyComponent
    {
        btRigidBody* rigidBody;
    };

    // struct TransformComponent
    // {
    //     glm::vec3 position;
    //     glm::fquat rotation;
    //     glm::vec3 scale;
    // };

}

#endif