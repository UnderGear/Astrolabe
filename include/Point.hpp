#pragma once

#include <cstdint>

#include "Vector.hpp"

struct Point2D
{
	std::int32_t X{ 0 };
	std::int32_t Y{ 0 };

	constexpr Point2D operator +(const Vector2D& Other) const
	{
		return { X + Other.X, Y + Other.Y };
	}

	constexpr Point2D& operator +=(const Vector2D& Other)
	{
		X += Other.X;
		Y += Other.Y;
		return *this;
	}

	constexpr Vector2D operator -(const Point2D& Other) const
	{
		return { X - Other.X, Y - Other.Y };
	}
};

namespace Point
{
	constexpr inline Point2D Origin { 0, 0 };
}