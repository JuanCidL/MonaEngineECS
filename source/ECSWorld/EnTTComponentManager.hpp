#pragma once
#ifndef ENTT_COMPONENT_MANAGER_H
#define ENTT_COMPONENT_MANAGER_H
#include "../World/ComponentTypes.hpp"
#include <entt/entt.hpp>
#include <typeindex>
#include "unordered_map"

namespace Mona
{
    /**
     * Clase que representa el ComponentManager en la solución prpuesta. Provee métodos para el acceso y
     * modificación de compnentes, funcionando cómo un wrapper par EnTT.
     */
    class EnTTComponentManager
    {
    public:
        /**
         * Constructor que inicializa el administrador de componentes con un registro de EnTT.
         * @param registry Referencia al registro de EnTT utilizado para gestionar las entidades y sus componentes.
         */
        EnTTComponentManager(entt::registry &registry) : m_registry(registry), m_componentCount{} {}

        /**
         * Agrega un componente a una entidad específica.
         * @tparam ComponentType Tipo del componente a agregar.
         * @tparam Args Tipos de los argumentos necesarios para construir el componente.
         * @param entity Entidad a la que se agregará el componente.
         * @param args Argumentos utilizados para inicializar el componente.
         * @return Referencia al componente agregado.
         */
        template <typename ComponentType, typename... Args>
        ComponentType &AddComponent(entt::entity entity, Args &&...args)
        {
            if (m_componentCount.find(ComponentType) == m_componentCount.end())
            {
                m_componentCount[ComponentType] = 1;
            }
            else
            {
                m_componentCount[ComponentType]++;
            }
            return m_registry.emplace<ComponentType>(entity, std::forward<Args>(args)...);
        }

        /**
         * Elimina un componente de una entidad específica.
         * @tparam ComponentType Tipo del componente a eliminar.
         * @param entity Entidad de la que se eliminará el componente.
         */
        template <typename ComponentType>
        void RemoveComponent(entt::entity entity)
        {
            m_registry.remove<ComponentType>(entity);
            m_componentCount[ComponentType]--;
        }

        /**
         * Verifica si una entidad tiene un componente específico.
         * @tparam ComponentType Tipo del componente a verificar.
         * @param entity Entidad a verificar.
         * @return `true` si la entidad tiene el componente, `false` en caso contrario.
         */
        template <typename ComponentType>
        bool HasComponent(entt::entity entity)
        {
            return m_registry.all_of<ComponentType>(entity);
        }

        /**
         * Obtiene una referencia al componente de una entidad específica.
         * @tparam ComponentType Tipo del componente a obtener.
         * @param entity Entidad de la que se obtendrá el componente.
         * @return Referencia al componente.
         */
        template <typename ComponentType>
        ComponentType &GetComponent(entt::entity entity)
        {
            return m_registry.get<ComponentType>(entity);
        }

        /**
         * Obtiene la cantidad de instancias de un componente específico.
         * @tparam ComponentType Tipo del componente del que se quiere conocer la cantidad.
         * @return Número de instancias del componente.
         */
        template <typename ComponentType>
        uint32_t GetComponentCount()
        {
            return m_componentCount[std::type_index(typeid(ComponentType))];
        }

        /**
         * Itera sobre todas las combinaciones de componentes especificadas y ejecuta una función.
         * @tparam ComponentTypes Tipos de componentes sobre los que se iterará.
         * @tparam Func Tipo de la función que se ejecutará.
         * @param func Función a ejecutar por cada combinación de componentes.
         */
        template <typename... ComponentTypes, typename Func>
        void ForEach(Func func)
        {
            m_registry.view<ComponentTypes...>().each(func);
        }

        /**
         * Obtiene una referencia al registro interno de EnTT.
         * @return Referencia al registro de EnTT.
         */
        entt::registry &GetRegistry()
        {
            return m_registry;
        }

    private:
        entt::registry &m_registry;
        std::unordered_map<std::type_index, uint32_t> m_componentCount;
    };
}

#endif