#pragma once

#include <bit>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <string_view>
#include <string>

#include "PaletteColor.hpp"

namespace Codegen
{
	// Utilities for asset index tracking
	inline constexpr auto AssetIndexingFileName{ "AssetIndexing" };

	int GetNextAssetIndex()
	{
		int Index{ 0 };
		std::ifstream File{ AssetIndexingFileName, std::ios::binary };
		File >> Index;
		return Index;
	}

	void WriteLastUsedAssetIndex(int Index)
	{
		std::ofstream File{ AssetIndexingFileName, std::ios::binary };
		File << Index;
	}

	static constexpr auto EntriesPerLine{ 8 };
	static constexpr std::string_view HeaderSuffix{ ".hpp" };

	void PrintAsset(std::ofstream& File, const std::vector<std::uint32_t>& Asset, std::string_view AssetName)
	{
		File << "inline constexpr std::array<std::uint32_t, " << Asset.size() << "> " << AssetName << "\n";
		File << "{";

		auto OldFlags{ File.flags() };
		for (std::size_t i{ 0 }; i < Asset.size(); ++i)
		{
			if (i % EntriesPerLine == 0)
			{
				File << "\n\t";
			}
			File << std::showbase << std::internal << std::setfill('0') << std::setw(10) << std::hex << Asset[i] << ", ";
		}
		File << "\n};";
		File << "\n";
		File.flags(OldFlags);
	}

	void GeneratePaletteHeader(std::string_view PathPaletteName, std::string_view PaletteName, const std::vector<std::uint32_t>& PackedPalette)
	{
		std::ofstream File;
		std::filesystem::path FileName{ std::string{ PathPaletteName } + std::string{ HeaderSuffix } };
		File.open(FileName);
		File << "// GENERATED CODE. DO NOT MANUALLY MODIFY THIS FILE.\n";
		File << "\n";
		File << "#pragma once\n";
		File << "\n";
		File << "#include <array>\n";
		File << "#include <cstdint>\n";
		File << "#include <span>\n";
		File << "\n";
		File << "#include \"Display/Palette.hpp\"\n";
		File << "\n";

		auto RawName{ std::string{ PaletteName } + "_raw" };
		PrintAsset(File, PackedPalette, RawName);

		auto NextIndex{ GetNextAssetIndex() };
		
		File << "\n";
		File << "inline constexpr PaletteAsset " << PaletteName << "\n";
		File << "{\n";
		File << "\tstd::span<const std::uint32_t>{ " << RawName << ".begin(), " << RawName << ".end() }, " << NextIndex << "\n";
		File << "};\n";

		WriteLastUsedAssetIndex(NextIndex);
	}

	void GenerateTileHeader(std::string_view AssetPathName, std::string_view AssetName, const std::vector<std::vector<std::uint32_t>>& PackedIndices)
	{
		// includes section
		std::ofstream File;
		std::filesystem::path FileName{ std::string{ AssetPathName } + std::string{ HeaderSuffix } };
		File.open(FileName);
		File << "// GENERATED CODE. DO NOT MANUALLY MODIFY THIS FILE.\n";
		File << "\n";
		File << "#pragma once\n";
		File << "\n";
		File << "#include <array>\n";
		File << "#include <cstdint>\n";
		File << "#include <span>\n";
		File << "\n";
		File << "#include \"Display/SpriteManager.hpp\"\n";
		File << "\n";

		for (std::size_t i{ 0 }; i < PackedIndices.size(); ++i)
		{
			auto NameAtIndex{ std::string{ AssetName } + std::to_string(i) };
			PrintAsset(File, PackedIndices[i], NameAtIndex);
			File << "\n";
		}

		auto NextIndex{ GetNextAssetIndex() };

		File << "\n";
		File << "inline constexpr std::array<SpriteTileAsset, " << PackedIndices.size() << "> " << AssetName << "\n";
		File << "{\n";
		for (std::size_t i{ 0 }; i < PackedIndices.size(); ++i)
		{
			auto NameAtIndex{ std::string{ AssetName } + std::to_string(i) };
			File << "\tSpriteTileAsset{ std::span<const std::uint32_t>{ " << NameAtIndex << ".begin(), " << NameAtIndex << ".end() }, " << NextIndex++ << " },\n";
		}
		File << "};\n";

		WriteLastUsedAssetIndex(NextIndex);
	}
};
