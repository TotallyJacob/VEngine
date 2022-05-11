#pragma once

#include <iostream>
#include <unordered_map>

#include "IComponentArray.h"

namespace vengine
{

template <typename T>
class ComponentArray : public IComponentArray
{
    public:

        void insert_data(Entity entity, T component)
        {
            assert(m_entity_to_index_map.find(entity) == m_entity_to_index_map.end() && "Component added to same entity more than once.");

            size_t newIndex = m_size;
            m_entity_to_index_map[entity] = newIndex;
            m_index_to_entity_map[newIndex] = entity;
            m_component_array[newIndex] = component;
            m_size++;
        }

        void remove_data(Entity entity)
        {
            // Switch the component array to the last element
            size_t indexOfRemovedEntity = m_entity_to_index_map[entity];
            size_t indexOfLastElement = m_size - 1;
            m_component_array[indexOfRemovedEntity] = m_component_array[indexOfLastElement];

            // Change the entity and index to become the last element
            Entity entityOfLastElement = m_index_to_entity_map[indexOfLastElement];
            m_entity_to_index_map[entityOfLastElement] = indexOfRemovedEntity;
            m_index_to_entity_map[indexOfRemovedEntity] = entityOfLastElement;

            // Removed the previous last element and element to remove stuff
            m_entity_to_index_map.erase(entity);
            m_index_to_entity_map.erase(indexOfLastElement);


            --m_size;
        }

        T& get_data(Entity entity)
        {
            assert(m_entity_to_index_map.find(entity) != m_entity_to_index_map.end() && "Error, entity component doesn't exist");

            return m_component_array[m_entity_to_index_map[entity]];
        }

        void entity_destroyed(Entity entity) override
        {
            if (m_entity_to_index_map.find(entity) != m_entity_to_index_map.end())
            {
                remove_data(entity);
            }
        }

    private:

        std::array<T, MAX_ENTITIES>        m_component_array;
        std::unordered_map<Entity, size_t> m_entity_to_index_map;
        std::unordered_map<size_t, Entity> m_index_to_entity_map;
        size_t                             m_size;
};
}; // namespace vengine
