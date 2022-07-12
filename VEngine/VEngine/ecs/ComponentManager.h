#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>

// My includes
#include "ComponentArray.h"
#include "EntityManager.h"

namespace vengine
{


class ComponentManager
{
    public:

        template <typename T>
        void register_components()
        {
            const char* typeName = typeid(T).name();

            assert(m_component_types.find(typeName) == m_component_types.end() && "Error, component already registered");

            m_component_types.insert({typeName, m_next_component_type});
            m_component_arrays.insert({typeName, std::make_shared<ComponentArray<T>>()});
            m_next_component_type++;
        }

        template <typename T>
        ComponentType get_component_type()
        {
            const char* typeName = typeid(T).name();

            assert(m_component_types.find(typeName) != m_component_types.end() &&
                   "Error, trying to get componentType from a component that isn't registered.");

            return m_component_types[typeName];
        }

        template <typename T>
        void add_component(Entity entity, T component)
        {
            get_compoent_array<T>()->insert_data(entity, component);
        }

        template <typename T>
        void remove_component(Entity entity)
        {
            get_compoent_array<T>()->remove_data(entity);
        }

        template <typename T>
        T& get_component(Entity entity)
        {
            return get_compoent_array<T>()->get_data(entity);
        }

        void entity_destroyed(Entity entity);

    private:

        std::unordered_map<const char*, ComponentType>                    m_component_types{};
        std::unordered_map<const char*, std::shared_ptr<IComponentArray>> m_component_arrays{};
        ComponentType                                                     m_next_component_type = 0;


        template <typename T>
        std::shared_ptr<ComponentArray<T>> get_compoent_array()
        {
            const char* typeName = typeid(T).name();

            assert(m_component_types.find(typeName) != m_component_types.end() && "Component not registered before use.");

            return std::static_pointer_cast<ComponentArray<T>>(m_component_arrays[typeName]); // Nice little dogy cast ting
        }
};

}; // namespace vengine
