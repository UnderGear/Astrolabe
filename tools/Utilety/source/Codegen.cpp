#include "Codegen.hpp"

#include <iostream>

int Codegen::GetNextAssetIndex()
{
	int Index{ 0 };
	std::ifstream File{ AssetIndexingFileName, std::ios::binary };
	File >> Index;
	return Index;
}

void Codegen::WriteLastUsedAssetIndex(int Index)
{
	std::ofstream File{ AssetIndexingFileName, std::ios::binary };
	File << Index;
}

void Codegen::GenerateFolders()
{
	std::filesystem::create_directory(std::filesystem::path{ CodegenHeaderPath } / std::filesystem::path{ CodegenHeaderSubPath });
	std::filesystem::create_directory(std::filesystem::path{ CodegenSourcePath });
}

void Codegen::GeneratePaletteSource(const std::string &PaletteName, const std::vector<std::uint32_t> &PackedPalette)
{
	std::ofstream File;
	std::filesystem::path HeaderFileName{ std::string{ CodegenHeaderSubPath } + PaletteName + std::string{ HeaderSuffix } };
	std::filesystem::path HeaderFilePath{ std::string{ CodegenHeaderPath } + HeaderFileName.string() };
	File.open(HeaderFilePath);
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

	auto RawName{ PaletteName + "_raw" };
	PrintAssetDeclaration(File, PackedPalette, "std::uint32_t", RawName);

	auto NextIndex{ GetNextAssetIndex() };
	
	File << "\n";
	File << "static constexpr PaletteAsset " << PaletteName << "\n";
	File << "{\n";
	File << "\tstd::span<const std::uint32_t>{ " << RawName << ".begin(), " << RawName << ".end() }, " << NextIndex << "\n";
	File << "};\n";
	File.close();
	File.clear();

	std::filesystem::path SourceFileName{ std::string{ CodegenSourcePath } + PaletteName + std::string{ SourceSuffix } };
	File.open(SourceFileName);
	File << "// GENERATED CODE. DO NOT MANUALLY MODIFY THIS FILE.\n";
	File << "\n";
	File << "#include " << HeaderFileName << "\n";
	File << "\n";
	PrintAssetDefinition(File, PackedPalette, "std::uint32_t", 8, RawName);
	File << "\n";
	File.close();
	File.clear();

	WriteLastUsedAssetIndex(NextIndex);
}

void Codegen::GenerateSpriteTileSource(const std::string& SpriteName, const std::vector<std::vector<std::uint32_t>>& PackedIndices, const SpritesheetDescription& Desc)
{
	// includes section
	std::ofstream File;
	std::filesystem::path HeaderFileName{ std::string{ CodegenHeaderSubPath } + SpriteName + std::string{ HeaderSuffix } };
	std::filesystem::path HeaderFilePath{ std::string{ CodegenHeaderPath } + HeaderFileName.string() };
	File.open(HeaderFilePath);
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
		auto NameAtIndex{ SpriteName + std::to_string(i) };
		PrintAssetDeclaration(File, PackedIndices[i], "std::uint32_t", NameAtIndex);
	}

	auto NextIndex{ GetNextAssetIndex() };

	File << "\n";
	File << "extern const std::array<SpriteTileAsset, " << PackedIndices.size() << "> " << SpriteName << "_tiles;\n";
	File << "\n";
	File << "extern const std::array<Animation, " << Desc.TotalAnimationCount << "> " << SpriteName << "_anims;\n";
	File << "\n";
	File << "static constexpr AnimationSuite " << SpriteName << "_animsuite\n";
	File << "{\n";
	File << "\tstd::span<const Animation>{ " << SpriteName << "_anims.begin(), " << SpriteName << "_anims.end() }\n";
	File << "};\n";
	File.close();
	File.clear();

	// Source file
	std::filesystem::path SourceFileName{ std::string{ CodegenSourcePath } + SpriteName + std::string{ SourceSuffix } };
	File.open(SourceFileName);
	File << "// GENERATED CODE. DO NOT MANUALLY MODIFY THIS FILE.\n";
	File << "\n";
	File << "#include " << HeaderFileName << "\n";
	File << "\n";

	for (std::size_t i{ 0 }; i < PackedIndices.size(); ++i)
	{
		auto NameAtIndex{ SpriteName + std::to_string(i) };
		PrintAssetDefinition(File, PackedIndices[i], "std::uint32_t", 8, NameAtIndex);
		File << "\n";
	}

	File << "extern const std::array<SpriteTileAsset, " << PackedIndices.size() << "> " << SpriteName << "_tiles\n";
	File << "{\n";
	for (std::size_t i{ 0 }; i < PackedIndices.size(); ++i)
	{
		auto NameAtIndex{ SpriteName + std::to_string(i) };
		File << "\tSpriteTileAsset{ std::span<const std::uint32_t>{ " << NameAtIndex << ".begin(), " << NameAtIndex << ".end() }, " << NextIndex++ << " },\n";
	}
	File << "};\n";

	File << "\n";
	File << "extern const std::array<Animation, " << Desc.TotalAnimationCount << "> " << SpriteName << "_anims\n";
	File << "{\n";
	for (const auto& AnimDesc : Desc.AnimationSetDescriptions)
	{
		for (const auto& AnimSet : AnimDesc.AnimIndices)
		{
			//std::cout << "set count: " << AnimSet.size() << std::endl;
			for (std::size_t i{ 0 }; i < AnimSet.size(); i += AnimDesc.AnimFrameCount)
			{
				File << "\tAnimation{";

				for (auto j{ 0 }; j < AnimDesc.AnimFrameCount; ++j)
				{
					//std::cout << "i: " << i << ", j: " << j << ", tiles: " << AnimSet[i + j] << std::endl;
					File << " { &" << SpriteName << "_tiles[" << AnimSet[i + j] << "], " << AnimDesc.FrameDurations[j] << " },";
				}
				
				File << " },\n";
			}
		}
	}
	File << "};\n";

	File.close();
	File.clear();

	WriteLastUsedAssetIndex(NextIndex);
}

void Codegen::GenerateBackgroundSource(const std::string& BackgroundName, const std::vector<std::uint32_t>& Tiles, const std::vector<TileMapEntry>& TileMap, int WidthTiles, int HeightTiles)
{
	// includes section
	std::ofstream File;
	std::filesystem::path HeaderFileName{ std::string{ CodegenHeaderSubPath } + BackgroundName + std::string{ HeaderSuffix } };
	std::filesystem::path HeaderFilePath{ std::string{ CodegenHeaderPath } + HeaderFileName.string() };
	File.open(HeaderFilePath);
	File << "// GENERATED CODE. DO NOT MANUALLY MODIFY THIS FILE.\n";
	File << "\n";
	File << "#pragma once\n";
	File << "\n";
	File << "#include <array>\n";
	File << "#include <cstdint>\n";
	File << "#include <span>\n";
	File << "\n";
	File << "#include \"Display/Background.hpp\"\n";
	File << "#include \"Display/BackgroundManager.hpp\"\n";
	File << "\n";

	PrintAssetDeclaration(File, Tiles, "std::uint32_t", BackgroundName + "_tiles_raw");
	File << "\n";

	std::vector<std::uint16_t> TransformedTileMap;
	TransformedTileMap.reserve(TileMap.size());

	std::ranges::transform(TileMap, std::back_inserter(TransformedTileMap), [](const TileMapEntry& Entry)
	{
		return std::bit_cast<std::uint16_t>(Entry);
	});

	PrintAssetDeclaration(File, TransformedTileMap, "std::uint16_t", BackgroundName + "_map_raw");
	File << "\n";

	auto TileIndex{ GetNextAssetIndex() };
	File << "inline constexpr BackgroundTileAsset " << BackgroundName << "_tiles\n";
	File << "{\n";
	File << "\tstd::span<const std::uint32_t>(" << BackgroundName << "_tiles_raw.begin(), " << BackgroundName << "_tiles_raw.end()),\n";
	File << "\t" << TileIndex << ",\n";
	File << "\t" << "BackgroundControlRegister::RegularBackgroundDimensions::t" << WidthTiles << "xt" << HeightTiles << "\n";
	File << "};\n";
	File << "\n";
	WriteLastUsedAssetIndex(TileIndex);

	auto MapIndex{ GetNextAssetIndex() };
	File << "inline constexpr BackgroundMapAsset " << BackgroundName << "_map\n";
	File << "{\n";
	File << "\tstd::span<const std::uint16_t>(" << BackgroundName << "_map_raw.begin(), " << BackgroundName << "_map_raw.end()),\n";
	File << "\t" << TileIndex << "\n";
	File << "};\n";
	File.close();
	File.clear();

	std::filesystem::path SourceFileName{ std::string{ CodegenSourcePath } + BackgroundName + std::string{ SourceSuffix } };
	File.open(SourceFileName);
	File << "// GENERATED CODE. DO NOT MANUALLY MODIFY THIS FILE.\n";
	File << "\n";
	File << "#include " << HeaderFileName << "\n";
	File << "\n";

	PrintAssetDefinition(File, Tiles, "std::uint32_t", 8, BackgroundName + "_tiles_raw");
	File << "\n";
	PrintAssetDefinition(File, TransformedTileMap, "std::uint16_t", 4, BackgroundName + "_map_raw");
	File.close();
	File.clear();

	WriteLastUsedAssetIndex(MapIndex);
}
