#pragma once
#ifndef COLLISION_COMPONENTS_HPP
#define COLLISION_COMPONENTS_HPP
#include "../../MonaEngine.hpp"
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace MonaECS
{
    struct CollisionShapeComponent
    {
        btCollisionShape *shape;
    };

    struct RigidBodyComponent
    {
        btRigidBody *rigidBody;
    };

    struct TransformComponent
    {
        Mona::TransformHandle *tHandle;
        operator const Mona::TransformHandle *() { return tHandle; }
    };

}

#endif