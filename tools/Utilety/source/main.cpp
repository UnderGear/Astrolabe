#include <algorithm>
#include <array>
#include <bit>
#include <charconv>
#include <cstdint>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "BackgroundDescription.hpp"
#include "Codegen.hpp"
#include "PaletteColor.hpp"
#include "PaletteDescription.hpp"
#include "PaletteIO.hpp"
#include "Pixel.hpp"
#include "SpriteDimension.hpp"
#include "SpritesheetDescription.hpp"
#include "TileMap.hpp"

// Tools
//  - Utilety
// Assets
//  - Sprites/*
//  - Backgrounds/*
// 
// Leaf subfolders of Sprites or Backgrounds share a palette

inline constexpr auto TileWidth{ 8 };
inline constexpr auto TileHeight{ 8 };

//TODO: look into SIMD and/or parallelization if it seems slow on larger sheets

template <auto Deleter>
struct DeleterFunc
{
	template <typename T>
	void operator()(T* Pointer) const
	{
		Deleter(Pointer);
	}
};

using stbi_image_ptr = std::unique_ptr<unsigned char, DeleterFunc<&stbi_image_free>>;

PaletteDescription LoadPalette(const std::filesystem::directory_entry& Directory, bool AddAlpha)
{
	//TODO: I'm thinking that we should use palette banks for sprites instead of full palettes
	static constexpr auto PaletteSuffix{ ".pal" };
	auto PaletteFileName{ Directory.path().filename().string() + PaletteSuffix };
	auto PalettePath{ Directory.path() / PaletteFileName };
	std::vector<PaletteColor> Palette;
	PaletteIO::LoadPalette(PalettePath, Palette, AddAlpha);
	std::map<PaletteColor, std::uint16_t> PaletteMapping;
	for (std::uint16_t i{ 0 }; i < Palette.size(); ++i)
	{
		PaletteMapping[Palette[i]] = i;
	}

	return PaletteDescription{ PalettePath, PaletteMapping, Palette };
}

static const std::filesystem::path CogedenPath{ "include/Assets/" };

template<typename PixelT>
std::pair<std::vector<std::uint32_t>, bool> UpdatePalette(PaletteDescription& PaletteDesc,
	std::span<PixelT> Pixels, int Width, int Height, std::size_t TilesIndicesPerTilePack)
{
	// Associate bitmap pixels with palette indices, add new colors to the palette
	std::vector<std::uint16_t> Indices;
	Indices.reserve(Width * Height);
	auto HasPaletteChanged{ false };
	for (auto& Sample : Pixels)
	{
		std::uint16_t PaletteIndex{ 0 };

		// If it's transparent, don't bother adding it to the palette. Use the default index 0 (which is always transparent in GBA)
		if (Sample.HasAlpha())
		{
			Indices.push_back(PaletteIndex);
			continue;
		}

		auto SampleColor{ Sample.ToColor() };
		auto Iter{ PaletteDesc.PaletteMapping.find(SampleColor) };
		if (Iter == PaletteDesc.PaletteMapping.end())
		{
			PaletteDesc.Palette.push_back(SampleColor);
			PaletteIndex = static_cast<std::uint16_t>(PaletteDesc.Palette.size() - 1);
			PaletteDesc.PaletteMapping[SampleColor] = PaletteIndex;
			HasPaletteChanged = true;
		}
		else
		{
			PaletteIndex = Iter->second;
		}

		Indices.push_back(PaletteIndex);
	}

	//TODO: if we're using a bank, we'll have to pack 8 colors in and not shift as far

	// Pack palette indices into uint32s to make it faster for the GBA hardware to process
	std::vector<std::uint32_t> PackedIndices;
	PackedIndices.reserve(Indices.size() / TilesIndicesPerTilePack);
	for (std::size_t i{ 0 }; i < Indices.size(); i += TilesIndicesPerTilePack)
	{
		std::uint32_t Value{ 0 };
		// Shift the remaining indices into our ui32
		for (std::size_t j{ 0 }; j < TilesIndicesPerTilePack; ++j)
		{
			Value |= (static_cast<std::uint32_t>(std::bit_cast<std::uint16_t>(Indices[i + j])) << (32 / TilesIndicesPerTilePack * j));
		}

		PackedIndices.push_back(Value);
	}

	return { PackedIndices, HasPaletteChanged };
}

void WritePalette(const std::filesystem::directory_entry& Directory, const PaletteDescription& PaletteDesc)
{
	// Save out the new palette binary file
	PaletteIO::WritePaletteFile(PaletteDesc.PalettePath, PaletteDesc.Palette);

	// Generate the new palette header
	std::vector<std::uint32_t> PackedPalette;
	static constexpr auto ColorsPerPalettePack{ sizeof(std::uint32_t) / sizeof(PaletteColor) };
	static constexpr auto MaxPaletteSize{ 128 };
	PackedPalette.reserve(MaxPaletteSize);

	// Pack palette colors into uint32s to make it faster for the GBA hardware to process
	for (std::size_t i{ 0 }; i < MaxPaletteSize * ColorsPerPalettePack; i += ColorsPerPalettePack)
	{
		std::uint32_t Value{ 0 };
		if (i < PaletteDesc.Palette.size())
		{
			Value = std::bit_cast<std::uint16_t>(PaletteDesc.Palette[i]);
		}
		if (i + 1 < PaletteDesc.Palette.size())
		{
			Value |= (static_cast<std::uint32_t>(std::bit_cast<std::uint16_t>(PaletteDesc.Palette[i + 1])) << 16);
		}

		PackedPalette.push_back(Value);
	}

	std::filesystem::path HeaderName{ Directory.path().stem().string() + "_palette" };
	Codegen::GeneratePaletteHeader((CogedenPath / HeaderName).string(), HeaderName.string(), PackedPalette);
}

void ProcessSpriteDirectory(const std::filesystem::directory_entry& Directory)
{
	auto PaletteDesc{ LoadPalette(Directory, true) };

	auto ShouldWritePaletteFiles{ false };
	for (const auto& Entry : std::filesystem::directory_iterator{ Directory })
	{
		// If it's not an anim description file, continue
		if (Entry.path().extension() != ".ad")
			continue;

		SpritesheetDescription Desc;
		//TODO: parse it from the entry path.
		std::ifstream DescFile{ Entry.path() };
		std::string Buffer;
		std::getline(DescFile, Buffer, ',');
		Desc.SpriteWidth = static_cast<SpriteDimension>(std::stoi(Buffer));
		std::getline(DescFile, Buffer);
		Desc.SpriteHeight = static_cast<SpriteDimension>(std::stoi(Buffer));

		std::getline(DescFile, Buffer);
		int FileCount{ std::stoi(Buffer) };
		Desc.AnimationSetDescriptions.reserve(FileCount);

		for (int FileIndex{ 0 }; FileIndex < FileCount; ++FileIndex)
		{
			SpriteAnimationSetDescription AnimDesc;
			std::getline(DescFile, Buffer);
			AnimDesc.FilePath = Buffer;

			std::getline(DescFile, Buffer, ',');
			AnimDesc.AnimFrameCount = std::stoi(Buffer);
			std::getline(DescFile, Buffer);
			AnimDesc.AnimCount = std::stoi(Buffer);
			Desc.TotalAnimationCount += AnimDesc.AnimCount;

			AnimDesc.AnimIndices.reserve(AnimDesc.AnimCount);
			for (int AnimIndex{ 0 }; AnimIndex < AnimDesc.AnimCount; ++AnimIndex)
			{
				std::vector<int> AnimIndices;
				AnimIndices.reserve(AnimDesc.AnimFrameCount);

				std::getline(DescFile, Buffer);
				for (auto IndicesString : std::views::split(Buffer, ','))
				{
					int PanelIndex{ 0 };
					std::string_view sv(&*IndicesString.begin(), std::ranges::distance(IndicesString));
					std::from_chars(sv.data(), sv.data() + sv.size(), PanelIndex);
					AnimIndices.push_back(PanelIndex);
				}

				AnimDesc.AnimIndices.push_back(std::move(AnimIndices));
			}

			std::getline(DescFile, Buffer);
			for (auto DurationString : std::views::split(Buffer, ','))
			{
				int AnimDuration{ 0 };
				std::string_view sv(&*DurationString.begin(), std::ranges::distance(DurationString));
				std::from_chars(sv.data(), sv.data() + sv.size(), AnimDuration);
				AnimDesc.FrameDurations.push_back(AnimDuration);
			}

			std::getline(DescFile, Buffer);
			for (auto SuffixString : std::views::split(Buffer, ','))
			{
				std::string_view sv(&*SuffixString.begin(), std::ranges::distance(SuffixString));
				AnimDesc.AnimSuffixes.push_back(std::string{ sv });
			}

			Desc.AnimationSetDescriptions.push_back(std::move(AnimDesc));
		}

		std::vector<std::vector<std::uint32_t>> SpriteTileIndices;

		for (const auto& AnimSetDesc : Desc.AnimationSetDescriptions)
		{
			//TODO: make sure it's the right extension
			auto FullPath{ Directory / AnimSetDesc.FilePath };

			int Width, Height, ChannelCount;
			stbi_image_ptr Bytes{ stbi_load(FullPath.string().c_str(), &Width, &Height, &ChannelCount, sizeof(Pixel)) };
			if (Bytes == nullptr)
			{
				//TODO: error messaging
				continue;
			}

			std::cout << FullPath << std::endl;

			std::span<Pixel> Pixels{ reinterpret_cast<Pixel*>(Bytes.get()), static_cast<std::size_t>(Width * Height) };

			static constexpr auto TilesIndicesPerTilePack{ 4 };
			auto [PackedIndices, HasPaletteChanged] { UpdatePalette(PaletteDesc, Pixels, Width, Height, TilesIndicesPerTilePack) };
			ShouldWritePaletteFiles = HasPaletteChanged;

			auto HorizontalTileCount{ Width / TileWidth };
			auto VerticalTileCount{ Height / TileHeight };
			auto HorizontalSpriteCount{ HorizontalTileCount / static_cast<int>(Desc.SpriteWidth) };
			auto VerticalSpriteCount{ VerticalTileCount / static_cast<int>(Desc.SpriteHeight) };
			auto SpriteCount{ HorizontalSpriteCount * VerticalSpriteCount };

			// Generate the new tile header
			SpriteTileIndices.reserve(SpriteTileIndices.size() + SpriteCount);

			// Loop over sprites, 
			for (int SpriteIndex{ 0 }; SpriteIndex < SpriteCount; ++SpriteIndex)
			{
				//TODO: this could probably be done in-place by swapping entries and some really fiddly indexing
				auto SpriteY{ SpriteIndex / HorizontalSpriteCount };
				auto SpriteX{ SpriteIndex % HorizontalSpriteCount };
				auto SpriteTilesStartY{ static_cast<int>(Desc.SpriteHeight) * SpriteY };
				auto SpriteTilesStartX{ static_cast<int>(Desc.SpriteWidth) * SpriteX };

				static constexpr auto HorizontalTileMultiplier{ TileWidth / TilesIndicesPerTilePack };
				std::vector<std::uint32_t> AdjustedIndices;
				AdjustedIndices.reserve(HorizontalTileMultiplier * TileHeight);

				//TODO: consider deduplicating tiles
				// make an explicit tile type made of 16 packed u32s
				// keep a set of tiles similar to the .pal asset
				// generate a header of an array of the character's tiles
				// sprite assets can be an array of indices into the character tile array
				// loading a sprite would be done tile by tile by index
				// indices could also be packed
				for (int SpriteTileY{ SpriteTilesStartY }; SpriteTileY < SpriteTilesStartY + static_cast<int>(Desc.SpriteWidth); ++SpriteTileY)
				{
					for (int SpriteTileX{ SpriteTilesStartX }; SpriteTileX < SpriteTilesStartX + static_cast<int>(Desc.SpriteWidth); ++SpriteTileX)
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

				SpriteTileIndices.push_back(std::move(AdjustedIndices));
			}
		}

		std::filesystem::path HeaderName{ Directory.path().stem() };
		if (SpriteTileIndices.size() > 0)
		{
			Codegen::GenerateSpriteTileHeader(CogedenPath, HeaderName, SpriteTileIndices, Desc);
		}
	}

	if (ShouldWritePaletteFiles)
	{
		WritePalette(Directory, PaletteDesc);
	}
}

//TODO: consolidate more code with sprite processing

//TODO: add an option to use a palette bank asset instead of a full fat palette
void ProcessBackgroundDirectory(const std::filesystem::directory_entry& Directory)
{
	auto PaletteDesc{ LoadPalette(Directory, false) };

	auto ShouldWritePaletteFiles{ true }; //TODO: don't always write the palette?
	for (const auto& Entry : std::filesystem::directory_iterator{ Directory })
	{
		// If it's not a background description file, continue
		if (Entry.path().extension() != ".bg")
			continue;

		BackgroundDescription Desc;
		std::ifstream DescFile{ Entry.path() };
		std::string Buffer;
		std::getline(DescFile, Buffer);
		Desc.FilePath = Buffer;
		std::getline(DescFile, Buffer);
		Desc.BGType = static_cast<BackgroundType>(std::stoi(Buffer));

		auto FullPath{ Directory / Desc.FilePath };

		int Width, Height, ChannelCount;
		stbi_image_ptr Bytes{ stbi_load(FullPath.string().c_str(), &Width, &Height, &ChannelCount, sizeof(PixelNoAlpha)) };
		if (Bytes == nullptr)
		{
			//TODO: error messaging
			continue;
		}

		std::cout << FullPath << std::endl;

		//TODO: we have width and height. we should validate dimensions with the bg type

		std::span<PixelNoAlpha> Pixels{ reinterpret_cast<PixelNoAlpha*>(Bytes.get()), static_cast<std::size_t>(Width * Height) };

		static constexpr auto TilesIndicesPerTilePack{ 8 };
		auto [PackedIndices, HasPaletteChanged] { UpdatePalette<PixelNoAlpha>(PaletteDesc, Pixels, Width, Height, TilesIndicesPerTilePack) };
		ShouldWritePaletteFiles = HasPaletteChanged;

		auto HorizontalTileCount{ Width / TileWidth };
		auto VerticalTileCount{ Height / TileHeight };
		auto TileCount{ HorizontalTileCount * VerticalTileCount };

		//std::cout << "width: " << Width << ", height: " << Height << ", channel count: " << ChannelCount << std::endl;

		std::vector<std::array<std::uint32_t, TileHeight>> Tiles;
		Tiles.reserve(TileCount);
		std::vector<std::vector<TileMapEntry>> TileMaps;
		constexpr std::size_t TileMapDimension{ 32 };
		constexpr std::size_t TileMapSize{ TileMapDimension * TileMapDimension }; // Each tile map is 32x32 tiles
		auto TileMapCount{ TileCount / TileMapSize };
		TileMaps.reserve(TileMapCount);
		//std::cout << "tile count: " << TileCount << ", tile map count: " << TileMapCount << std::endl;
		for (auto TileMapIndex{ 0 }; TileMapIndex < TileMapCount; ++TileMapIndex)
		{
			TileMaps.push_back({});
			TileMaps[TileMapIndex].reserve(TileCount / TileMapCount);
		}

		// Loop over tiles
		for (int TileIndex{ 0 }; TileIndex < TileCount; ++TileIndex)
		{
			auto TileY{ TileIndex / HorizontalTileCount };
			auto TileX{ TileIndex % HorizontalTileCount };

			// Build up the current tile by the packed palette indices it contains
			std::array<std::uint32_t, TileHeight> Tile;
			for (int TileRowIndex{ 0 }; TileRowIndex < TileHeight; ++TileRowIndex)
			{
				// The Y offset has to take into account the tile's y plus additional horizontal rows per row index within the tile
				auto PaletteY{ TileY * HorizontalTileCount * TileHeight + TileRowIndex * HorizontalTileCount };
				auto PaletteX{ TileX };
				auto PaletteIndex{ (PaletteY + PaletteX) };

				Tile[TileRowIndex] = PackedIndices[PaletteIndex];
			}

			//TODO try the tile flipped vertically, horizontally, and both
			// If we have a matching tile already, just set our flip bits when inserting into the tile map and don't add a new tile
			auto TileIter{ std::find(Tiles.begin(), Tiles.end(), Tile) };
			if (TileIter == Tiles.end())
			{
				Tiles.push_back(Tile);
				TileIter = std::prev(Tiles.end());
			}

			// Now add the tile to the tile map at the appropriate index
			// based on the map layout, we need to split this into multiple background maps
			// each map is 32x32 tiles:
			// 32x32 - 0
			// 64x32 - 0, 1
			// 32x64 - 0
			//         1
			// 64x64 - 0, 1
			//         2, 3
			TileMapEntry MapEntry;
			MapEntry.TileIndex = static_cast<std::uint16_t>(std::distance(Tiles.begin(), TileIter));
			MapEntry.HorizontalFlip = 0;
			MapEntry.VerticalFlip = 0;
			MapEntry.PaletteBank = 0;

			auto MapIndex{ TileY / TileMapDimension + TileX / TileMapDimension };
			TileMaps[MapIndex].push_back(std::move(MapEntry));
		}

		// The current Brin bg has 41, (should be 31 when we handle flips)
		//std::cout << '\n' << "unique tile count: " << Tiles.size() << std::endl;

		std::filesystem::path HeaderName{ Directory.path().stem() };
		if (Tiles.size() > 0)
		{
			std::vector<std::uint32_t> FlattenedTiles;
			for (auto& Tile : Tiles)
			{
				for (auto& TileRow : Tile)
				{
					FlattenedTiles.push_back(TileRow);
				}
			}

			std::vector<TileMapEntry> AdjustedTileMap;
			AdjustedTileMap.reserve(TileCount);
			for (auto& TileMap : TileMaps)
			{
				for (auto& Tile : TileMap)
				{
					AdjustedTileMap.push_back(Tile);
				}
			}

			Codegen::GenerateBackgroundHeader(CogedenPath, HeaderName, FlattenedTiles, AdjustedTileMap, HorizontalTileCount, VerticalTileCount);
		}
	}

	if (ShouldWritePaletteFiles)
	{
		WritePalette(Directory, PaletteDesc);
	}
	// std::cout << "Palette size: " << PaletteDesc.Palette.size() << std::endl;
	// for (auto& PaletteColor : PaletteDesc.Palette)
	// {
	// 	std::cout << PaletteColor.R << ", " << PaletteColor.G << ", " << PaletteColor.B << std::endl;
	// }
}

int main()
{
	for (const auto& Entry : std::filesystem::recursive_directory_iterator{ "assets/sprites/" })
	{
		if (Entry.is_directory())
		{
			ProcessSpriteDirectory(Entry);
		}
	}

	for (const auto& Entry : std::filesystem::recursive_directory_iterator{ "assets/backgrounds/" })
	{
		if (Entry.is_directory())
		{
			ProcessBackgroundDirectory(Entry);
		}
	}

	return 0;
}
