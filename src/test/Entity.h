
#pragma once

namespace EntitySystems
{
	/*
	EntityID -> [(COMPONENT_TYPE, ComponentID)] // HashMap<UniqueID,


	*/

	typedef uint32 ComponentTypeID;
	typedef uint64 UniqueID;
	typedef Atomic64 AtomicUniqueID;

	class Entity;
	class Context;
	class Component;

	class Entity
	{
	private:
		static AtomicUniqueID _guidCounter;

		const UniqueID _guid;
		Context * _context;
		Mutable::HashMap<ComponentTypeID, int32> _components;

		Entity(Context * c) : _context(c), _guid(_guidCounter++) {}
		~Entity()
		{
			// remove all components from their stores
			auto cItr = _components.GetIterator();
			while (cItr.HasNext())
			{
				auto c = cItr.Next();
				ComponentStores[ComponentTypeID]->Remove(c.value);
			}
		}

	  friend class Context;

	public:

		UniqueID GetID() const { return _guid; }

		template <class T> T& AddNewComponentOfType()
		{
			UniqueID id;
			auto c = T::CreateNewComponent(id);
			components[T::TypeID] = id;
			return c;
		}

		template <class T> T& GetComponentOfType()
		{ return components[T::TypeID]; }
	};


	class Context
	{
	private:
		Mutable::HashMap<UniqueID, Ref<Entity>> _entities;
	
	public:
	
		Ref<Entity> CreateNewEntity()
		{
			Ref<Entity> e = new Entity(this);
			_entities[e.GetID()] = e;
			return e;
		}
		void DestroyEntity(EntityID id)
		{
		   _entities -= id;
		}
	};


}










