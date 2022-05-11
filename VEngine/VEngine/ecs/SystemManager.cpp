#include "SystemManager.h"

using namespace vengine;

void SystemManager::entity_destroyed(Entity entity)
{
    for (auto const& pair : m_systems)
    {
        auto const& system = pair.second;

        system->m_entities.erase(entity);
    }
}

void SystemManager::entity_signature_changed(Entity entity, Signature newSignature)
{
    for (auto const& pair : m_systems)
    {

        auto const& typeName = pair.first;
        auto const& system = pair.second;
        auto const& systemSignature = m_signatures[typeName];

        // Does bitwise binary wizardry to return a std::bitsize that only has common bits set
        if ((newSignature & systemSignature) == systemSignature)
        {
            system->m_entities.insert(entity);
        }
        else
        {
            system->m_entities.erase(entity);
        }
    }
}
