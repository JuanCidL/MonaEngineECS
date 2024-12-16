#pragma once
#ifndef ECS_STATS_SYSTEM_HPP
#define ECS_STATS_SYSTEM_HPP
#include "BaseSystem.hpp"
#include "../Components/StatComponents.hpp"

namespace MonaECS {
    
    class StatsSystem : public BaseSystem {
    public:
        StatsSystem() = default;
        ~StatsSystem() = default;
        void Update(ComponentManager& componentManager, EventManager& eventManager, float deltaTime) noexcept;

    private:
        float time = 0.0f;
    };
}

#endif