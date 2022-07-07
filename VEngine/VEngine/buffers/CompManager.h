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
        void register_components(const Id preallocation_size);

        template <typename T>
        ComponentType get_component_type();

        template <typename T>
        void add_component(Id id, T component);

        template <typename T>
        void remove_component(Id id);

        template <typename T>
        auto get_component(Id id) -> T&;

        void id_destroyed(Id id);

    private:

        std::unordered_map<const char*, ComponentType>              m_component_types{};
        std::unordered_map<const char*, std::shared_ptr<ICompData>> m_component_datas{};
        ComponentType                                               m_next_component_type = 0;


        template <typename T>
        auto get_compoent_data() -> std::shared_ptr<CompData<T>>;
};

}; // namespace buf

#include "CompManager.ipp"