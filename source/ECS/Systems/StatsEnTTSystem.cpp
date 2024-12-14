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

        auto vw = componentManager.ComponentQuery<Comp, Comp2>();

        componentManager.ForEach<Comp, Comp2>(
            [&](entt::entity entity, Comp& cmp, Comp2& cmp2) {
                cmp.x += 0.001;
                cmp2.x += 0.001;
                std::cout << "Comp: " << cmp.x << std::endl;
                std::cout << "Comp2: " << cmp2.x << std::endl;
            }
        );
    }

    void StatsEnTTSystem::ShutDown(EnTTComponentManager& componentManager, EnTTEventManager& eventManager) noexcept {
    }

}