#pragma once
#include "Core/Entity.h"

#include <list>

namespace Amber
{

static const uint32_t MAX_ENTITIES = 1024;

class EntityManager;

template <typename Delegate, bool All = false>
class ViewIterator : public std::iterator<std::input_iterator_tag, EntityID>
{
public:
	Delegate& operator++()
	{
		++m_i;
		next();
		return *static_cast<Delegate*>(this);
	}
	bool operator==(const Delegate& rhs) const { return m_i == rhs.m_i; }
	bool operator!=(const Delegate& rhs) const { return m_i != rhs.m_i; }
	Entity& operator*() { return *m_entityManager->getEntity(m_i); }
	const Entity& operator*() const { return *m_entityManager->getEntity(m_i); }

protected:
	EntityManager* m_entityManager;
	ComponentMask m_compMask;
	EntityID m_i;
	size_t m_capacity;
	std::list<EntityID>::iterator m_freeCursor;

	ViewIterator(EntityManager* manager, EntityID index)
		: m_entityManager(manager)
		, m_i(index)
		, m_capacity(m_entityManager->m_capacity())
	{
		if (All)
		{
			m_entityManager->m_freeList.sort();
			m_freeCursor = m_entityManager->m_freeList.begin();
		}
	}
	ViewIterator(EntityManager* manager, const ComponentMask mask, EntityID index)
		: m_entityManager(manager)
		, m_compMask(mask)
		, m_i(index)
		, m_capacity(m_entityManager->capacity())
	{
		if (All)
		{
			m_entityManager->m_freeList.sort();
			m_freeCursor = m_entityManager->m_freeList.begin();
		}
	}

	void next()
	{
		while (m_i < m_capacity && !predicate())
		{
			++m_i;
		}

		if (m_i < m_capacity)
		{
			Entity* entity = m_entityManager->getEntity(m_i);
			static_cast<Delegate*>(this)->next_entity(*entity);
		}
	}

	inline bool predicate()
	{
		return (All && validEntity()) || (m_entityManager->getEntity(m_i)->componentMask & m_compMask) == m_compMask;
	}

	inline bool validEntity()
	{
		if (m_freeCursor != m_entityManager->m_freeList.end() && *m_freeCursor == m_i)
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


	Iterator begin() { return Iterator(m_entityManager, m_compMask, 0); }
	Iterator end() { return Iterator(m_entityManager, m_compMask, m_entityManager->capacity()); }
	const Iterator begin() const { return Iterator(m_entityManager, m_compMask, 0); }
	const Iterator end() const { return Iterator(m_entityManager, m_compMask, m_entityManager->capacity()); }

private:
	EntityManager* m_entityManager;
	ComponentMask m_compMask;
	friend class EntityManager;

	BaseView(EntityManager* manager)
		: m_entityManager(manager)
	{
		m_compMask.set();
	}
	BaseView(EntityManager* manager, ComponentMask mask)
		: m_entityManager(manager)
		, m_compMask(mask)
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
			, m_unpacker(unpacker)
		{
			ViewIterator<Iterator>::next();
		}

		void next_entity(Entity &entity)
		{
			m_unpacker.unpack(entity);
		}

	private:
		const Unpacker& m_unpacker;
	};


	Iterator begin() { return Iterator(m_entityManager, m_compMask, 0, m_unpacker); }
	Iterator end() { return Iterator(m_entityManager, m_compMask, m_entityManager->capacity(), m_unpacker); }
	const Iterator begin() const { return Iterator(m_entityManager, m_compMask, 0, m_unpacker); }
	const Iterator end() const { return Iterator(m_entityManager, m_compMask, m_entityManager->capacity(), m_unpacker); }


private:
	EntityManager* m_entityManager;
	ComponentMask m_compMask;
	Unpacker m_unpacker;

	UnpackingView(EntityManager* manager, ComponentMask mask, Components& ... components)
		: m_entityManager(manager)
		, m_compMask(mask)
		, m_unpacker(components...)
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
	std::list<EntityID> m_freeList;

	EntityID newEntityID();

	template <typename Delegate, bool All>
	friend class ViewIterator;
};

}