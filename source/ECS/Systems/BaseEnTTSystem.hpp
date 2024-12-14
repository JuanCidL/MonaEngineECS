#pragma once
#ifndef BASE_ENTT_SYSTEM_HPP
#define BASE_ENTT_SYSTEM_HPP
#include "../EnTTComponentManager.hpp"
#include "../EnTTEventManager.hpp"

namespace Mona {
    
    class BaseEnTTSystem {
    public:
        virtual ~BaseEnTTSystem() = default;
        virtual void StartUp(EnTTComponentManager& componentManager, EnTTEventManager& eventManager) noexcept = 0;
        virtual void Update(EnTTComponentManager& componentManager, EnTTEventManager& eventManager, float deltaTime) noexcept = 0;
        virtual void ShutDown(EnTTComponentManager& componentManager, EnTTEventManager& eventManager) noexcept = 0;
    };
}

#endif