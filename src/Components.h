#pragma once
#include "Vec2.h"

#include <string>

namespace Meche
{

struct Component
{
	bool Exists = false;
};

struct TransformComponent : Component
{
	Vec2 Position;
	Vec2 PreviousPosition;
	Vec2 Velocity;
	Vec2 Scale = Vec2(1.0f, 1.0f);

	TransformComponent() = default;
	TransformComponent(const Vec2& position) : Position(position), PreviousPosition(position) {}
	TransformComponent(const Vec2& position, const Vec2& scale)
		: Position(position), PreviousPosition(position), Scale(scale)
	{
	}
};

struct LifespanComponent : Component
{
	uint32_t DurationInFrames = 0;
	uint32_t RemainingFrames = 0;

	LifespanComponent() = default;
	LifespanComponent(uint32_t durationInFrames) : DurationInFrames(durationInFrames), RemainingFrames(durationInFrames)
	{
	}
};

struct InputComponent : Component
{
	bool Up = false;
	bool Down = false;
	bool Right = false;
	bool Left = false;
	bool Shoot = false;
	bool CanJump = true;
	bool CanShoot = true;

	InputComponent() = default;
};

struct BoundingBoxComponent : Component
{
	Vec2 Size;
	Vec2 HalfSize;

	BoundingBoxComponent() = default;
	BoundingBoxComponent(const Vec2& size) : Size(size), HalfSize(size / 2) {}
};

struct GravityComponent : Component
{
	float Gravity = 0.0f;

	GravityComponent() = default;
	GravityComponent(float gravity) : Gravity(gravity) {}
};

struct StateComponent : Component
{
	std::string State;

	StateComponent() = default;
	StateComponent(const std::string& state) : State(state) {}
};

} // namespace Meche