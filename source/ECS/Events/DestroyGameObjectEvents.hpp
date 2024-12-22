#pragma once
#ifndef ECS_DESTROY_GAME_OBJECT_EVENTS_HPP
#define ECS_DESTROY_GAME_OBJECT_EVENTS_HPP
#include <entt/entt.hpp>

namespace MonaECS
{
    struct DestroyGameObjectEvent
    {
        DestroyGameObjectEvent() = default;
        // entt::entity entity;
    };
}

#endif