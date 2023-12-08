#include "Display/Display.hpp"
#include "Display/Sprite.hpp"
#include "Point.hpp"

Sprite::Sprite(SpriteManager& InOwner, ObjectAttributes& InAttributes, std::int32_t InTileAssetIndex, std::int32_t InPaletteAssetIndex)
        : Owner(InOwner), Attributes(InAttributes), TileAssetIndex(InTileAssetIndex), PaletteAssetIndex(InPaletteAssetIndex)
{
    //TODO: pass in params. modes, shape, size, intial position?
	Attributes.Attribute0.YCoordinate = 32;
	Attributes.Attribute0.ObjectMode = static_cast<std::uint16_t>(Attribute0ObjectMode::Normal);
	Attributes.Attribute0.GraphicsMode = static_cast<std::uint16_t>(Attribute0GraphicsMode::Normal);
	Attributes.Attribute0.SpriteShape = static_cast<std::uint16_t>(Attribute0SpriteShape::Square);
	Attributes.Attribute1.Standard.SpriteSize = 0b11;
	Attributes.Attribute1.Standard.XCoordinate = 96;
	Attributes.Attribute2.PaletteBank = PaletteAssetIndex;
	Attributes.Attribute2.TileIndex = TileAssetIndex;
}

Sprite::~Sprite()
{
    Owner.ReleaseOAM(Attributes);
    Owner.UnloadTiles(TileAssetIndex);
    Owner.RemoveFromPalette(PaletteAssetIndex);
}

void Sprite::SetPosition(const Point2D& Position)
{
	Attributes.Attribute0.YCoordinate = Position.Y;
	Attributes.Attribute1.Standard.XCoordinate = Position.X;
}
