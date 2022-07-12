#pragma once

#include <array>
#include <assert.h>
#include <unordered_map>

#include "ICompData.h"
#include "MutableBufferOptions.hpp"

namespace buf
{

#define STATIC_COMP_DATA_IDENTIFIER <T, num_ids>
#define STATIC_COMP_DATA_TEMPLATE   template <typename T, size_t num_ids>

STATIC_COMP_DATA_TEMPLATE
class StaticCompData : public ICompData
{
    public:

        void insert_data(Id id, T component);
        void remove_data(Id id);

        auto get_data(Id id) -> T&;

        void destroy_id(Id id) override;

    protected:

        std::array<T, num_ids>         m_component_data{};
        std::unordered_map<Id, size_t> m_Id_to_index_map{};
        std::unordered_map<size_t, Id> m_index_to_Id_map{};
        size_t                         m_size = 0;
};

}; // namespace buf

#include "StaticCompData.ipp"

#undef STATIC_COMP_DATA_TEMPLATE
#undef STATIC_COMP_DATA_IDENTIFIER