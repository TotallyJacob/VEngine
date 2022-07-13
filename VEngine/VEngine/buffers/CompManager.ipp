#pragma once

namespace buf
{

COMP_MANAGER_TEMPLATE
template <typename DATA_TYPE>
void CompManager COMP_MANAGER_IDENTIFIER::register_component(const Id preallocation_size)
{
    using type = COMP_TYPE<DATA_TYPE, sizes...>;

    if constexpr (!assert_base_class<type>()) // so we can see if this is a constant expression
    {
        return;
    }

    const char* typeName = typeid(DATA_TYPE).name();

    assert(m_component_types.find(typeName) == m_component_types.end() && "Error, component already registered");

    m_component_types.insert({typeName, m_next_component_type});
    m_component_datas.insert({typeName, static_cast<std::shared_ptr<ICompData>>(std::make_shared<type>())});
    m_next_component_type++;

    if constexpr (std::is_base_of<CompData<DATA_TYPE>, type>())
    {
        get_component_data<DATA_TYPE>()->preallocate(preallocation_size);
    }
}

COMP_MANAGER_TEMPLATE
template <typename DATA_TYPE>
ComponentType CompManager COMP_MANAGER_IDENTIFIER::get_component_type()
{
    const char* typeName = typeid(DATA_TYPE).name();

    assert(m_component_types.find(typeName) != m_component_types.end() &&
           "Error, trying to get componentType from a component that isn't registered.");

    return m_component_types[typeName];
}

COMP_MANAGER_TEMPLATE
template <typename DATA_TYPE>
void CompManager COMP_MANAGER_IDENTIFIER::add_component(Id id, DATA_TYPE component)
{
    get_component_data<DATA_TYPE>()->insert_data(id, component);
}

COMP_MANAGER_TEMPLATE
template <typename DATA_TYPE>
void CompManager COMP_MANAGER_IDENTIFIER::remove_component(Id id)
{
    get_component_data<DATA_TYPE>()->remove_data(id);
}

COMP_MANAGER_TEMPLATE
template <typename DATA_TYPE>
auto CompManager COMP_MANAGER_IDENTIFIER::get_component(Id id) -> DATA_TYPE&
{
    return get_component_data<DATA_TYPE>()->get_data(id);
}

COMP_MANAGER_TEMPLATE
void CompManager COMP_MANAGER_IDENTIFIER::id_destroyed(Id id)
{
    for (auto const& [type_name, component] : m_component_datas)
    {
        component->destroy_id(id);
    }
}

COMP_MANAGER_TEMPLATE
template <typename DATA_TYPE>
auto CompManager COMP_MANAGER_IDENTIFIER::get_component_data() -> std::shared_ptr<COMP_TYPE<DATA_TYPE, sizes...>>
{
    const char* typeName = typeid(DATA_TYPE).name();

    assert(m_component_types.find(typeName) != m_component_types.end() && "Component not registered before use.");

    return std::static_pointer_cast<COMP_TYPE<DATA_TYPE, sizes...>>(m_component_datas[typeName]); // Nice little dogy cast ting
}


}; // namespace buf