#pragma once
#include "Vec2.h"

namespace Meche
{
class Entity;
namespace Physics
{

Vec2 GetOverlap(const Entity& a, const Entity& b);
Vec2 GetPreviousOverlap(const Entity& a, const Entity& b);

} // namespace Physics
} // namespace Meche