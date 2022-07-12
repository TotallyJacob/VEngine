#pragma once

namespace buf
{


STATIC_COMP_DATA_TEMPLATE
void StaticCompData STATIC_COMP_DATA_IDENTIFIER::insert_data(Id id, T component)
{
    assert(m_Id_to_index_map.find(id) == m_Id_to_index_map.end() && "Component added to same Id more than once.");

    size_t newIndex = m_size;
    m_Id_to_index_map[id] = newIndex;
    m_index_to_Id_map[newIndex] = id;
    m_component_data[newIndex] = component;
    m_size++;
}

STATIC_COMP_DATA_TEMPLATE
void StaticCompData STATIC_COMP_DATA_IDENTIFIER::remove_data(Id id)
{
    // Switch the component array to the last element
    size_t indexOfRemovedId = m_Id_to_index_map[id];
    size_t indexOfLastId = m_size - 1;
    m_component_data[indexOfRemovedId] = m_component_data[indexOfLastId];

    // Change the entity and index to become the last element
    Id entityOfLastId = m_index_to_Id_map[indexOfLastId];
    m_Id_to_index_map[entityOfLastId] = indexOfRemovedId;
    m_index_to_Id_map[indexOfRemovedId] = entityOfLastId;

    // Removed the previous last element and element to remove stuff
    m_Id_to_index_map.erase(id);
    m_index_to_Id_map.erase(indexOfLastId);


    --m_size;
}

STATIC_COMP_DATA_TEMPLATE
auto StaticCompData STATIC_COMP_DATA_IDENTIFIER::get_data(Id id) -> T&
{
    assert(m_Id_to_index_map.find(id) != m_Id_to_index_map.end() && "Error, Id component doesn't exist");

    return m_component_data[m_Id_to_index_map[id]];
}

STATIC_COMP_DATA_TEMPLATE
void StaticCompData STATIC_COMP_DATA_IDENTIFIER::destroy_id(Id id)
{
    if (m_Id_to_index_map.find(id) != m_Id_to_index_map.end())
    {
        remove_data(id);
    }
}


}; // namespace buf