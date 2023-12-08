#pragma once

#include <cstdint>

struct Point2D;

struct Vector2D
{
	std::int32_t X{ 0 };
	std::int32_t Y{ 0 };

	constexpr Vector2D operator *(std::int32_t Scalar) const
	{
		return { X * Scalar, Y * Scalar };
	}

	constexpr Vector2D& operator *(std::int32_t Scalar)
	{
		X *= Scalar;
		Y *= Scalar;
		return *this;
	}

	constexpr Vector2D operator *=(std::int32_t Scalar)
	{
		return { X * Scalar, Y * Scalar };
	}

	//TODO: add vector
	//TODO: unary negation

	constexpr Vector2D operator -() const
{
    return { -X, -Y };
}

	//TODO: subtract vector
	//TODO: magnitude, squaremagnitude
	//TODO: normalize, get normalized
};

namespace Vector
{
	inline constexpr Vector2D Zero{ 0, 0 };
}
