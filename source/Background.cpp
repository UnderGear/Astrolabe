#include "Display/Background.hpp"
#include "Math/Vector.hpp"

Background::Background(
	BackgroundManager& InOwner,
	std::int32_t InBackgroundIndex,
	std::int32_t InMapBlockIndex,
	RegularBackgroundDimensions InDimensions,
	volatile BackgroundControlRegister& InControlRegister,
	volatile BackgroundOffset& InOffset)
	: Owner(InOwner),
	BackgroundIndex(InBackgroundIndex),
	MapBlockIndex(InMapBlockIndex),
	Dimensions(InDimensions),
	ControlRegister(InControlRegister),
	Offset(InOffset)
{
	ControlRegister.TileBlockBaseIndex = static_cast<std::uint16_t>(BackgroundIndex);
	ControlRegister.TileMapBlockBaseIndex = static_cast<std::uint16_t>(MapBlockIndex);
	ControlRegister.BackgroundSize = static_cast<std::uint16_t>(Dimensions);
	ControlRegister.ColorMode = 0;
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
	Offset.X = static_cast<std::uint32_t>(ScreenOffset.X);
	Offset.Y = static_cast<std::uint32_t>(ScreenOffset.Y);
}

void Background::SetOffset(const Point2D& NewScreenOffset)
{
	ScreenOffset = NewScreenOffset;
	Offset.X = static_cast<std::uint32_t>(ScreenOffset.X);
	Offset.Y = static_cast<std::uint32_t>(ScreenOffset.Y);
}

std::pair<int, int> Background::GetDimensions() const
{
	switch (Dimensions)
	{
	case RegularBackgroundDimensions::t32xt32:
		return { 32, 32 };
	case RegularBackgroundDimensions::t32xt64:
		return { 32, 64 };
	case RegularBackgroundDimensions::t64xt32:
		return { 64, 32 };
	case RegularBackgroundDimensions::t64xt64:
		return { 64, 64 };
	}
}
