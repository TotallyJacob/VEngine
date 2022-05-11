#include "EntityManager.h"

using namespace vengine;

EntityManager::EntityManager()
{
    for (Entity e = 0; e < MAX_ENTITIES; ++e)
    {
        m_avalible_entities.push(e);
    }
}


Entity EntityManager::create_entity()
{

    assert(m_living_entity_count < MAX_ENTITIES && "Too many Entities");

    Entity id = m_avalible_entities.front();
    m_avalible_entities.pop();
    ++m_living_entity_count;

    return id;
}


void EntityManager::destroy_entity(Entity entity)
{
    assert(entity < MAX_ENTITIES && "Entity too large");

    m_signatures[entity].reset();

    m_avalible_entities.push(entity);
    --m_living_entity_count;
}

void EntityManager::set_signature(Entity entity, Signature signature)
{
    assert(entity < MAX_ENTITIES && "Entity too large");

    m_signatures[entity] = signature;
}

Signature EntityManager::get_signature(Entity entity)
{
    assert(entity < MAX_ENTITIES && "Entity too large");

    return m_signatures[entity];
}
