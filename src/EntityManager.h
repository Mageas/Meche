#pragma once
#include "Entity.h"

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Meche
{

using EntityPtr = std::shared_ptr<Entity>;
using Entities = std::vector<EntityPtr>;
using EntitiesByTag = std::unordered_map<Entity::Tag, Entities>;

class EntityManager
{
public:
	EntityPtr AddEntity(Entity::Tag tag);

	void Update();

	const Entities& GetEntities() const { return m_Entities; }
	const Entities& GetEntitiesByTag(Entity::Tag tag) { return m_EntitiesByTag[tag]; }

private:
	void AddPendingEntities();
	void RemoveDeadEntities();

private:
	Entities m_Entities;
	Entities m_EntitiesToAdd;
	EntitiesByTag m_EntitiesByTag;
	uint32_t m_NextID = 0;
};

} // namespace Meche