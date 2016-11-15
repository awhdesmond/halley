#pragma once

#include <algorithm>
#include "family_type.h"
#include "family_mask.h"
#include "entity_id.h"

namespace Halley {
	class Entity;
	class FamilyBindingBase;

	class Family {
		friend class World;

	public:
		Family(FamilyMaskType mask);
		virtual ~Family() {}

		size_t count() const
		{
			return elemCount;
		}

		void* getElement(size_t n) const
		{
			return static_cast<char*>(elems) + (n * elemSize);
		}

		void addOnEntityAdded(FamilyBindingBase* bind);
		void removeOnEntityAdded(FamilyBindingBase* bind);
		void addOnEntityRemoved(FamilyBindingBase* bind);
		void removeOnEntityRemoved(FamilyBindingBase* bind);

		void notifyAdd(void* entity);
		void notifyRemove(void* entity);

	protected:
		virtual void addEntity(Entity& entity) = 0;
		void removeEntity(Entity& entity);
		virtual void removeDeadEntities() = 0;
		virtual void clearEntities() = 0;
		
		void* elems = nullptr;
		size_t elemCount = 0;
		size_t elemSize = 0;
		Vector<EntityId> toRemove;

		Vector<FamilyBindingBase*> addEntityCallbacks;
		Vector<FamilyBindingBase*> removeEntityCallbacks;

	private:
		FamilyMaskType inclusionMask;
	};

	// Apple's Clang 3.5 does not seem to have constexpr std::max...
	constexpr size_t maxSize(size_t a, size_t b)
	{
		return a > b ? a : b;
	}

	template <typename T>
	class FamilyImpl : public Family
	{
		struct StorageType
		{
			EntityId entityId;
			alignas(alignof(void*)) std::array<char, sizeof(T) - maxSize(sizeof(EntityId), sizeof(void*))> data;
		};
		static_assert(((T::Type::getNumComponents() + 1) * sizeof(void*)) == sizeof(T), "Family type has unexpected storage size");

	public:
		FamilyImpl() : Family(T::Type::inclusionMask()) {}
				
	protected:
		void addEntity(Entity& entity) override
		{
			entities.push_back(StorageType());
			auto& e = entities.back();
			e.entityId = entity.getEntityId();
			T::Type::loadComponents(entity, &e.data[0]);

			notifyAdd(&e);

			updateElems();
		}

		void removeDeadEntities() override
		{
			// Performance-critical code
			// Benchmarks suggest that using a Vector is faster than std::set and std::unordered_set
			if (!toRemove.empty()) {
				std::sort(toRemove.begin(), toRemove.end());

				size_t size = entities.size();
				for (size_t i = 0; i < size; i++) {
					EntityId id = entities[i].entityId;
					auto iter = std::lower_bound(toRemove.begin(), toRemove.end(), id);
					if (iter != toRemove.end() && id == *iter) {
						notifyRemove(&entities[i]);

						toRemove.erase(iter);
						std::swap(entities[i], entities[size - 1]);
						entities.pop_back();
						size--;
						i--;
						if (toRemove.empty()) break;
					}
				}
				toRemove.clear();
				updateElems();
			}
		}

		void clearEntities() override
		{
			for (auto& e: entities) {
				notifyRemove(&e);
			}
			entities.clear();
			updateElems();
		}

	private:
		Vector<StorageType> entities;

		void updateElems()
		{
			elems = entities.empty() ? nullptr : entities.data();
			elemCount = entities.size();
			elemSize = sizeof(StorageType);
		}
	};
}
