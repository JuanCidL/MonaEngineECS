#include "./EnTTTranslationHandler.hpp"
#include "./MetaWorld.hpp"

namespace Mona {

    // EnTTTranslationHandler Implementation

    void EnTTTranslationHandler::SetWorld(MetaWorld* world) {
        m_metaWorld = world;
    }

    void EnTTTranslationHandler::SubscribeEnTTToEventManager() {
        auto& eventManager = m_metaWorld->GetWorld().GetEventManager();
        eventManager.Subscribe(m_windowResizeSubscribtion, this, &EnTTTranslationHandler::EnTTOnWindowResize);
        eventManager.Subscribe(m_mouseScrollSubscribtion, this, &EnTTTranslationHandler::EnTTOnMouseScroll);
        eventManager.Subscribe(m_gameObjectDestroyedSubscribtion, this, &EnTTTranslationHandler::EnTTOnGameObjectDestroyed);
        eventManager.Subscribe(m_applicationEndSubscribtion, this, &EnTTTranslationHandler::EnTTOnApplicationEnd);
        eventManager.Subscribe(m_debugGUISubscribtion, this, &EnTTTranslationHandler::EnTTOnDebugGUI);
        eventManager.Subscribe(m_startCollisionSubscribtion, this, &EnTTTranslationHandler::EnTTOnStartCollision);
        eventManager.Subscribe(m_endCollisionSubscribtion, this, &EnTTTranslationHandler::EnTTOnEndCollision);
        eventManager.Subscribe(m_customUserSubscribtion, this, &EnTTTranslationHandler::OnCustomUser);
    }

    void EnTTTranslationHandler::SubscribeEventManagerToEnTT() {
        RegistryEnTTEvent<WindowResizeEvent>();
        RegistryEnTTEvent<MouseScrollEvent>();
        RegistryEnTTEvent<GameObjectDestroyedEvent>();
        RegistryEnTTEvent<ApplicationEndEvent>();
        RegistryEnTTEvent<DebugGUIEvent>();
        RegistryEnTTEvent<StartCollisionEvent>();
        RegistryEnTTEvent<EndCollisionEvent>();
        RegistryEnTTEvent<CustomUserEvent>();
    }

    void EnTTTranslationHandler::EnTTOnWindowResize(const WindowResizeEvent& event) {
        m_metaWorld->GetEnTTWorld().GetDispatcher().trigger(event);
    }

    void EnTTTranslationHandler::EnTTOnMouseScroll(const MouseScrollEvent& event) {
        m_metaWorld->GetEnTTWorld().GetDispatcher().trigger(event);
    }

    void EnTTTranslationHandler::EnTTOnGameObjectDestroyed(const GameObjectDestroyedEvent& event) {
        m_metaWorld->GetEnTTWorld().GetDispatcher().trigger(event);
    }

    void EnTTTranslationHandler::EnTTOnApplicationEnd(const ApplicationEndEvent& event) {
        m_metaWorld->GetEnTTWorld().GetDispatcher().trigger(event);
    }

    void EnTTTranslationHandler::EnTTOnDebugGUI(const DebugGUIEvent& event) {
        m_metaWorld->GetEnTTWorld().GetDispatcher().trigger(event);
    }

    void EnTTTranslationHandler::EnTTOnStartCollision(const StartCollisionEvent& event) {
        m_metaWorld->GetEnTTWorld().GetDispatcher().trigger(event);
    }

    void EnTTTranslationHandler::EnTTOnEndCollision(const EndCollisionEvent& event) {
        m_metaWorld->GetEnTTWorld().GetDispatcher().trigger(event);
    }

    void EnTTTranslationHandler::OnCustomUser(const CustomUserEvent& event) {
        m_metaWorld->GetEnTTWorld().GetDispatcher().trigger(event);
    }

}