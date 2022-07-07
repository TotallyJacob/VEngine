#pragma once

#include <assert.h>
#include <unordered_map>
#include <vector>

#include "ICompData.h"

namespace buf
{

#define COMP_DATA_TEMPLATE   template <typename T>
#define COMP_DATA_IDENTIFIER <T>

COMP_DATA_TEMPLATE
class CompData : public ICompData
{
    public:

        void preallocate(const Id preallocation_size);

        void insert_data(Id id, T component);
        void remove_data(Id id);

        auto get_data(Id id) -> T&;

        void destroy_id(Id id) override;

    protected:

        std::vector<T>                 m_component_data{};
        std::unordered_map<Id, size_t> m_Id_to_index_map{};
        std::unordered_map<size_t, Id> m_index_to_Id_map{};
        size_t                         m_size = 0;
};

}; // namespace buf

#include "CompData.ipp"

#undef COMP_DATA_TEMPLATE
#undef COMP_DATA_IDENTIFIER