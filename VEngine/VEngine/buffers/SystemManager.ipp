#pragma once

namespace buf
{

template <typename T>
auto SystemManager::register_system() -> std::shared_ptr<T>
{
    const char* typeName = typeid(T).name();

    assert(m_systems.find(typeName) == m_systems.end() && "Registering a system more then once.");

    auto system = std::make_shared<T>();
    m_systems.insert({typeName, system});

    return system;
}

template <typename T>
void SystemManager::set_signature(Signature signature)
{
    const char* typeName = typeid(T).name();

    assert(m_systems.find(typeName) != m_systems.end() && "System used before registering");

    m_signatures.insert({typeName, signature});
}

void SystemManager::id_destroyed(Id id)
{
    for (auto const& [typeName, system] : m_systems)
    {
        system->m_ids.erase(id);
    }
}

void SystemManager::id_signature_changed(Id id, Signature newSignature)
{
    for (auto const& [typeName, system] : m_systems)
    {
        auto const& systemSignature = m_signatures[typeName];

        // Does bitwise binary wizardry to return a std::bitsize that only has common bits set
        if ((newSignature & systemSignature) == systemSignature)
        {
            system->m_ids.insert(id);
        }
        else
        {
            system->m_ids.erase(id);
        }
    }
}

}; // namespace buf
