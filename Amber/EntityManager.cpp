#include "EntityManager.h"

#include <cassert>


EntityManager::EntityManager()
{
}


EntityManager::~EntityManager()
{
}

void EntityManager::init()
{
    entityCount = 0;
    entities = new Entity[MAX_ENTITIES];
}

void EntityManager::destroy()
{
	delete[] entities;
}

uint32_t EntityManager::capacity()
{
    return MAX_ENTITIES;
}

Entity* EntityManager::addEntity()
{
    EntityID id = newEntityID();
    entities[id].id = id;
    return &entities[id];
}

void EntityManager::destroyEntity(uint32_t id)
{
    entities[id].components.clear();
    entities[id].componentMask.reset();

    m_freeList.push_back(id);
    entityCount--;
}

void EntityManager::destroyEntity(Entity* entity)
{
    destroyEntity(entity->id);
}

Entity* EntityManager::getEntity(uint32_t id)
{
    assert(id < capacity());
    return &entities[id];
}

EntityID EntityManager::newEntityID()
{
    entityCount++;
    if (m_freeList.size() > 0)
    {
        EntityID eID = m_freeList.front();
        m_freeList.pop_front();
        return eID;
    }

    static EntityID id = 0;
    return id++;
}