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

        // Worlds Handling
        void InitializeCommunication();
        World& GetWorld();
        EnTTWorld& GetEnTTWorld();

    private:
        World& m_world;
        EnTTWorld& m_enttWorld;
        EnTTTranslationHandler m_translationHandler;

        // Worlds communication
        void RegisterWorldToEnTT();
        void RegisterEnTTToWorld();
    };

    // Class to handle the communication between the World event system and the EnTT event system with dispatcher
    class EnTTTranslationHandler {
    public:
        friend class MetaWorld;
        EnTTTranslationHandler(MetaWorld& world);
        ~EnTTTranslationHandler() = default;
    
    private:
        MetaWorld& m_metaWorld;

        // Event Subscriptions for Event Manager
        SubscriptionHandle m_windowResizeSubscribtion;
        SubscriptionHandle m_mouseScrollSubscribtion;
        SubscriptionHandle m_gameObjectDestroyedSubscribtion;
        SubscriptionHandle m_applicationEndSubscribtion;
        SubscriptionHandle m_debugGUISubscribtion;
        SubscriptionHandle m_startCollisionSubscribtion;
        SubscriptionHandle m_endCollisionSubscribtion;
        SubscriptionHandle m_customUserSubscribtion;

        // Communication initialization
        void SubscribeEnTTToEventManager();
        void SubscribeEventManagerToEnTT();

        // World to EnTT Event Translation
        void EnTTOnWindowResize(const WindowResizeEvent& event);
        void EnTTOnMouseScroll(const MouseScrollEvent& event);
        void EnTTOnGameObjectDestroyed(const GameObjectDestroyedEvent& event);
        void EnTTOnApplicationEnd(const ApplicationEndEvent& event);
        void EnTTOnDebugGUI(const DebugGUIEvent& event);
        void EnTTOnStartCollision(const StartCollisionEvent& event);
        void EnTTOnEndCollision(const EndCollisionEvent& event);
        void OnCustomUser(const CustomUserEvent& event);

        // EnTT to World Event Translation
        template <typename EventType>
        void RegistryEnTTEvent() {
            m_metaWorld.GetEnTTWorld().GetDispatcher().sink<EventType>().template connect<&EnTTTranslationHandler::OnEnTTEvent<EventType>>(*this);
        }

        template <typename EventType>
        void OnEnTTEvent(const EventType& event) {
            m_metaWorld.GetWorld().GetEventManager().Publish(event);
        }
    };

}

#endif