#pragma once
#ifndef ENTT_WORLD_H
#define ENTT_WORLD_H
#include <entt/entt.hpp>
#include "EnTTComponentManager.hpp"
#include "EnTTSystemManager.hpp"
#include "EnTTEventManager.hpp"
#include "./systems/AudioEnttSystem.hpp"
#include "./Systems/Systems.hpp"
#include "../World/ComponentTypes.hpp"

namespace Mona
{

    /**
     * Clase principal que representa el mundo del juego, manejando entidades,
     * componentes, eventos y sistemas.
     */
    class EnTTWorld
    {
        friend class EnttAudioSystem;
        friend class RenderingSystem;

    public:
        /**
         * Constructor que inicializa un nuevo mundo con todos sus administradores.
         */
        EnTTWorld() : m_registry(),
                      m_dispatcher(),
                      m_componentManager(m_registry),
                      m_entityCount(0),
                      m_eventManager(m_dispatcher),
                      m_systemManager(m_componentManager, m_eventManager),
                      m_currentCamera(nullptr),
                      m_ambientLight(glm::vec3{0.1f})
        {
            auto worldEntity = m_registry.create();
            m_registry.emplace<EnTTWorld *>(worldEntity, this);
        }

        EnTTWorld(const EnTTWorld &world) = delete;            ///< Constructor de copia eliminado.
        EnTTWorld &operator=(const EnTTWorld &world) = delete; ///< Operador de asignación eliminado.

        /**
         * Obtiene una referencia al registro de entidades de EnTT.
         * @return Referencia al registro.
         */
        entt::registry &GetRegistry()
        {
            return m_registry;
        }

        /**
         * Obtiene una referencia al despachador de eventos de EnTT.
         * @return Referencia al despachador.
         */
        entt::dispatcher &GetDispatcher()
        {
            return m_dispatcher;
        }

        /**
         * Obtiene la cantidad actual de objetos del juego.
         * @return Número de objetos del juego.
         */
        size_t GetGameObjectCount()
        {
            return m_entityCount;
        }

        /**
         * Crea un nuevo objeto del juego.
         * @return Identificador de la nueva entidad.
         */
        entt::entity CreateGameObject()
        {
            m_entityCount++;
            return m_registry.create();
        }

        /**
         * Destruye un objeto del juego dado su identificador.
         * @param entity Identificador de la entidad a destruir.
         */
        void DestroyGameObject(entt::entity entity)
        {
            m_registry.destroy(entity);
            m_entityCount--;
        }

        /**
         * Agrega un componente a una entidad específica.
         * @tparam ComponentType Tipo del componente.
         * @tparam Args Tipos de los argumentos para construir el componente.
         * @param entity Entidad a la que se agregará el componente.
         * @param args Argumentos para inicializar el componente.
         */
        template <typename ComponentType, typename... Args>
        void AddComponent(entt::entity entity, Args &&...args)
        {
            m_componentManager.AddComponent<ComponentType>(entity, std::forward<Args>(args)...);
        }

        /**
         * Elimina un componente de una entidad específica.
         * @tparam ComponentType Tipo del componente a eliminar.
         * @param entity Entidad de la que se eliminará el componente.
         */
        template <typename ComponentType>
        void RemoveComponent(entt::entity entity)
        {
            m_componentManager.RemoveComponent<ComponentType>(entity);
        }

        /**
         * Verifica si una entidad tiene un componente específico.
         * @tparam ComponentType Tipo del componente.
         * @param entity Entidad a verificar.
         * @return Verdadero si la entidad tiene el componente, falso en caso contrario.
         */
        template <typename ComponentType>
        bool HasComponent(entt::entity entity)
        {
            return m_componentManager.HasComponent<ComponentType>(entity);
        }

        /**
         * Obtiene una referencia a un componente de una entidad específica.
         * @tparam ComponentType Tipo del componente.
         * @param entity Entidad de la que se obtendrá el componente.
         * @return Referencia al componente.
         */
        template <typename ComponentType>
        ComponentType &GetComponent(entt::entity entity)
        {
            return m_componentManager.GetComponent<ComponentType>(entity);
        }

        /**
         * Itera sobre varias entidades que tienen componentes específicos y ejecuta una función para cada una.
         * @tparam ComponentTypes Tipos de los componentes requeridos.
         * @tparam Func Tipo de la función que se ejecutará.
         * @param func Función a ejecutar.
         */
        template <typename... ComponentTypes, typename Func>
        void ForEachComponents(Func func)
        {
            m_componentManager.ForEach<ComponentTypes...>(func);
        }

        /**
         * Agrega un sistema al mundo.
         * @tparam SystemType Tipo del sistema a agregar.
         */
        template <typename SystemType>
        void AddSystem()
        {
            m_systemManager.AddSystem<SystemType>();
        }

        /**
         * Elimina un sistema del mundo.
         * @tparam SystemType Tipo del sistema a eliminar.
         */
        template <typename SystemType>
        void RemoveSystem()
        {
            m_systemManager.RemoveSystem<SystemType>();
        }

        /**
         * Actualiza todos los sistemas registrados en el mundo.
         * @param timeStep Intervalo de tiempo para la actualización.
         */
        void UpdateSystems(float timeStep)
        {
            m_systemManager.Update(timeStep);
        }

        /**
         * Establece la cámara actual del mundo.
         * @param camera Puntero a la entidad de la cámara.
         */
        void SetCurrentCamera(entt::entity *camera)
        {
            m_currentCamera = camera;
        }

        /**
         * Obtiene la cámara actual del mundo.
         * @return Puntero a la entidad de la cámara actual.
         */
        entt::entity *GetCurrentCamera()
        {
            return m_currentCamera;
        }

    private:
        entt::registry m_registry;               ///< Registro de entidades del mundo.
        entt::dispatcher m_dispatcher;           ///< Despachador de eventos.
        EnTTComponentManager m_componentManager; ///< Administrador de componentes.
        size_t m_entityCount;                    ///< Conteo de entidades.
        EnTTSystemManager m_systemManager;       ///< Administrador de sistemas.

        glm::vec3 m_ambientLight;                                                      ///< Luz ambiental del mundo.
        EnTTEventManager m_eventManager;                                               ///< Administrador de eventos.
        glm::fquat m_audioListenerOffsetRotation = glm::fquat(1.0f, 0.0f, 0.0f, 0.0f); ///< Rotación del oyente de audio.
        entt::entity *m_currentCamera;                                                 ///< Cámara actual del mundo.
    };
}

#endif
