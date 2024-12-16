#include "./StatsSystem.hpp"
#include <iostream>

namespace MonaECS
{

    void StatsSystem::Update(ComponentManager &componentManager, EventManager &eventManager, float deltaTime) noexcept
    {
        time += deltaTime;
        if (time >= 1.0f)
        {
            time = 0.0f;
            std::cout << "StatsSystem: Update" << std::endl;
        }

        auto vw = componentManager.ComponentQuery<Stats>();

        componentManager.ForEach<Stats>(
            [&](entt::entity entity, Stats &stats)
            {
                stats.health -= 0.1f;
                if (stats.health > 30.0f && stats.health <= 60.0f)
                {
                    stats.state = StatsColors::green;
                    std::cout << "Stat State: green" << std::endl;
                }
                else if (stats.health > 0.0f && stats.health <= 30.0f)
                {
                    stats.state = StatsColors::blue;
                    std::cout << "Stat State: blue" << std::endl;
                }
                else if (stats.health <= 0.0f)
                {
                    componentManager.DestroyEntity(entity);
                }
                else
                {
                    stats.state = StatsColors::red;
                    std::cout << "Stat State: red" << std::endl;
                }
            });
    }
}