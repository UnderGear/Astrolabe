#pragma once

#include <cstdint>

#include "PaletteColor.hpp"

struct Pixel
{
	std::uint8_t B{ 0 };
	std::uint8_t G{ 0 };
	std::uint8_t R{ 0 };
	std::uint8_t A{ 0 };

	PaletteColor ToColor() const
	{
		if (A == 0)
			return PaletteColor{ 0, 0, 0 };

		static constexpr auto FiveBitMax{ 0b11111 };
		std::uint16_t NewB{ static_cast<std::uint16_t>(static_cast<std::uint16_t>(B * FiveBitMax) / std::numeric_limits<std::uint8_t>::max()) };
		std::uint16_t NewG{ static_cast<std::uint16_t>(static_cast<std::uint16_t>(G * FiveBitMax) / std::numeric_limits<std::uint8_t>::max()) };
		std::uint16_t NewR{ static_cast<std::uint16_t>(static_cast<std::uint16_t>(R * FiveBitMax) / std::numeric_limits<std::uint8_t>::max()) };
		return PaletteColor{ NewB, NewG, NewR };
	}
};
