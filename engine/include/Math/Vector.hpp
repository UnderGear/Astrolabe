#pragma once

#include <cstdint>

#include "Fixed.hpp"

struct Point2D;

struct Vector2D
{

	//TODO: template Vector2D's backing type?
	i24f8_t X{ 0.f };
	i24f8_t Y{ 0.f };

	constexpr Vector2D() = default;
	constexpr Vector2D(i24f8_t InX, i24f8_t InY) : X(InX), Y(InY) {}

	template <typename IntegralT> requires std::is_integral_v<IntegralT>
	constexpr Vector2D(IntegralT InX, IntegralT InY) : X(InX), Y(InY) {}

	constexpr Vector2D operator *(i24f8_t Scalar) const
	{
		return { X * Scalar, Y * Scalar };
	}

	Vector2D& operator *=(i24f8_t Scalar)
	{
		X *= Scalar;
		Y *= Scalar;
		return *this;
	}

	//TODO: multiply by other integral scalar. maybe just add requires clauses to the above?

	//TODO: add vector...?

	constexpr Vector2D operator -() const
	{
		return { -X, -Y };
	}

	//TODO: subtract vector
	//TODO: magnitude, squaremagnitude
	
	constexpr auto operator<=>(const Vector2D& Other) const = default;

	constexpr i24f8_t SquaredMagnitude() const
	{
		return X * X + Y * Y;
	}

	constexpr i24f8_t Magnitude() const
	{
		return Sqrt(SquaredMagnitude());
	}

	Vector2D& Normalize()
	{
		auto Mag{ Magnitude() };
		if (Mag == 0)
		{
			return *this;
		}

		X /= Mag;
		Y /= Mag;
		return *this;
	}

	constexpr Vector2D GetNormalized() const
	{
		auto Result = *this;
		//TODO:
		auto Mag{ Result.Magnitude() };
		if (Mag == 0)
		{
			return Result;
		}

		Result.X /= Mag;
		Result.Y /= Mag;
		return Result;
	}

	constexpr i24f8_t Dot(const Vector2D& Other) const
	{
		return X * Other.X + Y * Other.Y;
	}

	//TODO: we need fixed point transcendental functions
	// Angle in Radians
	/*
	constexpr i24f8_t AngleBetween(const Vector2D& Other) const
	{
		return ArcCos(Dot(Other) / (Magnitude * Other.Magnitude));
	}
	*/

	//TODO: alternate for vectors we know are normalized?
};

namespace Vector
{
	inline constexpr Vector2D Zero{ 0, 0 };
	inline constexpr Vector2D One{ 1, 1 };
	inline constexpr Vector2D Up{ 0, 1 };
	inline constexpr Vector2D Down{ 0, -1 };
	inline constexpr Vector2D Left{ -1, 0 };
	inline constexpr Vector2D Right{ 1, 0 };
}
