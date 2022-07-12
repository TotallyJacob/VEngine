#include "BufferSystemManager.h"

using namespace buf;

void BufferSystemManager::id_destroyed(Id id)
{
    for (auto const& [typeName, system] : m_systems)
    {
        system->m_ids.erase(id);
    }
}

void BufferSystemManager::id_signature_changed(Id id, Signature newSignature)
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
