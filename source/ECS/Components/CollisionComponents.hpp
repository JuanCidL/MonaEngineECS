#pragma once
#ifndef COLLISION_COMPONENTS_HPP
#define COLLISION_COMPONENTS_HPP
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Mona
{
    struct CollisionShapeEnTTComponent
    {
        btCollisionShape* shape;
    };

    struct RigidBodyEnTTComponent
    {
        btRigidBody* rigidBody;
    };

    struct TransformEnTTComponent
    {
        glm::vec3 position;
        glm::fquat rotation;
        glm::vec3 scale;
    };


}

#endif