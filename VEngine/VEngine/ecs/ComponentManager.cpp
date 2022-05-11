#include "ComponentManager.h"

using namespace vengine;

void ComponentManager::entity_destroyed(Entity entity)
{
    for (auto const& pair : m_component_arrays)
    {

        auto const& component = pair.second;

        component->entity_destroyed(entity);
    }
}
