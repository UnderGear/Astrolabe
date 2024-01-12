#pragma once

#include <cstdint>
#include <type_traits>

// Signed fixed-point value
template <typename SizeT, std::int32_t FractionalBitCount>
//TODO: requires clause that the fractional bit count is reasonable
requires std::is_integral_v<SizeT> && std::is_signed_v<SizeT>
struct Fixed
{
	SizeT Data{ 0 };

	constexpr Fixed() = default;

	template <typename IntegralT> requires std::is_integral_v<IntegralT>
	constexpr explicit Fixed(IntegralT Value)
	{
		Data = Value << FractionalBitCount;
	}

	// GBA doesn't support floating point operations, but we can initialize fixed at compile time
	consteval explicit Fixed(float Value)
	{
		Data = static_cast<SizeT>(Value * (1 << FractionalBitCount));
	}

	constexpr operator SizeT() const
	{
		//TODO: I guess I need to round rather than just truncate
		return Data >> FractionalBitCount;
	}

	constexpr Fixed operator +(Fixed Other) const
	{
		Fixed Result;
		Result.Data = Data + Other.Data;
		return Result;
	}

	Fixed& operator +=(Fixed Other)
	{
		Data += Other.Data;
		return *this;
	}

	constexpr Fixed operator -() const
	{
		Fixed Result;
		Result.Data = -Data;
		return Result;
	}

	constexpr Fixed operator -(Fixed Other) const
	{
		Fixed Result;
		Result.Data = Data - Other.Data;
		return Result;
	}

	Fixed& operator -=(Fixed Other)
	{
		Data -= Other.Data;
		return *this;
	}

	constexpr Fixed operator *(Fixed Other) const
	{
		Fixed Result;
		Result.Data = (Data * Other.Data) >> FractionalBitCount;
		return Result;
	}

	Fixed& operator *=(Fixed Other)
	{
		Data *= Other.Data;
		Data >>= FractionalBitCount;
		return *this;
	}

	constexpr Fixed operator /(Fixed Other) const
	{
		Fixed Result;
		Result.Data = (Data * (1 << FractionalBitCount)) / Other.Data;
		return Result;
	}

	Fixed& operator /=(Fixed Other)
	{
		Data *= (1 << FractionalBitCount);
		Data /= Other.Data;
		return *this;
	}

	constexpr friend Fixed sqrt(Fixed Value)
	{
		Fixed X{ Value };
		Fixed Root;

		constexpr auto Iterations{ 20 };
		for (auto i{ 0 }; i < Iterations; ++i)
		{
			Root = (X + (Value / X)) / Fixed{ 2 };
			X = Root;
		}

		return Root;
	}

	auto operator<=>(const Fixed& Other) const = default;
};

//TODO: looks like we'll need some functions. transcendentals, sqrt, etc

//TODO: is this the name we're going with?
using i24f8_t = Fixed<int, 8>;
