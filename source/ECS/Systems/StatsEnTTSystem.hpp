#pragma once
#ifndef STATS_ENTT_SYSTEM_HPP
#define STATS_ENTT_SYSTEM_HPP
#include "BaseEnTTSystem.hpp"
#include "../Components/StatComponents.hpp"

namespace Mona {
    
    class StatsEnTTSystem : public BaseEnTTSystem {
    public:
        StatsEnTTSystem() = default;
        ~StatsEnTTSystem() = default;
        void StartUp(EnTTComponentManager& componentManager, EnTTEventManager& eventManager) noexcept;
        void Update(EnTTComponentManager& componentManager, EnTTEventManager& eventManager, float deltaTime) noexcept;
        void ShutDown(EnTTComponentManager& componentManager, EnTTEventManager& eventManager) noexcept;

    private:
        float time = 0.0f;
    };
}

#endif