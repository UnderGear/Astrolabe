#pragma once

#include <cstdint>
#include <type_traits>

#include "Hardware/BIOS.hpp"

// Signed fixed-point value
template <typename SizeT, std::int32_t FractionalBitCount>
//TODO: requires clause that the fractional bit count is reasonable
//TODO: make sure ctor arg types fit into our Fixed
requires std::is_integral_v<SizeT> && std::is_signed_v<SizeT>
struct Fixed
{
	using BackingT = SizeT;
	static constexpr std::int32_t FractionalCount{ FractionalBitCount };
	SizeT Data{ 0 };

	constexpr Fixed() = default;

	template <typename IntegralT> requires std::is_integral_v<IntegralT>
	constexpr explicit Fixed(IntegralT Value)
	{
		Data = Value << FractionalBitCount;
	}

	template <typename IntegralT> requires std::is_integral_v<IntegralT>
	constexpr Fixed& operator=(IntegralT Value)
	{
		Data = Value << FractionalBitCount;
		return *this;
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

	template <typename IntegralT> requires std::is_integral_v<IntegralT>
	constexpr Fixed operator +(IntegralT Other) const
	{
		Fixed Result;
		Result.Data = Data + (Other << FractionalBitCount);
		return Result;
	}

	constexpr Fixed& operator +=(Fixed Other)
	{
		Data += Other.Data;
		return *this;
	}

	template <typename IntegralT> requires std::is_integral_v<IntegralT>
	constexpr Fixed& operator +=(IntegralT Other)
	{
		Data += (Other << FractionalBitCount);
		return *this;
	}

	constexpr Fixed& operator ++()
	{
		Data += (1 << FractionalBitCount);
		return *this;
	}

	constexpr Fixed operator ++(int)
	{
		Fixed Result = *this;
		++(*this);
		return Result;
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

	template <typename IntegralT> requires std::is_integral_v<IntegralT>
	constexpr Fixed operator -(IntegralT Other) const
	{
		Fixed Result;
		Result.Data = Data - (Other << FractionalBitCount);
		return Result;
	}

	constexpr Fixed& operator -=(Fixed Other)
	{
		Data -= Other.Data;
		return *this;
	}

	template <typename IntegralT> requires std::is_integral_v<IntegralT>
	constexpr Fixed& operator -=(IntegralT Other)
	{
		Data -= (Other << FractionalBitCount);
		return *this;
	}

	constexpr Fixed& operator --()
	{
		Data -= (1 << FractionalBitCount);
		return *this;
	}

	constexpr Fixed operator --(int)
	{
		Fixed Result = *this;
		--(*this);
		return Result;
	}

	constexpr Fixed operator *(Fixed Other) const
	{
		Fixed Result;
		Result.Data = (Data * Other.Data) >> FractionalBitCount;
		return Result;
	}

	template <typename IntegralT> requires std::is_integral_v<IntegralT>
	constexpr Fixed operator *(IntegralT Other) const
	{
		Fixed Result;
		Result.Data = (Data * (Other << FractionalBitCount)) >> FractionalBitCount;
		return Result;
	}

	constexpr Fixed& operator *=(Fixed Other)
	{
		Data *= Other.Data;
		Data >>= FractionalBitCount;
		return *this;
	}

	template <typename IntegralT> requires std::is_integral_v<IntegralT>
	constexpr Fixed& operator *=(IntegralT Other)
	{
		Data *= Other << FractionalBitCount;
		Data >>= FractionalBitCount;
		return *this;
	}

	constexpr Fixed operator /(Fixed Other) const
	{
		Fixed Result;
		Result.Data = (Data * (1 << FractionalBitCount)) / Other.Data;
		return Result;
	}
	
	template <typename IntegralT> requires std::is_integral_v<IntegralT>
	constexpr Fixed operator /(IntegralT Other) const
	{
		Fixed Result;
		Result.Data = (Data * (1 << FractionalBitCount)) / (Other << FractionalBitCount);
		return Result;
	}

	constexpr Fixed& operator /=(Fixed Other)
	{
		Data *= (1 << FractionalBitCount);
		Data /= Other.Data;
		return *this;
	}

	template <typename IntegralT> requires std::is_integral_v<IntegralT>
	constexpr Fixed& operator /=(IntegralT Other)
	{
		Data *= (1 << FractionalBitCount);
		Data /= (Other << FractionalBitCount);
		return *this;
	}

	constexpr auto operator <=>(const Fixed& Other) const = default;

	template <typename IntegralT> requires std::is_integral_v<IntegralT>
	constexpr auto operator <=>(const IntegralT& Other) const
	{
		return Data <=> (Other << FractionalBitCount);
	}

	//TOOD: make a LUT? try a different iteration count? weigh expense vs needed precision vs memory footprint on gba
	// maybe that decision can be made down the line
	// could try the BIOS sqrt function. shift left as far as possible, invoke, shift back
	constexpr friend Fixed Sqrt(Fixed Value)
	{
		static constexpr std::int32_t SqrtShift{ Fixed::FractionalCount % 2 == 0 ? Fixed::FractionalCount : Fixed::FractionalCount + 1 };
		static constexpr std::int32_t SqrtShiftBack{ Fixed::FractionalCount % 2 == 0 ? 0 : 1 };

		Fixed Root;

		auto RawValue{ static_cast<std::uint32_t>(Value.Data) };
		std::int32_t Shift{ std::countl_zero(RawValue) };

		if (Shift <= SqrtShift)
		{
			auto RawRoot{ BIOS::Sqrt(RawValue << SqrtShift) };
			Root.Data = static_cast<Fixed::BackingT>(RawRoot >> SqrtShiftBack);
		}
		else
		{
			Fixed X{ Value };
			constexpr auto Iterations{ 20 };
			for (auto i{ 0 }; i < Iterations; ++i)
			{
				Root = (X + (Value / X)) / 2;
				X = Root;
			}
		}

		return Root;
	}
};

//TODO: looks like we'll need some functions. transcendentals
//TODO: floor, ceil, round, abs, fractional part, integral part

//TODO: concept to represent integral types and fixed point types

//TODO: is this the name we're going with?
using i24f8_t = Fixed<std::int32_t, 8>;
consteval i24f8_t operator ""_i24f8(long double Value)
{
	return i24f8_t{ static_cast<float>(Value) };
}
constexpr i24f8_t operator ""_i24f8(unsigned long long Value)
{
	return i24f8_t{ Value };
}

using i8f8_t = Fixed<std::int16_t, 8>;
consteval i8f8_t operator ""_i8f8(long double Value)
{
	return i8f8_t{ static_cast<float>(Value) };
}
constexpr i8f8_t operator ""_i8f8(unsigned long long Value)
{
	return i8f8_t{ Value };
}
