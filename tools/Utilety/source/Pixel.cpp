#include "Pixel.hpp"

#include <limits>

#include "PaletteColor.hpp"

inline constexpr std::uint16_t ConvertColorChannel(std::uint8_t ColorChannel)
{
	constexpr auto FiveBitMax{ 0b11111 };
	return static_cast<std::uint16_t>(static_cast<std::uint16_t>(ColorChannel * FiveBitMax) / std::numeric_limits<std::uint8_t>::max());
}

PaletteColor Pixel::ToColor() const
{
	if (A == 0)
	{
		return PaletteColor{ 0, 0, 0 };
	}

	return PaletteColor{ ConvertColorChannel(B), ConvertColorChannel(G), ConvertColorChannel(R) };
}

PaletteColor PixelNoAlpha::ToColor() const
{
	return PaletteColor{ ConvertColorChannel(B), ConvertColorChannel(G), ConvertColorChannel(R) };
}
