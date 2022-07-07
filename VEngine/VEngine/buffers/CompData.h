#pragma once

#include <assert.h>
#include <unordered_map>
#include <vector>

#include "ICompData.h"

namespace buf
{

template <typename T>
class CompData : public ICompData
{
    public:

        void preallocate(const Id preallocation_size)
        {
            m_component_data.resize(preallocation_size);
            m_Id_to_index_map.reserve(preallocation_size);
            m_index_to_Id_map.reserve(preallocation_size);
        }

        void insert_data(Id id, T component)
        {
            assert(m_Id_to_index_map.find(id) == m_Id_to_index_map.end() && "Component added to same buffer more than once.");

            size_t newIndex = m_size;

            // Manage if there is currently no space to insert the data
            if (m_Id_to_index_map.find(id) == m_Id_to_index_map.end())
            {
                m_Id_to_index_map.emplace(id, 0);
            }

            if (m_index_to_Id_map.find(newIndex) == m_index_to_Id_map.end())
            {
                m_index_to_Id_map.emplace(newIndex, 0);
            }

            if (m_component_data.size() <= newIndex)
            {
                m_component_data.resize(newIndex + 1);
            }

            // Actually set the data
            m_Id_to_index_map[id] = newIndex;
            m_index_to_Id_map[newIndex] = id;
            m_component_data[newIndex] = component;
            m_size++;
        }

        void remove_data(Id id)
        {
            // Switch the component array to the last element
            size_t indexOfRemovedEntity = m_Id_to_index_map[id];
            size_t indexOfLastElement = m_size - 1;
            m_component_data[indexOfRemovedEntity] = m_component_data[indexOfLastElement];

            // Change the entity and index to become the last element
            Id entityOfLastElement = m_index_to_Id_map[indexOfLastElement];
            m_Id_to_index_map[entityOfLastElement] = indexOfRemovedEntity;
            m_index_to_Id_map[indexOfRemovedEntity] = entityOfLastElement;

            // Removed the previous last element and element to remove stuff
            m_Id_to_index_map.erase(id);
            m_index_to_Id_map.erase(indexOfLastElement);


            --m_size;
        }

        T& get_data(Id id)
        {
            assert(m_Id_to_index_map.find(id) != m_Id_to_index_map.end() && "Error, Buffer component doesn't exist");

            return m_component_data[m_Id_to_index_map[id]];
        }

        void destroy_id(Id id) override
        {
            if (m_Id_to_index_map.find(id) != m_Id_to_index_map.end())
            {
                remove_data(id);
            }
        }


    protected:

        std::vector<T>                 m_component_data{};
        std::unordered_map<Id, size_t> m_Id_to_index_map{};
        std::unordered_map<size_t, Id> m_index_to_Id_map{};
        size_t                         m_size = 0;
};

}; // namespace buf