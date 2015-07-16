#include "Entity.h"

Entity::Entity()
{
}


Entity::~Entity()
{
}

void Entity::setTag(Tag t)
{
    tag = t;
}

bool Entity::operator==(Entity const& rhs)
{
    return id == rhs.id;
}

bool Entity::operator!=(Entity const& rhs)
{
    return id != rhs.id;
}
