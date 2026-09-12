#include "Display/Background.hpp"
#include "Math/Vector.hpp"

Background::Background(
	BackgroundManager& InOwner,
	std::int32_t InBackgroundIndex,
	std::int32_t InMapBlockIndex,
	BackgroundControlRegister::RegularBackgroundDimensions InDimensions,
	volatile BackgroundControlRegister& InControlRegister,
	volatile BackgroundOffset& InOffset)
	: Owner(InOwner)
	, BackgroundIndex(InBackgroundIndex)
	, MapBlockIndex(InMapBlockIndex)
	, Dimensions(InDimensions)
	, ControlRegister(InControlRegister)
	, Offset(InOffset)
{
	ControlRegister.Set<BackgroundControlRegister::TileBlockBaseIndex>(static_cast<std::uint16_t>(BackgroundIndex));
	ControlRegister.Set<BackgroundControlRegister::TileMapBlockBaseIndex>(static_cast<std::uint16_t>(MapBlockIndex));
	ControlRegister.Set<BackgroundControlRegister::RegularBackgroundSize>(Dimensions);
	ControlRegister.Set<BackgroundControlRegister::ColorMode>(BackgroundControlRegister::ColorModeOptions::PaletteBank);
}

Background::~Background()
{
	Owner.UnloadTiles(BackgroundIndex);
	Owner.UnloadMap(BackgroundIndex);
	Owner.ClearPalette();
}

void Background::MoveOffset(const Vector2D& MoveAmount)
{
	ScreenOffset += MoveAmount;
	Offset.SetBatch<BackgroundOffset::X, BackgroundOffset::Y>(static_cast<std::int16_t>(ScreenOffset.X), static_cast<std::int16_t>(ScreenOffset.Y));
}

void Background::SetOffset(const Point2D& NewScreenOffset)
{
	ScreenOffset = NewScreenOffset;
	Offset.SetBatch<BackgroundOffset::X, BackgroundOffset::Y>(static_cast<std::int16_t>(ScreenOffset.X), static_cast<std::int16_t>(ScreenOffset.Y));
}

[[nodiscard]] std::pair<int, int> Background::GetDimensions() const
{
	switch (Dimensions)
	{
	case BackgroundControlRegister::RegularBackgroundDimensions::t32xt32:
		return { 32, 32 };
	case BackgroundControlRegister::RegularBackgroundDimensions::t32xt64:
		return { 32, 64 };
	case BackgroundControlRegister::RegularBackgroundDimensions::t64xt32:
		return { 64, 32 };
	default:
		return { 64, 64 };
	}
}
