#pragma once

#include <cstdint>

#include "Fixed.hpp"
#include "Vector.hpp"

struct Point2D
{
	i24f8_t X{ 0 };
	i24f8_t Y{ 0 };

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
	constexpr inline Point2D Origin { i24f8_t{ 0 }, i24f8_t{ 0 } };
}