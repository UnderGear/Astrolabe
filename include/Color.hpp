#pragma once

#include <cstdint>
//#include <bit>

struct Color
{
	std::uint16_t Padding : 1;
	std::uint16_t Red : 5;
	std::uint16_t Green : 5;
	std::uint16_t Blue : 5;

	constexpr explicit Color() noexcept = default;
	
	constexpr explicit Color(std::uint16_t R, std::uint16_t G, std::uint16_t B) noexcept
		: Padding(0), Red(R), Green(G), Blue(B)
	{ }

	//constexpr operator std::uint16_t() const noexcept { return std::bit_cast<std::uint16_t>(*this); }
};

namespace Colors
{
	constexpr inline Color RED{ 0xF, 0x0, 0x0 };
	constexpr inline Color GREEN{ 0x0, 0xF, 0x0 };
	constexpr inline Color BLUE{ 0x0, 0x0, 0xF };
	constexpr inline Color WHITE{ 0xF, 0xF, 0xF };
	constexpr inline Color BLACK{ 0x0, 0x0, 0x0 };
	constexpr inline Color YELLOW{ 0xF, 0xF, 0x0 };
	constexpr inline Color CYAN{ 0x0, 0xF, 0xF };
	constexpr inline Color MAGENTA{ 0xF, 0x0, 0xF };
}
