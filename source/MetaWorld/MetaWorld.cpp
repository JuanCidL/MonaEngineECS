#include "MetaWorld.hpp"
#include "EnTTTranslationHandler.hpp"

namespace Mona
{

    /**
     * Constructor de la clase MetaWorld.
     *
     * @param world Referencia al World que sera manejado por este objeto.
     * @param enttWorld Referencia al EnTTWorld que sera manejado por este objeto.
     *
     * Crea un objeto MetaWorld que se encargara de manejar la comunicacion entre
     * el World y el EnTTWorld.
     */
    MetaWorld::MetaWorld(World &world, EnTTWorld &enttWorld) : m_world(world),
                                                               m_enttWorld(enttWorld),
                                                               m_translationHandler(nullptr)
    {
        m_translationHandler = new EnTTTranslationHandler();
        m_translationHandler->SetWorld(this);
    }

    /**
     * Metodo que inicializa la comunicacion entre el World y el EnTTWorld.
     *
     * Registra los eventos del World en el EnTTWorld y viceversa.
     */
    void MetaWorld::InitializeCommunication()
    {
        RegisterWorldToEnTT();
        RegisterEnTTToWorld();
    }

    /**
     * Getter de la clase World asociada a MetaWorld.
     */
    World &MetaWorld::GetWorld()
    {
        return m_world;
    }
    /**
     * Getter de la clase EnTTWorld asociada a MetaWorld.
     */
    EnTTWorld &MetaWorld::GetEnTTWorld()
    {
        return m_enttWorld;
    }

    /**
     * Metodo que se encarga de suscribir los eventos del EnTTWorld
     * a los eventos del World, permitiendo la comunicación entre estos.
     */
    void MetaWorld::RegisterWorldToEnTT()
    {
        m_translationHandler->SubscribeEnTTToEventManager();
    }

    /**
     * Metodo que se encarga de suscribir los eventos del World
     * a los eventos del EnTTWorld, permitiendo la comunicación entre estos.
     */
    void MetaWorld::RegisterEnTTToWorld()
    {
        m_translationHandler->SubscribeEventManagerToEnTT();
    }

}