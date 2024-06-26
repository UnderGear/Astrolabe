#pragma once

#include <bit>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <string_view>
#include <string>

#include "PaletteColor.hpp"
#include "SpritesheetDescription.hpp"

namespace Codegen
{
	// Utilities for asset index tracking
	inline constexpr auto AssetIndexingFileName{ "assets/AssetIndexing" };

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
		File << "inline constexpr std::array<std::uint32_t, " << Asset.size() << "> __attribute__((section(\".rodata\"), aligned(2))) " << AssetName << "\n";
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

	// array of array of assets
	void GenerateTileHeader(std::filesystem::path Directory, std::filesystem::path AssetPath, const std::vector<std::vector<std::uint32_t>>& PackedIndices, const SpritesheetDescription& Desc)
	{
		// includes section
		std::ofstream File;
		std::filesystem::path FileName{ (Directory/AssetPath).string() + ".hpp" };
		File.open(FileName);
		File << "// GENERATED CODE. DO NOT MANUALLY MODIFY THIS FILE.\n";
		File << "\n";
		File << "#pragma once\n";
		File << "\n";
		File << "#include <array>\n";
		File << "#include <cstdint>\n";
		File << "#include <span>\n";
		File << "\n";
		File << "#include \"Display/Sprite.hpp\"\n";
		File << "#include \"Display/SpriteManager.hpp\"\n";
		File << "\n";

		for (std::size_t i{ 0 }; i < PackedIndices.size(); ++i)
		{
			auto NameAtIndex{ AssetPath.string() + std::to_string(i) };
			PrintAsset(File, PackedIndices[i], NameAtIndex);
			File << "\n";
		}

		auto NextIndex{ GetNextAssetIndex() };

		File << "inline constexpr std::array<SpriteTileAsset, " << PackedIndices.size() << "> " << AssetPath.string() << "_tiles\n";
		File << "{\n";
		for (std::size_t i{ 0 }; i < PackedIndices.size(); ++i)
		{
			auto NameAtIndex{ AssetPath.string() + std::to_string(i) };
			File << "\tSpriteTileAsset{ std::span<const std::uint32_t>{ " << NameAtIndex << ".begin(), " << NameAtIndex << ".end() }, " << NextIndex++ << " },\n";
		}
		File << "};\n";

		File << "\n";
		File << "inline const std::array<Animation, " << Desc.TotalAnimationCount << "> " << AssetPath.string() << "_anims\n";
		File << "{\n";
		for (const auto& AnimDesc : Desc.AnimationSetDescriptions)
		{
			for (const auto& AnimSet : AnimDesc.AnimIndices)
			{
				for (std::size_t i{ 0 }; i < AnimDesc.AnimIndices.size(); i += AnimDesc.AnimFrameCount)
				{
					File << "\tAnimation{";

					for (auto j{ 0 }; j < AnimDesc.AnimFrameCount; ++j)
					{
						File << " { &" << AssetPath.string() << "_tiles[" << AnimSet[i + j] << "], " << AnimDesc.FrameDurations[j] << " },";
					}
					
					File << " },\n";
				}
			}
		}
		File << "};\n";

		File << "\n";
		File << "inline const AnimationSuite " << AssetPath.string() << "_animsuite\n";
		File << "{\n";
		File << "\tstd::span<const Animation>{ " << AssetPath.string() << "_anims.begin(), " << AssetPath.string() << "_anims.end() }\n";
		File << "};\n";

		WriteLastUsedAssetIndex(NextIndex);
	}
};
