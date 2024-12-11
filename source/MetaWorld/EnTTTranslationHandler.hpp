#pragma once
#ifndef ENTT_TRANSLATION_HANDLER_H
#define ENTT_TRANSLATION_HANDLER_H
#include "../Event/Events.hpp"

namespace Mona
{
    class MetaWorld;
    
    // Class to handle the communication between the World event system and the EnTT event system with dispatcher
    class EnTTTranslationHandler {
    public:
        friend class MetaWorld;
        EnTTTranslationHandler() = default;
        ~EnTTTranslationHandler() = default;
    
    private:
        MetaWorld* m_metaWorld;

        // Event Subscriptions for Event Manager
        SubscriptionHandle m_windowResizeSubscribtion;
        SubscriptionHandle m_mouseScrollSubscribtion;
        SubscriptionHandle m_gameObjectDestroyedSubscribtion;
        SubscriptionHandle m_applicationEndSubscribtion;
        SubscriptionHandle m_debugGUISubscribtion;
        SubscriptionHandle m_startCollisionSubscribtion;
        SubscriptionHandle m_endCollisionSubscribtion;
        SubscriptionHandle m_customUserSubscribtion;

        void SetWorld(MetaWorld* world) {
            m_metaWorld = world;
        }

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
            m_metaWorld->GetEnTTWorld().GetDispatcher().sink<EventType>().template connect<&EnTTTranslationHandler::OnEnTTEvent<EventType>>(*this);
        }

        template <typename EventType>
        void OnEnTTEvent(const EventType& event) {
            m_metaWorld->GetWorld().GetEventManager().Publish(event);
        }
    };
}

#endif