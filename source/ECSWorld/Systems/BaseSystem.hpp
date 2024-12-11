#pragma once
#ifndef BASESYSTEM_HPP
#define BASESYSTEM_HPP
#include <entt/entt.hpp>
#include "../EnTTComponentManager.hpp"
#include "../EnTTEventManager.hpp"

namespace Mona
{
    /**
     * Clase base que pasará a presentar un Sistema en el ECS. Toda clase de este tipo tiene que poseer un método de StartUp, Update y ShutDown para
     * el inicio, actuaización y cierre del sistema en concreto.
     */
    class BaseSystem
    {
    public:
        BaseSystem() = default;
        virtual void StartUp(EnTTComponentManager &componentManager, EnTTEventManager &systemManager) = 0;
        virtual void Update(EnTTComponentManager &componentManager, EnTTEventManager &systemManager, float timeStep) = 0;
        virtual void ShutDown(EnTTComponentManager &componentManager, EnTTEventManager &systemManager) = 0;
    };
}

#endif