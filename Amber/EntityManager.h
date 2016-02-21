#pragma once
#include "Entity.h"

#include <list>

static const uint32_t MAX_ENTITIES = 1024;

class EntityManager;

template <typename Delegate, bool All = false>
class ViewIterator : public std::iterator<std::input_iterator_tag, EntityID>
{
public:
    Delegate& operator++()
    {
        ++i;
        next();
        return *static_cast<Delegate*>(this);
    }
    bool operator==(const Delegate& rhs) const { return i == rhs.i; }
    bool operator!=(const Delegate& rhs) const { return i != rhs.i; }
    Entity& operator*() { return *entityManager->getEntity(i); }
    const Entity& operator*() const { return *entityManager->getEntity(i); }

protected:
	EntityManager* entityManager;
	ComponentMask compMask;
	EntityID i;
	size_t capacity;
	std::list<EntityID>::iterator freeCursor;

    ViewIterator(EntityManager* manager, EntityID index)
        : entityManager(manager)
        , i(index)
        , capacity(entityManager->capacity())
    {
        if (All)
        {
			entityManager->freeList.sort();
			freeCursor = entityManager->freeList.begin();
        }
    }
    ViewIterator(EntityManager* manager, const ComponentMask mask, EntityID index)
    : entityManager(manager)
    , compMask(mask)
    , i(index)
    , capacity(entityManager->capacity())
    {
		if (All)
		{
			entityManager->freeList.sort();
			freeCursor = entityManager->freeList.begin();
		}
    }

    void next()
    {
        while (i < capacity && !predicate())
        {
            ++i;
        }

        if (i < capacity)
        {
            Entity* entity = entityManager->getEntity(i);
            static_cast<Delegate*>(this)->next_entity(*entity);
        }
    }

    inline bool predicate()
    {
        return (All && validEntity()) || (entityManager->getEntity(i)->componentMask & compMask) == compMask;
    }

    inline bool validEntity()
    {
        if (freeCursor != entityManager->freeList.end() && *freeCursor == i)
        {
            ++free_cursor_;
            return false;
        }
        return true;
    }    
};

template <bool All>
class BaseView
{
public:
    class Iterator : public ViewIterator<Iterator, All>
    {
    public:
        Iterator(EntityManager* manager, const ComponentMask mask, uint32_t index) 
            : ViewIterator<Iterator, All>(manager, mask, index)
        {
            ViewIterator<Iterator, All>::next();
        }

        void next_entity(Entity& entity) {}
    };


    Iterator begin() { return Iterator(entityManager, compMask, 0); }
    Iterator end() { return Iterator(entityManager, compMask, entityManager->capacity()); }
    const Iterator begin() const { return Iterator(entityManager, compMask, 0); }
    const Iterator end() const { return Iterator(entityManager, compMask, entityManager->capacity()); }

private:
	EntityManager* entityManager;
	ComponentMask compMask;
    friend class EntityManager;

    BaseView(EntityManager* manager)
		: entityManager(manager)
    {
	    compMask.set();
    }
    BaseView(EntityManager* manager, ComponentMask mask)
        : entityManager(manager)
        , compMask(mask)
    {
    }
};

typedef BaseView<false> View;
typedef BaseView<true> DebugView;

template <typename ... Components>
class UnpackingView
{
public:
    struct Unpacker
    {
        Unpacker(Components& ... components)
            : components(std::tuple<Components & ...>(components...))
        {
        }

        void unpack(Entity &entity) const
        {
            unpack_<0, Components...>(entity);
        }

    private:
        template <int N, typename C>
        void unpack_(Entity& entity) const
        {
            std::get<N>(components) = entity.getComponent<C>();
        }

        template <int N, typename C0, typename C1, typename ... Cn>
        void unpack_(Entity& entity) const
        {
            std::get<N>(components) = entity.getComponent<C0>();
            unpack_<N + 1, C1, Cn...>(entity);
        }

        std::tuple<Components& ...> components;
    };


    class Iterator : public ViewIterator<Iterator>
    {
    public:
        Iterator(EntityManager* manager, const ComponentMask mask, uint32_t index, const Unpacker& unpacker)
            : ViewIterator<Iterator>(manager, mask, index)
            , unpacker(unpacker)
        {
            ViewIterator<Iterator>::next();
        }

        void next_entity(Entity &entity)
        {
            unpacker.unpack(entity);
        }

    private:
        const Unpacker& unpacker;
    };


    Iterator begin() { return Iterator(entityManager, compMask, 0, unpacker); }
    Iterator end() { return Iterator(entityManager, compMask, entityManager->capacity(), unpacker); }
    const Iterator begin() const { return Iterator(entityManager, compMask, 0, unpacker); }
    const Iterator end() const { return Iterator(entityManager, compMask, entityManager->capacity(), unpacker); }


private:
	EntityManager* entityManager;
	ComponentMask compMask;
	Unpacker unpacker;

    UnpackingView(EntityManager* manager, ComponentMask mask, Components& ... components)
        : entityManager(manager)
        , compMask(mask)
        , unpacker(components...)
    {
    }

	friend class EntityManager;
};

class EntityManager
{
public:
    uint32_t entityCount;
    Entity* entities;

    EntityManager();
    ~EntityManager();

    void init();
	void destroy();

    uint32_t capacity();

    Entity* addEntity();
    void destroyEntity(uint32_t id);
    void destroyEntity(Entity* entity);

    Entity* getEntity(uint32_t id);

    ComponentMask component_mask(EntityID id)
    {
        return entities[id].componentMask;
    }

    template <typename C>
    ComponentMask component_mask()
    {
        ComponentMask mask;
        mask.set(C::template getComponentTypeID<C>());
        return mask;
    }

    template <typename C1, typename C2, typename ... Components>
    ComponentMask component_mask()
    {
        return component_mask<C1>() | component_mask<C2, Components ...>();
    }

    template <typename C>
    ComponentMask component_mask(const BaseComponent* c)
    {
        return component_mask<C>();
    }

    template <typename C1, typename ... Components>
    ComponentMask component_mask(const C1 &c1, const Components &... args)
    {
        return component_mask<C1, Components ...>();
    }

    template <typename ... Components>
    View entities_with_components()
    {
        auto mask = component_mask<Components ...>();
        return View(this, mask);
    }

    template <typename ... Components>
    UnpackingView<Components...> entities_with_components(Components & ... components)
    {
        auto mask = component_mask<Components...>();
        return UnpackingView<Components...>(this, mask, components...);
    }

private:
    std::list<EntityID> freeList;

    EntityID newEntityID();

	template <typename Delegate, bool All>
	friend class ViewIterator;
};