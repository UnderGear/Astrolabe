#include <algorithm>

#include "Display/BackgroundManager.hpp"

void BackgroundManager::SetPalette(const PaletteAsset& ToSet)
{
	BackgroundPaletteManager.SetPalette(ToSet);
}

void BackgroundManager::ClearPalette()
{
	BackgroundPaletteManager.ClearPalette();
}

std::int32_t BackgroundManager::AddToPalette(const PaletteBankAsset& PaletteAsset)
{
	return BackgroundPaletteManager.AddToPalette(PaletteAsset);
}

void BackgroundManager::RemoveFromPalette(std::int32_t Index)
{
	BackgroundPaletteManager.RemoveFromPalette(Index);
}

std::int32_t BackgroundManager::LoadTiles(const BackgroundTileAsset& ToAdd)
{
	//TODO: check if the asset is already loaded elsewhere?
	auto EmptyBlockEntry{ std::ranges::find(LoadedTileBlocks, true, &BackgroundTileBlockData::IsAvailable) };

	if (EmptyBlockEntry == LoadedTileBlocks.end())
		return INDEX_INVALID;

	// Calculate the index of our tile block from the iterator
	auto Index{ std::distance(LoadedTileBlocks.begin(), EmptyBlockEntry) };
	// Copy the data into the tile block at the index
	auto [LastIterator, Next] { std::ranges::copy(ToAdd.Data, TileBlocks[Index].begin()) };

	EmptyBlockEntry->AssetID = ToAdd.ID;
	EmptyBlockEntry->NextAvailable = Next;
	return Index;
}

void BackgroundManager::UnloadTiles(std::int32_t Index)
{
	auto& ToClear{ LoadedTileBlocks[Index] };
	auto& Block{ TileBlocks[Index] };
	ToClear.AssetID = BackgroundTileAsset::ID_INVALID;
	ToClear.NextAvailable = Block.begin();
	Block.fill(0); //TODO: this is also stomping map data. maybe this isn't the right move. is it even worth doing?
}

// Returns the base tile map block index
//TODO: I guess we'd do animations by modifying the map at runtime?
std::int32_t BackgroundManager::LoadMap(const BackgroundMapAsset& ToAdd, std::int32_t TileBlockIndex)
{
	//TODO: we should proooobably do some bounds checking with this information
	//const auto& TileBlockData{ LoadedTileBlocks[TileBlockIndex] };
	//auto TileBlockEntryCount{ std::distance((*TileBlocks)[TileBlockIndex].begin(), TileBlockData.NextAvailable) };
	// these are backed by uint32

	auto ToAddCount{ std::distance(ToAdd.Data.begin(), ToAdd.Data.end()) };
	auto TileMapEntryBlocksRequired{ ((ToAddCount - 1) / TileMapEntriesPerBlock) + 1 };
	// and these are backed by uint16 (for now)

	//TODO: magic number.
	auto StartIndex{ 8 * TileBlockIndex + 8 - TileMapEntryBlocksRequired };

	// note that we HAVE to start on our tile map entry block beginning
	auto Start{ std::ranges::next(TileMapEntries.begin(), StartIndex * TileMapEntriesPerBlock) };
	std::ranges::copy(ToAdd.Data, Start);

	auto& TileMapData{ LoadedTileMaps[TileBlockIndex] };
	TileMapData.AssetID = ToAdd.ID;
	TileMapData.BeginIndex = StartIndex;

	return StartIndex;
}

void BackgroundManager::UnloadMap(std::int32_t TileBlockIndex)
{
	auto& TileMapData{ LoadedTileMaps[TileBlockIndex] };
	TileMapData.AssetID = BackgroundTileAsset::ID_INVALID;
	TileMapData.BeginIndex = 0;
	//TODO: should we even bother clearing out the VRAM?
}

volatile BackgroundControlRegister& BackgroundManager::GetControlRegister(std::int32_t BackgroundIndex)
{
	//TODO: bounds checking
	return ControlRegisters[BackgroundIndex];
}

volatile BackgroundOffset& BackgroundManager::GetBackgroundOffset(std::int32_t BackgroundIndex)
{
	//TODO: bounds checking
	return OffsetRegisters[BackgroundIndex];
}
