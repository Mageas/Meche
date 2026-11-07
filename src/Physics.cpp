#include "Physics.h"
#include "Entity.h"

#include <cmath>

namespace Meche
{

namespace
{

Vec2 CalculateOverlap(const Entity& a, const Entity& b, bool usePreviousPositions)
{
	const TransformComponent& transformA = a.GetComponent<TransformComponent>();
	const TransformComponent& transformB = b.GetComponent<TransformComponent>();
	const Vec2& positionA = usePreviousPositions ? transformA.PreviousPosition : transformA.Position;
	const Vec2& positionB = usePreviousPositions ? transformB.PreviousPosition : transformB.Position;

	const Vec2 halfSizes =
		a.GetComponent<BoundingBoxComponent>().HalfSize + b.GetComponent<BoundingBoxComponent>().HalfSize;
	const Vec2 delta(std::abs(positionA.X - positionB.X), std::abs(positionA.Y - positionB.Y));

	return halfSizes - delta;
}

} // namespace

Vec2 Physics::GetOverlap(const Entity& a, const Entity& b) { return CalculateOverlap(a, b, false); }

Vec2 Physics::GetPreviousOverlap(const Entity& a, const Entity& b) { return CalculateOverlap(a, b, true); }

} // namespace Meche