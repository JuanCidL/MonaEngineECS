#pragma once
#ifndef ENTT_EVENT_MANAGER_HPP
#define ENTT_EVENT_MANAGER_HPP
#include <entt/entt.hpp>
#include <functional>

namespace Mona
{
    class EnTTEventManager
    {
    public:
        EnTTEventManager() = default;
        ~EnTTEventManager() = default;

        // Event subscription using a function pointer
        template<typename EventType, void(*func)(const EventType &)>
        void Subscribe()
        {
            m_dispatcher.sink<EventType>().template connect<func>();
        }

        // Event subscription using a member function pointer
        template<typename EventType, typename ObjectType, void(ObjectType::*func)(const EventType &)>
        void Subscribe(ObjectType& obj)
        {
            m_dispatcher.sink<EventType>().template connect<func>(obj);
        }

        template<typename EventType>
        void Publish(const EventType& event)
        {
            m_dispatcher.trigger(event);
        }

        template<typename EventType, void(*func)(const EventType &)>
        void Unsubscribe()
        {
            m_dispatcher.sink<EventType>().template disconnect<func>();
        }

        template<typename EventType, typename ObjectType, void(ObjectType::*func)(const EventType &)>
        void Unsubscribe(ObjectType& obj)
        {
            m_dispatcher.sink<EventType>().template disconnect<func>(obj);
        }
        
    private:
        entt::dispatcher m_dispatcher{};
    };   
}

#endif