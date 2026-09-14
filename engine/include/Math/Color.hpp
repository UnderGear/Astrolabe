#pragma once

#include <cstdint>

#include "Hardware/Register.hpp"

struct Color : Register<std::uint16_t>
{
	using Red = RegisterAccess<std::uint16_t, std::uint16_t, 0b00000000'00111110>;
	using Green = RegisterAccess<std::uint16_t, std::uint16_t, 0b00000111'11000000>;
	using Blue = RegisterAccess<std::uint16_t, std::uint16_t, 0b11111000'00000000>;

	constexpr explicit Color() = default;
	
	constexpr explicit Color(std::uint16_t R, std::uint16_t G, std::uint16_t B)
	{
		SetBatch<Red, Green, Blue>(R, G, B);
	}
};

namespace Colors
{
	constexpr inline Color RED{ 0b11111, 0, 0 };
	constexpr inline Color GREEN{ 0, 0b11111, 0 };
	constexpr inline Color BLUE{ 0, 0, 0b11111 };
	constexpr inline Color WHITE{ 0b11111, 0b11111, 0b11111 };
	constexpr inline Color BLACK{ 0, 0, 0 };
	constexpr inline Color YELLOW{ 0b11111, 0b11111, 0 };
	constexpr inline Color CYAN{ 0, 0b11111, 0b11111 };
	constexpr inline Color MAGENTA{ 0b11111, 0, 0b11111 };
}
