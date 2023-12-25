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
	static constexpr auto EntriesPerLine{ 8 };
	static constexpr std::string_view HeaderSuffix{ ".hpp" };

	void PrintAsset(std::ofstream& File, const std::vector<std::uint32_t>& Asset, std::string_view AssetName)
	{
		File << "inline constexpr std::array<std::uint32_t, " << Asset.size() << "> " << AssetName << "\n";
		File << "{";

		auto OldFlags{ File.flags() };
		for (int i{ 0 }; i < Asset.size(); ++i)
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

	void GeneratePaletteHeader(std::string_view PaletteName, const std::vector<std::uint32_t>& PackedPalette)
	{
		std::ofstream File;
		std::filesystem::path FileName{ std::string{ PaletteName } + std::string{ HeaderSuffix } };
		File.open(FileName);
		File << "// GENERATED CODE. DO NOT MANUALLY MODIFY THIS FILE.\n";
		File << "\n";
		File << "#pragma once\n";
		File << "\n";
		File << "#include <array>\n";
		File << "#include <cstdint>\n";
		File << "\n";

		PrintAsset(File, PackedPalette, PaletteName);
	}

	void GenerateTileHeader(std::string_view AssetName, const std::vector<std::vector<std::uint32_t>>& PackedIndices)
	{
		// includes section
		std::ofstream File;
		std::filesystem::path FileName{ std::string{ AssetName } + std::string{ HeaderSuffix } };
		File.open(FileName);
		File << "// GENERATED CODE. DO NOT MANUALLY MODIFY THIS FILE.\n";
		File << "\n";
		File << "#pragma once\n";
		File << "\n";
		File << "#include <array>\n";
		File << "#include <cstdint>\n";
		File << "\n";

		for (int i{ 0 }; i < PackedIndices.size(); ++i)
		{
			auto NameAtIndex{ std::string{ AssetName } + std::to_string(i) };
			PrintAsset(File, PackedIndices[i], NameAtIndex);
			File << "\n";
		}
	}
};
