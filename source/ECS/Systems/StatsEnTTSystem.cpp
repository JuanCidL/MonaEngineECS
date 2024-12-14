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

        auto vw = componentManager.ComponentQuery<Comp>(entt::exclude_t<Comp2>());
        for (auto entity : vw) {
            auto& comp = vw.get<Comp>(entity);
            std::cout << "Comp: " << comp.x << std::endl;
            comp.x -= 0.01f;
        }
    }

    void StatsEnTTSystem::ShutDown(EnTTComponentManager& componentManager, EnTTEventManager& eventManager) noexcept {
    }

}