#pragma once

namespace buf
{

template <typename T>
void CompManager::register_components(const Id preallocation_size)
{
    const char* typeName = typeid(T).name();

    assert(m_component_types.find(typeName) == m_component_types.end() && "Error, component already registered");

    m_component_types.insert({typeName, m_next_component_type});
    m_component_datas.insert({typeName, std::make_shared<CompData<T>>()});
    m_next_component_type++;

    get_component_data()->preallocate(preallocation_size);
}

template <typename T>
ComponentType CompManager::get_component_type()
{
    const char* typeName = typeid(T).name();

    assert(m_component_types.find(typeName) != m_component_types.end() &&
           "Error, trying to get componentType from a component that isn't registered.");

    return m_component_types[typeName];
}

template <typename T>
void CompManager::add_component(Id id, T component)
{
    get_compoent_data<T>()->insert_data(id, component);
}

template <typename T>
void CompManager::remove_component(Id id)
{
    get_compoent_data<T>()->remove_data(id);
}

template <typename T>
auto CompManager::get_component(Id id) -> T&
{
    return get_compoent_data<T>()->get_data(id);
}

void CompManager::id_destroyed(Id id)
{
    for (auto const& [type_name, component] : m_component_datas)
    {
        component->destroy_id(id);
    }
}

template <typename T>
auto CompManager::get_compoent_data() -> std::shared_ptr<CompData<T>>
{
    const char* typeName = typeid(T).name();

    assert(m_component_types.find(typeName) != m_component_types.end() && "Component not registered before use.");

    return std::static_pointer_cast<CompData<T>>(m_component_datas[typeName]); // Nice little dogy cast ting
}

}; // namespace buf