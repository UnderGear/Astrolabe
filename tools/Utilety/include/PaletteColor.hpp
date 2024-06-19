#pragma once

#include <cstdint>

struct PaletteColor
{
	std::uint16_t B : 5;
	std::uint16_t G : 5;
	std::uint16_t R : 5;
	std::uint16_t Padding : 1{ 0 };

	explicit PaletteColor() = default;

	explicit PaletteColor(std::uint16_t InB, std::uint16_t InG, std::uint16_t InR)
		: B(InB), G(InG), R(InR) {}

	auto operator <=>(const PaletteColor&) const = default;
};
