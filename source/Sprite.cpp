#include "Display/Display.hpp"
#include "Display/Sprite.hpp"
#include "Point.hpp"

Sprite::Sprite(SpriteManager& InOwner, ObjectAttributes& InAttributes, std::vector<std::int32_t> InTileAssetIndices, std::int32_t InPaletteAssetIndex, std::int32_t InInitialFrame)
        : Owner(InOwner), Attributes(InAttributes), TileAssetIndices(InTileAssetIndices), PaletteAssetIndex(InPaletteAssetIndex), InitialFrame(InInitialFrame)
{
    //TODO: pass in params. modes, shape, size, intial position?
	Attributes.Attribute0.YCoordinate = 32;
	Attributes.Attribute0.ObjectMode = static_cast<std::uint16_t>(Attribute0ObjectMode::Normal);
	Attributes.Attribute0.GraphicsMode = static_cast<std::uint16_t>(Attribute0GraphicsMode::Normal);
	Attributes.Attribute0.SpriteShape = static_cast<std::uint16_t>(Attribute0SpriteShape::Square);
	Attributes.Attribute1.Standard.SpriteSize = 0b11;
	Attributes.Attribute1.Standard.XCoordinate = 96;
	Attributes.Attribute2.PaletteBank = PaletteAssetIndex;
	Attributes.Attribute2.TileIndex = TileAssetIndices[CurrentFrameIndex];
}

Sprite::~Sprite()
{
    Owner.ReleaseOAM(Attributes);
	for (auto TileIndex : TileAssetIndices)
    	Owner.UnloadTiles(TileIndex);
    Owner.RemoveFromPalette(PaletteAssetIndex);
}

void Sprite::SetPosition(const Point2D& Position)
{
	Attributes.Attribute0.YCoordinate = Position.Y;
	Attributes.Attribute1.Standard.XCoordinate = Position.X;
}

void Sprite::Tick(std::int32_t CurrentFrame)
{
	//TODO: support other tick types
	//TODO: maybe return a value indicating that the animation has finished.
	//TODO: member variable for progression frequency instead of hard-coded half second
	if ((CurrentFrame % 30) == 0)
	{
		++CurrentFrameIndex;
		CurrentFrameIndex %= TileAssetIndices.size();
		Attributes.Attribute2.TileIndex = TileAssetIndices[CurrentFrameIndex];
	}
}
