#pragma once

#include <cstdint>

struct PaletteColor;

struct Pixel
{
	std::uint8_t B{ 0 };
	std::uint8_t G{ 0 };
	std::uint8_t R{ 0 };
	std::uint8_t A{ 0 };

	PaletteColor ToColor() const;

	bool HasAlpha() const { return A == 0; }
};

struct PixelNoAlpha
{
	std::uint8_t B{ 0 };
	std::uint8_t G{ 0 };
	std::uint8_t R{ 0 };

	PaletteColor ToColor() const;

	bool HasAlpha() const { return false; }
};
