#pragma once
#ifndef BASESYSTEM_HPP
#define BASESYSTEM_HPP
#include <entt/entt.hpp>

namespace Mona {
    class BaseSystem {
    public:
        BaseSystem() = default;
        virtual void StartUp(EnTTComponentManager& componentManager, EnTTSystemManager& systemManager) = 0;
        virtual void Update(EnTTComponentManager& componentManager, EnTTSystemManager& systemManager, float deltaTime) = 0;
        virtual void ShutDown(EnTTComponentManager& componentManager, EnTTSystemManager& systemManager) = 0;
    };
}

#endif