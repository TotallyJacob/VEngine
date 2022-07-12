#pragma once

namespace buf
{

template <typename T>
auto BufferSystemManager::register_system() -> std::shared_ptr<T>
{
    const char* typeName = typeid(T).name();

    assert(m_systems.find(typeName) == m_systems.end() && "Registering a system more then once.");

    auto system = std::make_shared<T>();
    m_systems.insert({typeName, system});

    return system;
}

template <typename T>
void BufferSystemManager::set_signature(Signature signature)
{
    const char* typeName = typeid(T).name();

    assert(m_systems.find(typeName) != m_systems.end() && "System used before registering");

    m_signatures.insert({typeName, signature});
}

}; // namespace buf
