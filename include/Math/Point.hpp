#pragma once

#include <cstdint>

#include "Fixed.hpp"
#include "Vector.hpp"

struct Point2D
{
	i24f8_t X{ 0 };
	i24f8_t Y{ 0 };

	constexpr Point2D() = default;
	constexpr Point2D(i24f8_t InX, i24f8_t InY) : X(InX), Y(InY) {}

	template <typename IntegralT> requires std::is_integral_v<IntegralT>
	constexpr Point2D(IntegralT InX, IntegralT InY) : X(InX), Y(InY) {}

	constexpr Point2D operator +(const Vector2D& Other) const
	{
		return { X + Other.X, Y + Other.Y };
	}

	Point2D& operator +=(const Vector2D& Other)
	{
		X += Other.X;
		Y += Other.Y;
		return *this;
	}

	constexpr Vector2D operator -(const Point2D& Other) const
	{
		return { X - Other.X, Y - Other.Y };
	}

	constexpr auto operator<=>(const Point2D& Other) const = default;
};

namespace Point
{
	constexpr inline Point2D Origin;
}
