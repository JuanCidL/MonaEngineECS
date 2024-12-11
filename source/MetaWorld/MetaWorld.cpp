#include "MetaWorld.hpp"
#include "EnTTTranslationHandler.hpp"

namespace Mona {

    // MetaWorld Implementation
    MetaWorld::MetaWorld(World& world, EnTTWorld& enttWorld) :
        m_world(world),
        m_enttWorld(enttWorld),
        m_translationHandler(nullptr)
    {
        m_translationHandler = new EnTTTranslationHandler();
        m_translationHandler->SetWorld(this);
    }

    void MetaWorld::InitializeCommunication() {
        RegisterWorldToEnTT();
        RegisterEnTTToWorld();
    }

    World& MetaWorld::GetWorld() {
        return m_world;
    }

    EnTTWorld& MetaWorld::GetEnTTWorld() {
        return m_enttWorld;
    }

    void MetaWorld::RegisterWorldToEnTT() {
        m_translationHandler->SubscribeEnTTToEventManager();
    }

    void MetaWorld::RegisterEnTTToWorld() {
        m_translationHandler->SubscribeEventManagerToEnTT();
    }

}