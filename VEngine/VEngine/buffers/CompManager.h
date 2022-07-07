#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>

// My includes
#include "CompData.h"
#include "MutableBufferOptions.hpp"

namespace buf
{

class CompManager
{

    public:

        template <typename T>
        void register_components(const Id preallocation_size)
        {
            const char* typeName = typeid(T).name();

            assert(m_component_types.find(typeName) == m_component_types.end() && "Error, component already registered");

            m_component_types.insert({typeName, m_next_component_type});
            m_component_datas.insert({typeName, std::make_shared<CompData<T>>()});
            m_next_component_type++;

            get_component_data()->preallocate(preallocation_size);
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
        void add_component(Id id, T component)
        {
            get_compoent_data<T>()->insert_data(id, component);
        }

        template <typename T>
        void remove_component(Id id)
        {
            get_compoent_data<T>()->remove_data(id);
        }

        template <typename T>
        T& get_component(Id id)
        {
            return get_compoent_data<T>()->get_data(id);
        }

        void id_destroyed(Id id)
        {
            for (auto const& [type_name, component] : m_component_datas)
            {
                component->destroy_id(id);
            }
        }

    private:

        std::unordered_map<const char*, ComponentType>              m_component_types{};
        std::unordered_map<const char*, std::shared_ptr<ICompData>> m_component_datas{};
        ComponentType                                               m_next_component_type = 0;


        template <typename T>
        std::shared_ptr<CompData<T>> get_compoent_data()
        {
            const char* typeName = typeid(T).name();

            assert(m_component_types.find(typeName) != m_component_types.end() && "Component not registered before use.");

            return std::static_pointer_cast<CompData<T>>(m_component_datas[typeName]); // Nice little dogy cast ting
        }
};

}; // namespace buf