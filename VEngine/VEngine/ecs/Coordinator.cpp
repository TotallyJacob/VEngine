#include "Coordinator.h"

using namespace vengine;

void Coordinator::init()
{

    m_component_manager = std::make_unique<ComponentManager>();
    m_entity_manager = std::make_unique<EntityManager>();
    m_system_manager = std::make_unique<SystemManager>();
}

Entity Coordinator::create_entity()
{
    return m_entity_manager->create_entity();
}

void Coordinator::destroy_entity(Entity entity)
{
    m_entity_manager->destroy_entity(entity);
    m_component_manager->entity_destroyed(entity);
    m_system_manager->entity_destroyed(entity);
}
