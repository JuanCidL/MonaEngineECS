#pragma once
#ifndef STAT_COMPONENTS_HPP
#define STAT_COMPONENTS_HPP
#include <glm/glm.hpp>

namespace MonaECS
{
    enum class StatsColors
    {
        red,
        green,
        blue
    };
    struct Stats
    {
        float health = 100.0f;
        float speed = 10.0f;
        StatsColors state = StatsColors::red;

        glm::vec3 getColor() // tengo la duda con que esto esté acá
        {
            switch (state)
            {
            case StatsColors::red:
                return glm::vec3(1.0f, 0.0f, 0.0f);
            case StatsColors::green:
                return glm::vec3(0.0f, 1.0f, 0.0f);
            case StatsColors::blue:
                return glm::vec3(0.0f, 0.0f, 1.0f);
            }
        }
    };
}

#endif