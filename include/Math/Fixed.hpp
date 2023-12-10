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

	//TODO: no implicit conversions

	constexpr Fixed() = default;

	constexpr explicit Fixed(SizeT Value)
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

	constexpr Fixed operator +=(Fixed Other)
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

	constexpr Fixed operator -=(Fixed Other)
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

	constexpr Fixed operator *=(Fixed Other)
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

	constexpr Fixed operator /=(Fixed Other)
	{
		Data *= (1 << FractionalBitCount);
		Data /= Other.Data;
		return *this;
	}
};

//TODO: is this the name we're going with?
using i24f8_t = Fixed<int, 8>;