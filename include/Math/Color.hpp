#pragma once

#include <cstdint>
//#include <bit>

struct Color
{
	std::uint16_t Padding : 1{ 0 };
	std::uint16_t Red : 5{ 0 };
	std::uint16_t Green : 5{ 0 };
	std::uint16_t Blue : 5{ 0 };

	constexpr explicit Color() noexcept = default;
	
	constexpr explicit Color(std::uint16_t R, std::uint16_t G, std::uint16_t B) noexcept
		: Red(R), Green(G), Blue(B)
	{ }

	//constexpr operator std::uint16_t() const noexcept { return std::bit_cast<std::uint16_t>(*this); }
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
