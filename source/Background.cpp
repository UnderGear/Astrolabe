#include "Display/Background.hpp"

Background::Background(BackgroundManager& InOwner,
	std::int32_t InBackgroundIndex,
	std::int32_t InMapBlockIndex,
    std::int32_t InPaletteAssetIndex,
	RegularBackgroundDimensions InDimensions,
	BackgroundControlRegister& InControlRegister,
	BackgroundOffset& InOffset)
	: Owner(InOwner),
	BackgroundIndex(InBackgroundIndex),
	MapBlockIndex(InMapBlockIndex),
	PaletteAssetIndex(InPaletteAssetIndex),
	Dimensions(InDimensions),
	ControlRegister(InControlRegister),
	Offset(InOffset)
{
	ControlRegister.TileBlockBaseIndex = static_cast<std::uint16_t>(BackgroundIndex);
	ControlRegister.TileMapBlockBaseIndex = static_cast<std::uint16_t>(MapBlockIndex);
	ControlRegister.BackgroundSize = static_cast<std::uint16_t>(Dimensions);
}

Background::~Background()
{
	Owner.UnloadTiles(BackgroundIndex);
	Owner.UnloadMap(BackgroundIndex);
	Owner.RemoveFromPalette(PaletteAssetIndex);
}
