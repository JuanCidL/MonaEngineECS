#include "./EnTTTranslationHandler.hpp"
#include "./MetaWorld.hpp"

namespace Mona
{

    /**
     * etter del MetaWorld
     *
     * @param world Puntero a la clase MetaWorld que se esta utilizando.
     */
    void EnTTTranslationHandler::SetWorld(MetaWorld *world)
    {
        m_metaWorld = world;
    }

    /**
     * Suscribe el EnTTTranslationHandler a los eventos del EventManager del World.
     */
    void EnTTTranslationHandler::SubscribeEnTTToEventManager()
    {
        auto &eventManager = m_metaWorld->GetWorld().GetEventManager();
        eventManager.Subscribe(m_windowResizeSubscribtion, this, &EnTTTranslationHandler::EnTTOnWindowResize);
        eventManager.Subscribe(m_mouseScrollSubscribtion, this, &EnTTTranslationHandler::EnTTOnMouseScroll);
        eventManager.Subscribe(m_gameObjectDestroyedSubscribtion, this, &EnTTTranslationHandler::EnTTOnGameObjectDestroyed);
        eventManager.Subscribe(m_applicationEndSubscribtion, this, &EnTTTranslationHandler::EnTTOnApplicationEnd);
        eventManager.Subscribe(m_debugGUISubscribtion, this, &EnTTTranslationHandler::EnTTOnDebugGUI);
        eventManager.Subscribe(m_startCollisionSubscribtion, this, &EnTTTranslationHandler::EnTTOnStartCollision);
        eventManager.Subscribe(m_endCollisionSubscribtion, this, &EnTTTranslationHandler::EnTTOnEndCollision);
        eventManager.Subscribe(m_customUserSubscribtion, this, &EnTTTranslationHandler::OnCustomUser);
    }

    /**
     * Suscribe los eventos normales a los eventos de EnTT.
     */
    void EnTTTranslationHandler::SubscribeEventManagerToEnTT()
    {
        RegistryEnTTEvent<WindowResizeEvent>();
        RegistryEnTTEvent<MouseScrollEvent>();
        RegistryEnTTEvent<GameObjectDestroyedEvent>();
        RegistryEnTTEvent<ApplicationEndEvent>();
        RegistryEnTTEvent<DebugGUIEvent>();
        RegistryEnTTEvent<StartCollisionEvent>();
        RegistryEnTTEvent<EndCollisionEvent>();
        RegistryEnTTEvent<CustomUserEvent>();
    }

    /**
     * Metodo que se encarga de reenviar el evento de cambio de tamaño de ventana
     * desde el EventManager del World hacia el EnTTWorld.
     *
     * @param event Evento de cambio de tamaño de ventana.
     */
    void EnTTTranslationHandler::EnTTOnWindowResize(const WindowResizeEvent &event)
    {
        m_metaWorld->GetEnTTWorld().GetDispatcher().trigger(event);
    }

    /**
     * Metodo que se encarga de reenviar el evento de scroll de mouse
     * desde el EventManager del World hacia el EnTTWorld.
     *
     * @param event Evento de scroll de mouse.
     */
    void EnTTTranslationHandler::EnTTOnMouseScroll(const MouseScrollEvent &event)
    {
        m_metaWorld->GetEnTTWorld().GetDispatcher().trigger(event);
    }

    /**
     * Metodo que se encarga de reenviar el evento de destrucción de objeto del juego
     * desde el EventManager del World hacia el EnTTWorld.
     *
     * @param event Evento de destrucción de objeto del juego.
     */
    void EnTTTranslationHandler::EnTTOnGameObjectDestroyed(const GameObjectDestroyedEvent &event)
    {
        m_metaWorld->GetEnTTWorld().GetDispatcher().trigger(event);
    }

    /**
     * Metodo que se encarga de reenviar el evento de finalización de la aplicación
     * desde el EventManager del World hacia el EnTTWorld.
     *
     * @param event Evento de finalización de la aplicación.
     */
    void EnTTTranslationHandler::EnTTOnApplicationEnd(const ApplicationEndEvent &event)
    {
        m_metaWorld->GetEnTTWorld().GetDispatcher().trigger(event);
    }

    /**
     * Metodo que se encarga de reenviar el evento de depuración de la interfaz
     * desde el EventManager del World hacia el EnTTWorld.
     *
     * @param event Evento de depuración de la interfaz.
     */
    void EnTTTranslationHandler::EnTTOnDebugGUI(const DebugGUIEvent &event)
    {
        m_metaWorld->GetEnTTWorld().GetDispatcher().trigger(event);
    }

    /**
     * Metodo que se encarga de reenviar el evento de inicio de colision entre
     * objetos del juego desde el EventManager del World hacia el EnTTWorld.
     *
     * @param event Evento de inicio de colision entre objetos del juego.
     */
    void EnTTTranslationHandler::EnTTOnStartCollision(const StartCollisionEvent &event)
    {
        m_metaWorld->GetEnTTWorld().GetDispatcher().trigger(event);
    }

    /**
     * Metodo que se encarga de reenviar el evento de fin de colision entre
     * objetos del juego desde el EventManager del World hacia el EnTTWorld.
     *
     * @param event Evento de fin de colision entre objetos del juego.
     */
    void EnTTTranslationHandler::EnTTOnEndCollision(const EndCollisionEvent &event)
    {
        m_metaWorld->GetEnTTWorld().GetDispatcher().trigger(event);
    }

    /**
     * Metodo que se encarga de reenviar el evento de usuario personalizado
     * desde el EventManager del World hacia el EnTTWorld.
     *
     * @param event Evento de usuario personalizado.
     */
    void EnTTTranslationHandler::OnCustomUser(const CustomUserEvent &event)
    {
        m_metaWorld->GetEnTTWorld().GetDispatcher().trigger(event);
    }

}