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
            m_dispatcher.sink<EventType>().template connect<freeFunction>();
        }
        
        // Subscribe to an event with a member function
        template<typename EventType, typename ObjType>
        void Subscribe(ObjType* obj, void (ObjType::* memberFunction)(const EventType&)) {
            m_dispatcher.sink<EventType>().template connect<ObjType, &ObjType::memberFunction>(obj);
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