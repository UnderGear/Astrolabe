#include "Display/BackgroundManager.hpp"

std::int32_t BackgroundManager::AddToPalette(const PaletteBankAsset& PaletteAsset)
{
	return BackgroundPaletteManager.AddToPalette(PaletteAsset);
}

void BackgroundManager::RemoveFromPalette(std::int32_t Index)
{
	BackgroundPaletteManager.RemoveFromPalette(Index);
}
