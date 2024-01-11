#pragma once

#include <cstdint>

#include "Fixed.hpp"

struct Point2D;

struct Vector2D
{
	i24f8_t X{ 0.f };
	i24f8_t Y{ 0.f };

	constexpr Vector2D operator *(i24f8_t Scalar) const
	{
		return { X * Scalar, Y * Scalar };
	}

	constexpr Vector2D& operator *(i24f8_t Scalar)
	{
		X *= Scalar;
		Y *= Scalar;
		return *this;
	}

	constexpr Vector2D operator *=(i24f8_t Scalar)
	{
		return { X * Scalar, Y * Scalar };
	}

	//TODO: add vector

	constexpr Vector2D operator -() const
	{
		return { -X, -Y };
	}

	//TODO: subtract vector
	//TODO: magnitude, squaremagnitude
	//TODO: normalize, get normalized
	
	auto operator<=>(const Vector2D& Other) const = default;
};

namespace Vector
{
	inline constexpr Vector2D Zero{ i24f8_t{ 0 }, i24f8_t{ 0 } };
	inline constexpr Vector2D One{ i24f8_t{ 1 }, i24f8_t{ 1 } };
	inline constexpr Vector2D Up{ i24f8_t{ 0 }, i24f8_t{ 1 } };
	inline constexpr Vector2D Down{ i24f8_t{ 0 }, i24f8_t{ -1 } };
	inline constexpr Vector2D Left{ i24f8_t{ -1 }, i24f8_t{ 0 } };
	inline constexpr Vector2D Right{ i24f8_t{ 1 }, i24f8_t{ 0 } };
}
