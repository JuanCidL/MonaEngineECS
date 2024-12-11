#pragma once
#ifndef ENTT_EVENT_MANAGER_HPP
#define ENTT_EVENT_MANAGER_HPP
#include <entt/entt.hpp>

namespace Mona {

    class EnTTEventManager {
    public:
        EnTTEventManager(entt::dispatcher& dispatcher) :
            m_dispatcher(dispatcher)
        {}
        ~EnTTEventManager() = default;

        // Subscribe to an event with a non-member function
        template<typename EventType>
        void Subscribe(void (*freeFunction)(const EventType&)) {
            m_dispatcher.sink<EventType>().connect<freeFunction>();
        }
        
        // Subscribe to an event with a member function
        template<typename EventType, typename ObjType>
        void Subscribe(ObjType* obj, void (ObjType::* memberFunction)(const EventType&)) {
            m_dispatcher.sink<EventType>().connect<ObjType, &ObjType::memberFunction>(obj);
        }

        // Unsubscribe from an event with a non-member function
        template<typename EventType>
        void Unsubscribe(void (*freeFunction)(const EventType&)) {
            m_dispatcher.sink<EventType>().disconnect<freeFunction>();
        }

        // Unsubscribe from an event with a member function
        template<typename EventType, typename ObjType>
        void Unsubscribe(ObjType* obj, void (ObjType::* memberFunction)(const EventType&)) {
            m_dispatcher.sink<EventType>().template disconnect<ObjType, &ObjType::memberFunction>(obj);
        }

        // Publish an event
        template<typename EventType>
        void Publish(const EventType& event) {
            m_dispatcher.trigger(event);
        }

    private:
    
        entt::dispatcher& m_dispatcher;
    };
}

#endif