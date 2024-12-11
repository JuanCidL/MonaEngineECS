#pragma once
#ifndef ENTT_EVENT_MANAGER_HPP
#define ENTT_EVENT_MANAGER_HPP
#include <entt/entt.hpp>

namespace Mona
{

    /**
     * Clase que administra eventos en el sistema utilizando EnTT.
     * Permite la suscripción, desuscripción y publicación de eventos.
     */
    class EnTTEventManager
    {
    public:
        /**
         * Constructor que inicializa el administrador de eventos con un despachador de EnTT.
         * @param dispatcher Referencia al despachador de EnTT utilizado para gestionar los eventos.
         */
        EnTTEventManager(entt::dispatcher &dispatcher) : m_dispatcher(dispatcher)
        {
        }

        /**
         * Destructor por defecto.
         */
        ~EnTTEventManager() = default;

        /**
         * Suscribe una función libre (no miembro) a un evento específico.
         * @tparam EventType Tipo del evento al que se desea suscribir.
         * @param freeFunction Puntero a la función libre que manejará el evento.
         */
        template <typename EventType>
        void Subscribe(void (*freeFunction)(const EventType &))
        {
            m_dispatcher.sink<EventType>().connect<freeFunction>();
        }

        /**
         * Suscribe una función miembro a un evento específico.
         * @tparam EventType Tipo del evento al que se desea suscribir.
         * @tparam ObjType Tipo del objeto que contiene la función miembro.
         * @param obj Puntero al objeto que manejará el evento.
         * @param memberFunction Puntero a la función miembro que manejará el evento.
         */
        template <typename EventType, typename ObjType>
        void Subscribe(ObjType *obj, void (ObjType::*memberFunction)(const EventType &))
        {
            m_dispatcher.sink<EventType>().connect<ObjType, &ObjType::memberFunction>(obj);
        }

        /**
         * Desuscribe una función libre (no miembro) de un evento específico.
         * @tparam EventType Tipo del evento del que se desea desuscribir.
         * @param freeFunction Puntero a la función libre que manejará el evento.
         */
        template <typename EventType>
        void Unsubscribe(void (*freeFunction)(const EventType &))
        {
            m_dispatcher.sink<EventType>().disconnect<freeFunction>();
        }

        /**
         * Desuscribe una función miembro de un evento específico.
         * @tparam EventType Tipo del evento del que se desea desuscribir.
         * @tparam ObjType Tipo del objeto que contiene la función miembro.
         * @param obj Puntero al objeto que manejará el evento.
         * @param memberFunction Puntero a la función miembro que manejará el evento.
         */
        template <typename EventType, typename ObjType>
        void Unsubscribe(ObjType *obj, void (ObjType::*memberFunction)(const EventType &))
        {
            m_dispatcher.sink<EventType>().template disconnect<ObjType, &ObjType::memberFunction>(obj);
        }

        /**
         * Publica un evento específico para notificar a todos los suscriptores.
         * @tparam EventType Tipo del evento a publicar.
         * @param event Instancia del evento que se publicará.
         */
        template <typename EventType>
        void Publish(const EventType &event)
        {
            m_dispatcher.trigger(event);
        }

    private:
        entt::dispatcher &m_dispatcher;
    };
}

#endif
