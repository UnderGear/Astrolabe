#include "Display/Display.hpp"
#include "Display/Sprite.hpp"
#include "Math/Point.hpp"

Sprite::Sprite(SpriteManager& InOwner, ObjectAttributes& InAttributes, const AnimationSuite& InAnimations, std::int32_t InPaletteAssetIndex, Attribute0ObjectMode ObjectMode, std::int32_t AffineOAMIndex)
        : Owner(InOwner), Attributes(InAttributes), Animations(InAnimations), PaletteAssetIndex(InPaletteAssetIndex)
{
	CurrentSpriteAsset = Animations[CurrentAnimationIndex][CurrentFrameIndex].Asset;
	auto LoadedTileIndex{ Owner.LoadTiles(*CurrentSpriteAsset) };
	assert(LoadedTileIndex != SpriteManager::INDEX_INVALID);

	//TODO: get size/shape params from the actual asset
    //TODO: pass in params. modes, intial position
	Attributes.Attribute0.ObjectMode = static_cast<std::uint16_t>(ObjectMode);
	Attributes.Attribute0.GraphicsMode = static_cast<std::uint16_t>(Attribute0GraphicsMode::Normal);
	Attributes.Attribute0.MosaicEnabled = 0;
	Attributes.Attribute0.SpriteShape = static_cast<std::uint16_t>(Attribute0SpriteShape::Square);
	Attributes.Attribute0.ColorMode = 1; //TODO: another param to pass in
	Attributes.Attribute1.Standard.SpriteSize = static_cast<std::uint16_t>(Attribute1SpriteSize::S32);

	//TODO: assert that these indices fit into their registers

	Attributes.Attribute2.Priority = 0; //TODO: add a parameter.

	if (PaletteAssetIndex != PaletteManager::INDEX_INVALID)
	{
		Attributes.Attribute2.PaletteBank = PaletteAssetIndex; //TODO: this only makes sense for 4bpp sprites
	}
	Attributes.Attribute2.TileIndex = LoadedTileIndex;

	auto [SpriteWidth, SpriteHeight]{ GetSpriteDimensions(Attribute0SpriteShape::Square, Attribute1SpriteSize::S32) };
	HalfWidth = SpriteWidth / 2;
	HalfHeight = SpriteHeight / 2;

	Attributes.Attribute1.Standard.XCoordinate = HalfWidth;
	Attributes.Attribute0.YCoordinate = HalfHeight;

	if (AffineOAMIndex != SpriteManager::INDEX_INVALID)
	{
		Attributes.Attribute1.Affine.AffineIndex = static_cast<std::int16_t>(AffineOAMIndex);
		auto* Affine = Owner.GetAffineOAMByIndex(Attributes.Attribute1.Affine.AffineIndex);
		if (Affine != nullptr)
		{
			Affine->Pa = 1;
			Affine->Pb = 0;
			Affine->Pc = 0;
			Affine->Pd = 1;
		}
	}
}

Sprite::~Sprite()
{
    Owner.ReleaseOAM(Attributes);
	Owner.ReleaseAffineOAM(Attributes.Attribute1.Affine.AffineIndex);
	Owner.UnloadTiles(Attributes.Attribute2.TileIndex);
    Owner.RemoveFromPalette(PaletteAssetIndex);
	//TODO: depending on how we loaded our palette, look to unload it. more bookkeeping in palette, I guess
	// note: I was thinking about palette banks vs full palette loads
}

void Sprite::SetPosition(const Point2D& Position)
{
	Attributes.Attribute0.YCoordinate = Position.Y - HalfWidth;
	Attributes.Attribute1.Standard.XCoordinate = Position.X - HalfHeight;
}

void Sprite::SetSpriteAnimationIndex(std::int32_t AnimationIndex)
{
	if (CurrentAnimationIndex == AnimationIndex)
	{
		return;
	}

	CurrentAnimationIndex = AnimationIndex;
	CurrentFrameIndex = 0;
	CurrentFrameCounter = 0;
}

void Sprite::SetShouldFlipHorizontal(bool InShouldFlipHorizontal)
{
	if (ShouldFlipHorizontal == InShouldFlipHorizontal)
	{
		return;
	}

	ShouldFlipHorizontal = InShouldFlipHorizontal;

	auto ObjectMode = static_cast<Attribute0ObjectMode>(Attributes.Attribute0.ObjectMode);
	if (ObjectMode == Attribute0ObjectMode::Normal)
	{
		Attributes.Attribute1.Standard.HorizontalFlip = static_cast<std::uint16_t>(InShouldFlipHorizontal);
	}
	else
	{
		auto* Affine = Owner.GetAffineOAMByIndex(Attributes.Attribute1.Affine.AffineIndex);
		if (Affine != nullptr)
		{
			Affine->Pa = ShouldFlipHorizontal ? -1 : 1;
			Affine->Pd = 1;
		}
	}
}

void Sprite::Tick()
{
	//TODO: support other animation tick types
	//TODO: maybe return a value indicating that the animation has finished.
	//TODO: member variable for progression frequency instead of hard-coded half second
	++CurrentFrameCounter;
	if ((CurrentFrameCounter % Animations[CurrentAnimationIndex][CurrentFrameIndex].FrameDuration) == 0)
	{
		++CurrentFrameIndex;
		auto Count{ Animations[CurrentAnimationIndex].size() };
		CurrentFrameIndex %= Count;

		// If we're back at the first frame, restart the counter
		if (CurrentFrameIndex == 0)
			CurrentFrameCounter = 0;
	}

	auto* SpriteAsset = Animations[CurrentAnimationIndex][CurrentFrameIndex].Asset;
	if (SpriteAsset != CurrentSpriteAsset)
	{
		Owner.UnloadTiles(Attributes.Attribute2.TileIndex);

		CurrentSpriteAsset = SpriteAsset;

		auto LoadedTileIndex{ Owner.LoadTiles(*CurrentSpriteAsset) };
		assert(LoadedTileIndex != SpriteManager::INDEX_INVALID);
		Attributes.Attribute2.TileIndex = LoadedTileIndex;
	}
}
