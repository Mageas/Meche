#pragma once

#include <SFML/System/Vector2.hpp>

#include <cmath>

namespace Meche
{

class Vec2
{
public:
	float X = 0.0f;
	float Y = 0.0f;

	Vec2() = default;
	Vec2(float x, float y) : X(x), Y(y) {}

	operator sf::Vector2f() const { return sf::Vector2f(X, Y); }

	Vec2& operator+=(const Vec2& other)
	{
		X += other.X;
		Y += other.Y;
		return *this;
	}

	Vec2& operator-=(const Vec2& other)
	{
		X -= other.X;
		Y -= other.Y;
		return *this;
	}

	Vec2 operator+(const Vec2& other) const
	{
		return {
			X + other.X,
			Y + other.Y,
		};
	}

	Vec2 operator-(const Vec2& other) const
	{
		return {
			X - other.X,
			Y - other.Y,
		};
	}

	Vec2 operator*(float multiplier) const
	{
		return {
			X * multiplier,
			Y * multiplier,
		};
	}

	Vec2 operator/(float denominator) const
	{
		return {
			X / denominator,
			Y / denominator,
		};
	}

	float LengthSquared() const { return X * X + Y * Y; }
	float Length() const { return std::sqrt(LengthSquared()); }

	float DistanceSquared(const Vec2& other) const { return (other - *this).LengthSquared(); }
	float Distance(const Vec2& other) const { return (other - *this).Length(); }

	Vec2& Normalize()
	{
		if (const float length = Length(); length != 0.0f)
		{
			X /= length;
			Y /= length;
		}

		return *this;
	}
};

} // namespace Meche