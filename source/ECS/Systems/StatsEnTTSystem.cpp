#include "./StatsEnTTSystem.hpp"
#include <iostream>

namespace Mona {

    void StatsEnTTSystem::StartUp(EnTTComponentManager& componentManager, EnTTEventManager& eventManager) noexcept {
    }

    void StatsEnTTSystem::Update(EnTTComponentManager& componentManager, EnTTEventManager& eventManager, float deltaTime) noexcept {
        time += deltaTime;
        if (time >= 1.0f) {
            time = 0.0f;
            std::cout << "StatsEnTTSystem: Update" << std::endl;
        }
    }

    void StatsEnTTSystem::ShutDown(EnTTComponentManager& componentManager, EnTTEventManager& eventManager) noexcept {
    }

} // namespace Mona