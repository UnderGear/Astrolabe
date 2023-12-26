#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <limits>
#include <map>
#include <span>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Codegen.hpp"
#include "PaletteColor.hpp"
#include "PaletteIO.hpp"
#include "Pixel.hpp"
#include "SpriteDimension.hpp"
#include "TilingMode.hpp"

//TODO: move this whole tool into the astrolabe project
// 
// Tools
//  - Utilety
// Assets
//  - Sprites/*
//  - Backgrounds/*
// 
// Leaf subfolders of Sprites or Backgrounds share a palette?

//TODO: drive this command line tool from make?


//TODO: should these be command line args?
inline constexpr auto TileWidth{ 8 };
inline constexpr auto TileHeight{ 8 };
 
//TODO: keep a registry of all assets in the project, not just the .pal file. this can probably

//TODO: background map support (separate tool entirely?)

//TODO: look into SIMD and/or parallelization if it seems slow on larger sheets

int main()
{
	auto Path{ "C:/Users/chris/Downloads/isaaclookalpha.bmp" }; //TODO: read this as a command line arg, validate the input file
	//TODO: what if we associate another description file with the raw spritesheet that at least names its entries?
	auto PalettePath{ "Palette.pal" }; //TODO: another command line arg
	auto ShouldWritePaletteFiles{ true };
	auto AssetName{ "TestTiles" };
	auto PaletteName{ "TestPalette" };
	auto TileMode{ TilingMode::Tiled };
	auto SpriteWidth{ SpriteDimension::p32 };
	auto SpriteHeight{ SpriteDimension::p32 };

	int Width, Height, ChannelCount;
	static constexpr auto PixelSize{ 4 };
	auto* Bytes{ stbi_load(Path, &Width, &Height, &ChannelCount, PixelSize) }; //TODO: RAII for this c array
	//TODO: support different input file types
	if (Bytes == nullptr)
	{
		//TODO: error messaging
		return 1;
	}

	std::span<Pixel> Pixels{ reinterpret_cast<Pixel*>(Bytes), static_cast<std::size_t>(Width * Height * ChannelCount / PixelSize) };

	// Load our palette
	std::vector<PaletteColor> Palette;
	PaletteIO::LoadPalette(PalettePath, Palette);
	std::map<PaletteColor, std::uint16_t> PaletteMapping;
	for (std::uint16_t i{ 0 }; i < Palette.size(); ++i)
	{
		PaletteMapping[Palette[i]] = i;
	}

	// Associate bitmap pixels with palette indices, add new colors to the palette
	auto HasPaletteChanged{ false };
	std::vector<std::uint16_t> Indices;
	Indices.reserve(Width * Height);
	for (auto& Sample : Pixels)
	{
		std::uint16_t PaletteIndex{ 0 };

		// If it's transparent, don't bother adding it to the palette. Use the default index 0 (which is always transparent in GBA)
		if (Sample.A == 0)
		{
			Indices.push_back(PaletteIndex);
			continue;
		}

		auto SampleColor{ Sample.ToColor() };
		auto Iter{ PaletteMapping.find(SampleColor) };
		if (Iter == PaletteMapping.end())
		{
			Palette.push_back(SampleColor);
			PaletteIndex = Palette.size() - 1;
			PaletteMapping[SampleColor] = PaletteIndex;
			HasPaletteChanged = true;
		}
		else
		{
			PaletteIndex = Iter->second;
		}

		Indices.push_back(PaletteIndex);
	}
	delete[] Bytes;

	if (HasPaletteChanged && ShouldWritePaletteFiles)
	{
		// Save out the new palette binary file
		PaletteIO::WritePaletteFile(PalettePath, Palette);

		// Generate the new palette header
		std::vector<std::uint32_t> PackedPalette;
		static constexpr auto ColorsPerPalettePack{ sizeof(std::uint32_t) / sizeof(PaletteColor) };
		static constexpr auto MaxPaletteSize{ 128 };
		PackedPalette.reserve(MaxPaletteSize);

		// Pack palette colors into uint32s to make it faster for the GBA hardware to process
		for (int i{ 0 }; i < MaxPaletteSize * ColorsPerPalettePack; i += ColorsPerPalettePack)
		{
			std::uint32_t Value{ 0 };
			if (i < Palette.size())
			{
				Value = std::bit_cast<std::uint16_t>(Palette[i]);
			}
			if (i + 1 < Palette.size())
			{
				Value |= (static_cast<std::uint32_t>(std::bit_cast<std::uint16_t>(Palette[i + 1])) << 16);
			}

			PackedPalette.push_back(Value);
		}
		Codegen::GeneratePaletteHeader("TestPalette", PackedPalette);
	}

	// Pack palette indices into uint32s to make it faster for the GBA hardware to process
	std::vector<std::uint32_t> PackedIndices;
	static constexpr auto TilesIndicesPerTilePack{ 4 };
	PackedIndices.reserve(Indices.size() / TilesIndicesPerTilePack);
	for (int i{ 0 }; i < Indices.size(); i += TilesIndicesPerTilePack)
	{
		std::uint32_t Value{ 0 };
		Value = std::bit_cast<std::uint16_t>(Indices[i]);
		// Shift the remaining indices into our ui32
		if (i + 1 < Indices.size())
		{
			Value |= (static_cast<std::uint32_t>(std::bit_cast<std::uint16_t>(Indices[i + 1])) << 8);
		}
		if (i + 2 < Indices.size())
		{
			Value |= (static_cast<std::uint32_t>(std::bit_cast<std::uint16_t>(Indices[i + 2])) << 16);
		}
		if (i + 3 < Indices.size())
		{
			Value |= (static_cast<std::uint32_t>(std::bit_cast<std::uint16_t>(Indices[i + 3])) << 24);
		}
		PackedIndices.push_back(Value);
	}

	auto HorizontalTileCount{ Width / TileWidth };
	auto VerticalTileCount{ Height / TileHeight };
	auto TileCount{ HorizontalTileCount * VerticalTileCount };
	auto HorizontalSpriteCount{ HorizontalTileCount / static_cast<int>(SpriteWidth) };
	auto VerticalSpriteCount{ VerticalTileCount / static_cast<int>(SpriteHeight) };
	auto SpriteCount{ HorizontalSpriteCount * VerticalSpriteCount };
	auto SpriteTileCount{ static_cast<int>(SpriteWidth) * static_cast<int>(SpriteHeight) };

	// Generate the new tile header
	if (TileMode == TilingMode::Tiled)
	{
		std::vector<std::vector<std::uint32_t>> SpriteTileIndices;
		SpriteTileIndices.resize(SpriteCount);

		// Loop over sprites, 
		for (int SpriteIndex{ 0 }; SpriteIndex < SpriteCount; ++SpriteIndex)
		{
			//TODO: this could probably be done in-place by swapping entries and some really fiddly indexing
			auto SpriteY{ SpriteIndex / HorizontalSpriteCount };
			auto SpriteX{ SpriteIndex % HorizontalSpriteCount };
			auto SpriteTilesStartY{ static_cast<int>(SpriteHeight) * SpriteY };
			auto SpriteTilesStartX{ static_cast<int>(SpriteWidth) * SpriteX };

			auto& AdjustedIndices{ SpriteTileIndices[SpriteIndex] };
			static constexpr auto HorizontalTileMultiplier{ TileWidth / TilesIndicesPerTilePack };
			AdjustedIndices.reserve(HorizontalTileMultiplier * TileHeight);
			
			// 
			for (int SpriteTileY{ SpriteTilesStartY }; SpriteTileY < SpriteTilesStartY + static_cast<int>(SpriteWidth); ++SpriteTileY)
			{
				for (int SpriteTileX{ SpriteTilesStartX }; SpriteTileX < SpriteTilesStartX + static_cast<int>(SpriteWidth); ++SpriteTileX)
				{
					auto TileStartY{ TileHeight * SpriteTileY };
					auto TileStartX{ HorizontalTileMultiplier * SpriteTileX };
					for (int OffsetY{ 0 }; OffsetY < TileHeight; ++OffsetY)
					{
						for (int OffsetX{ 0 }; OffsetX < HorizontalTileMultiplier; ++OffsetX)
						{
							auto CountPerLine{ HorizontalTileCount * HorizontalTileMultiplier };
							auto SampleY{ TileStartY + OffsetY };
							auto SampleX{ TileStartX + OffsetX };

							auto Index{ CountPerLine * SampleY + SampleX };
							AdjustedIndices.push_back(PackedIndices[Index]);
						}
					}
				}
				
			}
		}
		



		Codegen::GenerateTileHeader("TestTiles", SpriteTileIndices);
	}
	else // Bitmap
	{
		//TODO: we need to split out our tiles into sprites

		//Codegen::GenerateTileHeader("TestTiles", PackedIndices);
	}

	return 0;
}