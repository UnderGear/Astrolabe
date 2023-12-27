#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <vector>

#include "PaletteColor.hpp"

namespace PaletteIO
{
	void LoadPalette(std::filesystem::path FileName, std::vector<PaletteColor>& Palette)
	{
		std::ifstream PaletteFile{ FileName, std::ios::binary | std::ios::ate };
		auto Position{ PaletteFile.tellg() };
		if (Position <= 0)
		{
			// No/empty palette file. Add the default color
			Palette.emplace_back(0, 0, 0);
			return;
		}

		auto Size{ (static_cast<std::size_t>(Position) - 1) / sizeof(std::uint16_t) + 1 };
		Palette.reserve(Size);
		PaletteFile.seekg(0, std::ios::beg);
		std::vector<std::byte> RawData{ static_cast<std::size_t>(Position) };
		PaletteFile.read(reinterpret_cast<char*>(RawData.data()), static_cast<std::size_t>(Position));
		
		for (std::size_t i{ 0 }; i < RawData.size(); i += sizeof(std::uint16_t))
		{
			auto RawColor{ static_cast<std::uint16_t>(RawData[i]) };
			if (i + 1 < RawData.size())
			{
				RawColor |= static_cast<std::uint16_t>(RawData[i + 1]) << 8;
			}
			Palette.push_back(std::bit_cast<PaletteColor>(RawColor));
		}
	}

	void WritePaletteFile(std::filesystem::path FileName, const std::vector<PaletteColor>& Palette)
	{
		std::ofstream PaletteFile{ FileName, std::ios::binary };
		
		PaletteFile.write(reinterpret_cast<const char*>(Palette.data()), sizeof(std::uint16_t) * Palette.size());
	}
}
