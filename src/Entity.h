#pragma once

#include "Components.h"

#include <cstdint>
#include <tuple>
#include <utility>

namespace Meche
{

using ComponentTuple = std::tuple<TransformComponent, LifespanComponent, InputComponent, BoundingBoxComponent,
								  GravityComponent, StateComponent>;

class Entity
{
public:
	enum class Tag
	{
		Player,
		Tile,
		Bomb,
		Explosion,
		Decoration,
		Diamond,
	};

	void Destroy() { m_IsAlive = false; }

	uint32_t GetID() const { return m_ID; }
	Tag GetTag() const { return m_Tag; }
	bool IsAlive() const { return m_IsAlive; }

	template <typename T, typename... Args> T& AddComponent(Args&&... args)
	{
		T& component = GetComponent<T>() = T(std::forward<Args>(args)...);
		component.Exists = true;
		return component;
	}

	template <typename T> T& GetComponent() { return std::get<T>(m_Components); }
	template <typename T> const T& GetComponent() const { return std::get<T>(m_Components); }

	template <typename T> bool HasComponent() const { return GetComponent<T>().Exists; }

	template <typename T> void RemoveComponent() { GetComponent<T>() = T(); }

private:
	Entity(uint32_t id, Tag tag) : m_ID(id), m_Tag(tag) {}

private:
	bool m_IsAlive = true;
	uint32_t m_ID;
	ComponentTuple m_Components;
	Tag m_Tag;

	friend class EntityManager;
};

} // namespace Meche