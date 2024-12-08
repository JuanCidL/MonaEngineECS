#pragma once
#ifndef META_WORLD_H
#define META_WORLD_H
#include "../World/World.hpp"
#include "../ECSWorld/EnTTWorld.hpp"
#include "../Event/EventManager.hpp"

namespace Mona {

    class MetaWorld {
    public:
        friend class EnTTTranslationHandler;
        MetaWorld(World& world, EnTTWorld& enttWorld) :
            m_world(world),
            m_enttWorld(enttWorld),
            m_translationHandler(*this)
        {}

        // Worlds Handling

        void InitializeCommunication() {
            RegisterWorldToEnTT();
            RegisterEnTTToWorld();
        }

        World& GetWorld() {
            return m_world;
        }

        EnTTWorld& GetEnTTWorld() {
            return m_enttWorld;
        }

    private:
        World& m_world;
        EnTTWorld& m_enttWorld;
        EnTTTranslationHandler m_translationHandler;

        // Worlds communication

        void RegisterWorldToEnTT() {
            m_translationHandler.SubscribeEnTTToEventManager();
        }

        void RegisterEnTTToWorld() {
            m_translationHandler.SubscribeEnTTToEventManager();
        }
    };

    // Class to handle the communication between the World event system and the EnTT event system with dispatcher
    class EnTTTranslationHandler {
    public:
        friend class MetaWorld;
        EnTTTranslationHandler(MetaWorld& world) : m_metaWorld(world) {}
        ~EnTTTranslationHandler() = default;
    
    private:
        MetaWorld& m_metaWorld;

        SubscriptionHandle m_windowResizeSubscribtion;
        SubscriptionHandle m_mouseScrollSubscribtion;
        SubscriptionHandle m_gameObjectDestroyedSubscribtion;
        SubscriptionHandle m_applicationEndSubscribtion;
        SubscriptionHandle m_debugGUISubscribtion;
        SubscriptionHandle m_startCollisionSubscribtion;
        SubscriptionHandle m_endCollisionSubscribtion;
        SubscriptionHandle m_customUserSubscribtion;

        void SubscribeEnTTToEventManager() {
            auto& eventManager = m_metaWorld.GetWorld().GetEventManager();
            eventManager.Subscribe(m_windowResizeSubscribtion, this, &EnTTTranslationHandler::EnTTOnWindowResize);
            eventManager.Subscribe(m_mouseScrollSubscribtion, this, &EnTTTranslationHandler::EnTTOnMouseScroll);
            eventManager.Subscribe(m_gameObjectDestroyedSubscribtion, this, &EnTTTranslationHandler::EnTTOnGameObjectDestroyed);
            eventManager.Subscribe(m_applicationEndSubscribtion, this, &EnTTTranslationHandler::EnTTOnApplicationEnd);
            eventManager.Subscribe(m_debugGUISubscribtion, this, &EnTTTranslationHandler::EnTTOnDebugGUI);
            eventManager.Subscribe(m_startCollisionSubscribtion, this, &EnTTTranslationHandler::EnTTOnStartCollision);
            eventManager.Subscribe(m_endCollisionSubscribtion, this, &EnTTTranslationHandler::EnTTOnEndCollision);
            eventManager.Subscribe(m_customUserSubscribtion, this, &EnTTTranslationHandler::OnCustomUser);
        }


        void SubscribeEnTTToEventManager() {
            RegistryEnTTEvent<WindowResizeEvent>();
            RegistryEnTTEvent<MouseScrollEvent>();
            RegistryEnTTEvent<GameObjectDestroyedEvent>();
            RegistryEnTTEvent<ApplicationEndEvent>();
            RegistryEnTTEvent<DebugGUIEvent>();
            RegistryEnTTEvent<CustomUserEvent>();
            RegistryEnTTEvent<StartCollisionEvent>();
            RegistryEnTTEvent<EndCollisionEvent>();
        }

        // World to EnTT Event Translation

        void EnTTOnWindowResize(const WindowResizeEvent& event) {
            m_metaWorld.GetEnTTWorld().GetDispatcher().trigger(event);
        }

        void EnTTOnMouseScroll(const MouseScrollEvent& event) {
            m_metaWorld.GetEnTTWorld().GetDispatcher().trigger(event);
        }

        void EnTTOnGameObjectDestroyed(const GameObjectDestroyedEvent& event) {
            m_metaWorld.GetEnTTWorld().GetDispatcher().trigger(event);
        }

        void EnTTOnApplicationEnd(const ApplicationEndEvent& event) {
            m_metaWorld.GetEnTTWorld().GetDispatcher().trigger(event);
        }

        void EnTTOnDebugGUI(const DebugGUIEvent& event) {
            m_metaWorld.GetEnTTWorld().GetDispatcher().trigger(event);
        }

        void EnTTOnStartCollision(const StartCollisionEvent& event) {
            m_metaWorld.GetEnTTWorld().GetDispatcher().trigger(event);
        }

        void EnTTOnEndCollision(const EndCollisionEvent& event) {
            m_metaWorld.GetEnTTWorld().GetDispatcher().trigger(event);
        }

        void OnCustomUser(const CustomUserEvent& event) {
            m_metaWorld.GetEnTTWorld().GetDispatcher().trigger(event);
        }

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