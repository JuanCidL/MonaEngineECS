#pragma once
#ifndef META_WORLD_H
#define META_WORLD_H
#include "../World/World.hpp"
#include "../ECSWorld/EnTTWorld.hpp"
#include "../Event/EventManager.hpp"

namespace Mona {

    class EnTTTranslationHandler;

    class MetaWorld {
    public:
        friend class EnTTTranslationHandler;
        MetaWorld(World& world, EnTTWorld& enttWorld);
        ~MetaWorld() = default;

        // Worlds Handling
        void InitializeCommunication();
        World& GetWorld();
        EnTTWorld& GetEnTTWorld();

    private:
        World& m_world;
        EnTTWorld& m_enttWorld;
        EnTTTranslationHandler* m_translationHandler;

        // Worlds communication
        void RegisterWorldToEnTT();
        void RegisterEnTTToWorld();
    };

}

#endif