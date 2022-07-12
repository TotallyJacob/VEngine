#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>

// My includes
#include "CompData.h"
#include "MutableBufferOptions.hpp"

namespace buf
{

#define COMP_MANAGER_IDENTIFIER <COMP_TYPE, sizes...>
#define COMP_MANAGER_TEMPLATE   template <template <typename, size_t...> class COMP_TYPE, size_t... sizes>

COMP_MANAGER_TEMPLATE
class CompManager
{

    public:

        CompManager() = default;

        template <typename DATA_TYPE>
        void register_component(const Id preallocation_size = 0);

        template <typename DATA_TYPE>
        ComponentType get_component_type();

        template <typename DATA_TYPE>
        void add_component(Id id, DATA_TYPE component);

        template <typename DATA_TYPE>
        void remove_component(Id id);

        template <typename DATA_TYPE>
        auto get_component(Id id) -> DATA_TYPE&;

        void id_destroyed(Id id);

    private:

        std::unordered_map<const char*, ComponentType>              m_component_types{};
        std::unordered_map<const char*, std::shared_ptr<ICompData>> m_component_datas{};
        ComponentType                                               m_next_component_type = 0;

        template <typename T>
        constexpr static auto assert_base_class() -> bool // LOL huge circular help discussion for this
        {
            static_assert(std::is_base_of<ICompData, T>(), "COMP_TYPE must inherit from ICompData.");

            return true;
        }


        template <typename DATA_TYPE>
        auto get_component_data() -> std::shared_ptr<COMP_TYPE<DATA_TYPE, sizes...>>;
};

}; // namespace buf

#include "CompManager.ipp"

#undef COMP_MANAGER_IDENTIFIER
#undef COMP_MANAGER_TEMPLATE