#pragma once

#include <algorithm>
#include <bit>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <string_view>
#include <string>

#include "PaletteColor.hpp"
#include "SpritesheetDescription.hpp"
#include "TileMap.hpp"

namespace Codegen
{
	// Utilities for asset index tracking
	inline constexpr auto AssetIndexingFileName{ "assets/AssetIndexing" };

	int GetNextAssetIndex();

	void WriteLastUsedAssetIndex(int Index);

	static constexpr auto EntriesPerLine{ 8 };
	static constexpr std::string_view HeaderSuffix{ ".hpp" };

	//TODO: param for array's data type, I guess the call to setw, too
	template <typename BackingT>
	void PrintAsset(std::ofstream& File, const std::vector<BackingT>& Asset, std::string_view BackingTypeName, int DigitsCount, std::string_view AssetName)
	{
		File << "inline constexpr std::array<" << BackingTypeName << ", " << Asset.size() << "> __attribute__((section(\".rodata\"), aligned(2))) " << AssetName << "\n";
		File << "{";

		auto OldFlags{ File.flags() };
		for (std::size_t i{ 0 }; i < Asset.size(); ++i)
		{
			if (i % EntriesPerLine == 0)
			{
				File << "\n\t";
			}
			File << std::showbase << std::internal << std::setfill('0') << std::setw(2 + DigitsCount) << std::hex << Asset[i] << ", ";
		}
		File << "\n};";
		File << "\n";
		File.flags(OldFlags);
	}

	void GeneratePaletteHeader(std::string_view PathPaletteName, std::string_view PaletteName, const std::vector<std::uint32_t>& PackedPalette);

	// array of array of assets
	void GenerateSpriteTileHeader(std::filesystem::path Directory, std::filesystem::path AssetPath, const std::vector<std::vector<std::uint32_t>>& PackedIndices, const SpritesheetDescription& Desc);

	void GenerateBackgroundHeader(std::filesystem::path Directory, std::filesystem::path AssetPath,
		const std::vector<std::uint32_t>& Tiles, const std::vector<TileMapEntry>& TileMap, int WidthTiles, int HeightTiles);
};
