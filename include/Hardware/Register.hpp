#pragma once

#include <bit>
#include <concepts>
#include <type_traits>

enum class RegisterAccessType
{
	ReadWrite,
	ReadOnly,
	WriteOnly,
};

template<std::unsigned_integral BackingT, typename ValueT, BackingT MaskValue, RegisterAccessType AccessType = RegisterAccessType::ReadWrite>
struct RegisterAccess
{
	static constexpr BackingT Mask{ MaskValue };
	static constexpr auto Shift{ std::countr_zero(Mask) };

	static ValueT Get(const volatile BackingT& RawRegister) requires (AccessType != RegisterAccessType::WriteOnly)
	{
		auto RawResult{ static_cast<BackingT>((RawRegister & Mask) >> Shift) };

		if constexpr(std::signed_integral<ValueT> && !std::is_enum_v<ValueT>)
		{
			return std::bit_cast<ValueT>(static_cast<std::make_unsigned_t<BackingT>>(RawResult));
		}

		return static_cast<ValueT>(RawResult);
	}

	static void Set(volatile BackingT& RawRegister, ValueT Value) requires (AccessType != RegisterAccessType::ReadOnly)
	{
		BackingT ConvertedValue;

		if constexpr(std::signed_integral<ValueT> && !std::is_enum_v<ValueT>)
		{
			ConvertedValue = static_cast<BackingT>(std::bit_cast<std::make_unsigned_t<ValueT>>(Value));
		}
		else
		{
			ConvertedValue = static_cast<BackingT>(Value);
		}

		RawRegister = (RawRegister & ~Mask) | static_cast<BackingT>((ConvertedValue << Shift) & Mask);
	}
};

template<std::unsigned_integral BackingT, RegisterAccessType AccessType = RegisterAccessType::ReadWrite>
struct Register
{
	BackingT Data;

	template<typename RegisterAccessT>
	auto Get() const volatile requires (AccessType != RegisterAccessType::WriteOnly)
	{
		return RegisterAccessT::Get(Data);
	}

	template<typename RegisterAccessT>
	void Set(auto Value) volatile requires (AccessType != RegisterAccessType::ReadOnly)
	{
		RegisterAccessT::Set(Data, Value);
	}

	template<typename... RegisterAccessTs, typename... ValueTs>
	void SetBatch(ValueTs... Values) volatile requires (AccessType != RegisterAccessType::ReadOnly)
	{
		BackingT ToSet{ 0 };
		(RegisterAccessTs::Set(ToSet, Values), ...);
		Data = ToSet;
	}
};
