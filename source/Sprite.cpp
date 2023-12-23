#include "Display/Display.hpp"
#include "Display/Sprite.hpp"
#include "Math/Point.hpp"

Sprite::Sprite(SpriteManager& InOwner, ObjectAttributes& InAttributes, std::vector<std::int32_t> InTileAssetIndices, std::int32_t InPaletteAssetIndex, std::int32_t InInitialFrame)
        : Owner(InOwner), Attributes(InAttributes), TileAssetIndices(InTileAssetIndices), PaletteAssetIndex(InPaletteAssetIndex), InitialFrame(InInitialFrame)
{
	//TODO: get size/shape params from the actual asset
    //TODO: pass in params. modes, intial position
	Attributes.Attribute0.ObjectMode = static_cast<std::uint16_t>(Attribute0ObjectMode::Normal);
	Attributes.Attribute0.GraphicsMode = static_cast<std::uint16_t>(Attribute0GraphicsMode::Normal);
	Attributes.Attribute0.SpriteShape = static_cast<std::uint16_t>(Attribute0SpriteShape::Square);
	Attributes.Attribute0.ColorMode = 1; //TODO: another param to pass in
	Attributes.Attribute1.Standard.SpriteSize = static_cast<std::uint16_t>(Attribute1SpriteSize::S32);
	Attributes.Attribute2.PaletteBank = PaletteAssetIndex;
	Attributes.Attribute2.TileIndex = TileAssetIndices[CurrentFrameIndex];

	auto [SpriteWidth, SpriteHeight]{ GetSpriteDimensions(Attribute0SpriteShape::Square, Attribute1SpriteSize::S32) };
	HalfWidth = SpriteWidth / 2;
	HalfHeight = SpriteHeight / 2;

	Attributes.Attribute1.Standard.XCoordinate = HalfWidth;
	Attributes.Attribute0.YCoordinate = HalfHeight;
}

Sprite::~Sprite()
{
    Owner.ReleaseOAM(Attributes);
	for (auto TileIndex : TileAssetIndices)
    	Owner.UnloadTiles(TileIndex);
    Owner.RemoveFromPalette(PaletteAssetIndex);
	//TODO: depending on how we loaded our palette, look to unload it. more bookkeeping in palette, I guess
}

void Sprite::SetPosition(const Point2D& Position)
{
	Attributes.Attribute0.YCoordinate = Position.Y - HalfWidth;
	Attributes.Attribute1.Standard.XCoordinate = Position.X - HalfHeight;
}

void Sprite::Tick(std::int32_t CurrentFrame)
{
	//TODO: support other animation tick types
	//TODO: maybe return a value indicating that the animation has finished.
	//TODO: member variable for progression frequency instead of hard-coded half second
	if ((CurrentFrame % 30) == 0)
	{
		++CurrentFrameIndex;
		CurrentFrameIndex %= TileAssetIndices.size();
		Attributes.Attribute2.TileIndex = TileAssetIndices[CurrentFrameIndex];
	}
}
