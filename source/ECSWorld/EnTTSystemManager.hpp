#pragma once
#ifndef ENTT_SYSTEMMANAGER_HPP
#define ENTT_SYSTEMMANAGER_HPP
#include <entt/entt.hpp>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include "./EnTTComponentManager.hpp"
#include "./EnTTEventManager.hpp"
#include "./Systems/BaseSystem.hpp"

namespace Mona
{

    /**
     * Clase que administra los sistemas dentro del marco de trabajo basado en EnTT.
     * Permite agregar, obtener, eliminar y manejar la ejecución de sistemas.
     */
    class EnTTSystemManager
    {
    public:
        /**
         * Constructor que inicializa el administrador de sistemas con referencias al administrador de componentes y eventos.
         * @param componentManager Referencia al administrador de componentes.
         * @param eventManager Referencia al administrador de eventos.
         */
        EnTTSystemManager(EnTTComponentManager &componentManager, EnTTEventManager &eventManager) : m_componentManager(componentManager),
                                                                                                    m_eventManager(eventManager)
        {
        }

        /**
         * Agrega un sistema al administrador.
         * @tparam SystemType Tipo del sistema que debe derivar de BaseSystem.
         * @tparam Args Tipos de los argumentos necesarios para construir el sistema.
         * @param args Argumentos para inicializar el sistema.
         * @return Referencia al sistema agregado.
         */
        template <typename SystemType, typename... Args>
        SystemType &AddSystem(Args &&...args)
        {
            static_assert(std::is_base_of<BaseSystem, SystemType>::value, "SystemType must derive from BaseSystem");
            std::type_index typeIndex = typeid(SystemType);
            if (m_systems.find(typeIndex) != m_systems.end())
            {
                return *static_cast<SystemType *>(m_systems[typeIndex].get());
            }
            auto system = std::make_unique<SystemType>(std::forward<Args>(args)...);
            SystemType &systemRef = *system;
            m_systems[typeIndex] = std::move(system);
            return systemRef;
        }

        /**
         * Obtiene una referencia a un sistema existente.
         * @tparam SystemType Tipo del sistema que se desea obtener.
         * @return Referencia al sistema.
         */
        template <typename SystemType>
        SystemType &GetSystem()
        {
            return *static_cast<SystemType *>(m_systems[typeid(SystemType)].get());
        }

        /**
         * Elimina un sistema del administrador.
         * @tparam SystemType Tipo del sistema que se desea eliminar.
         */
        template <typename SystemType>
        void RemoveSystem()
        {
            m_systems.erase(typeid(SystemType));
        }

        /**
         * Inicializa todos los sistemas registrados.
         * Este método llama al método `StartUp` de cada sistema.
         */
        void StartUp()
        {
            for (auto &[typeIndex, system] : m_systems)
            {
                system->StartUp(m_componentManager, m_eventManager);
            }
        }

        /**
         * Actualiza todos los sistemas registrados.
         * Este método llama al método `Update` de cada sistema, pasando un intervalo de tiempo.
         * @param timeStep Intervalo de tiempo para la actualización.
         */
        void Update(float timeStep)
        {
            for (auto &[typeIndex, system] : m_systems)
            {
                system->Update(m_componentManager, m_eventManager, timeStep);
            }
        }

        /**
         * Finaliza todos los sistemas registrados.
         * Este método llama al método `ShutDown` de cada sistema.
         */
        void ShutDown()
        {
            for (auto &[typeIndex, system] : m_systems)
            {
                system->ShutDown(m_componentManager, m_eventManager);
            }
        }

    private:
        EnTTComponentManager &m_componentManager;
        EnTTEventManager &m_eventManager;
        std::unordered_map<std::type_index, std::unique_ptr<BaseSystem>> m_systems;
    };
}

#endif
