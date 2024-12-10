#pragma once
#ifndef BASESYSTEM_HPP
#define BASESYSTEM_HPP
#include <entt/entt.hpp>
#include "../EnTTComponentManager.hpp"
#include "../EnTTEventManager.hpp"

namespace Mona {
    class BaseSystem {
    public:
        BaseSystem() = default;
        virtual void StartUp(EnTTComponentManager& componentManager, EnTTEventManager& systemManager) = 0;
        virtual void Update(EnTTComponentManager& componentManager, EnTTEventManager& systemManager, float deltaTime) = 0;
        virtual void ShutDown(EnTTComponentManager& componentManager, EnTTEventManager& systemManager) = 0;
    };
}

#endif