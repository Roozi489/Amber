#include "EntityManager.h"

#include <cassert>


EntityManager::EntityManager()
{
}


EntityManager::~EntityManager()
{
    delete[] entities;
}

void EntityManager::init()
{
    entityCount = 0;
    entities = new Entity[MAX_ENTITIES];
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

    freeList.push_back(id);
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
    if (freeList.size() > 0)
    {
        EntityID eID = freeList.front();
        freeList.pop_front();
        return eID;
    }

    static EntityID id = 0;
    return id++;
}