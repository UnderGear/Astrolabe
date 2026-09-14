#pragma once

#include <cstdint>
#include <memory>

#include "Hardware/Register.hpp"
#include "MemoryMap.hpp"
#include "Math/Vector.hpp"

enum class InputKey : std::uint16_t
{
	A =	  0b00000000'00000001,
	B =	  0b00000000'00000010,
	Select = 0b00000000'00000100,
	Start =  0b00000000'00001000,
	Right =  0b00000000'00010000,
	Left =   0b00000000'00100000,
	Up =	 0b00000000'01000000,
	Down =   0b00000000'10000000,
	RB =	 0b00000001'00000000,
	LB =	 0b00000010'00000000,
};

constexpr inline std::uint16_t KeyMask{ 0b00000011'11111111 };

struct InputRegister : Register<std::uint16_t, RegisterAccessType::ReadOnly>
{
	// relying on GetRaw() from the base class instead of specifying individual accesses
};

struct InputInterruptRegister : Register<std::uint16_t>
{
	using A = RegisterAccess<std::uint16_t, bool, static_cast<std::uint16_t>(InputKey::A)>;
	using B = RegisterAccess<std::uint16_t, bool, static_cast<std::uint16_t>(InputKey::B)>;
	using Select = RegisterAccess<std::uint16_t, bool, static_cast<std::uint16_t>(InputKey::Select)>;
	using Start = RegisterAccess<std::uint16_t, bool, static_cast<std::uint16_t>(InputKey::Start)>;
	using Right = RegisterAccess<std::uint16_t, bool, static_cast<std::uint16_t>(InputKey::Right)>;
	using Left = RegisterAccess<std::uint16_t, bool, static_cast<std::uint16_t>(InputKey::Left)>;
	using Up = RegisterAccess<std::uint16_t, bool, static_cast<std::uint16_t>(InputKey::Up)>;
	using Down = RegisterAccess<std::uint16_t, bool, static_cast<std::uint16_t>(InputKey::Down)>;
	using RB = RegisterAccess<std::uint16_t, bool, static_cast<std::uint16_t>(InputKey::RB)>;
	using LB = RegisterAccess<std::uint16_t, bool, static_cast<std::uint16_t>(InputKey::LB)>;

	using IRQEnable = RegisterAccess<std::uint16_t, bool, 0b01000000'00000000>;
	using Comparitor = RegisterAccess<std::uint16_t, bool, 0b10000000'00000000>; // 0 OR, 1 AND comparison of all set keys required to raise the interrupt
};

class Input
{
	std::unique_ptr<const volatile InputRegister> InputValues
	{
		new(reinterpret_cast<void*>(MemoryMap::INPUT_ADDRESS)) InputRegister
	};

	std::unique_ptr<volatile InputInterruptRegister> InputInterruptControl
	{
		new(reinterpret_cast<void*>(MemoryMap::INPUT_INTERRUPT_ADDRESS)) InputInterruptRegister
	};

	//TODO: use an InputKey backing type?
	std::uint16_t CurrentInput{ 0 };
	std::uint16_t PreviousInput{ 0 };

	Vector2D DPadInput{ Vector::Zero };
	Vector2D PreviousDPadInput{ Vector::Zero };

public:

	void Tick();

	bool IsKeyDown(InputKey Key) const;
	bool IsKeyUp(InputKey Key) const;

	bool WasKeyDown(InputKey Key) const;
	bool WasKeyUp(InputKey Key) const;

	bool WasKeyPressed(InputKey Key) const;
	bool WasKeyReleased(InputKey Key) const;

	Vector2D GetDPadInput() const;
	Vector2D GetPreviousDPadInput() const;
};
