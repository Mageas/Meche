#include "EntityManager.h"

#include <ranges>

namespace Meche
{

namespace
{

bool IsEntityDead(const EntityPtr& entity) { return !entity->IsAlive(); }

} // namespace

EntityPtr EntityManager::AddEntity(Entity::Tag tag)
{
	EntityPtr entity(new Entity(m_NextID++, tag));
	m_EntitiesToAdd.push_back(entity);
	return entity;
}

void EntityManager::Update()
{
	AddPendingEntities();
	RemoveDeadEntities();
}

void EntityManager::AddPendingEntities()
{
	for (const EntityPtr& entity : m_EntitiesToAdd)
	{
		m_Entities.push_back(entity);
		m_EntitiesByTag[entity->GetTag()].push_back(entity);
	}
	m_EntitiesToAdd.clear();
}

void EntityManager::RemoveDeadEntities()
{
	std::erase_if(m_Entities, IsEntityDead);
	for (Entities& entities : m_EntitiesByTag | std::views::values)
		std::erase_if(entities, IsEntityDead);
}

} // namespace Meche