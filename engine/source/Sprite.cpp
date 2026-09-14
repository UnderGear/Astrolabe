#include "Display/Sprite.hpp"

#include <utility>

#include "Display/Display.hpp"
#include "Math/Point.hpp"

Sprite::Sprite(SpriteManager& InOwner, ObjectAttributes& InAttributes, AnimationSuite InAnimations, std::int32_t InPaletteAssetIndex, Attribute0Register::ObjectModeOptions ObjectMode, std::int32_t AffineOAMIndex)
		: Owner(&InOwner), Attributes(&InAttributes), Animations(InAnimations), PaletteAssetIndex(InPaletteAssetIndex)
{
	CurrentSpriteAsset = Animations[CurrentAnimationIndex][CurrentFrameIndex].Asset;
	auto LoadedTileIndex{ Owner->LoadTiles(*CurrentSpriteAsset) };
	assert(LoadedTileIndex != SpriteManager::INDEX_INVALID);

	auto [SpriteWidth, SpriteHeight]{ GetSpriteDimensions(Attribute0Register::SpriteShapeOptions::Square, Attribute1Register::SpriteSizeOptions::S32) };
	HalfWidth = SpriteWidth / 2;
	HalfHeight = SpriteHeight / 2;

	//TODO: get size/shape params from the actual asset
	//TODO: pass in params. modes, intial position

	Attributes->Attribute0.SetData(0, ObjectMode, Attribute0Register::GraphicsModeOptions::Normal, false, Attribute0Register::ColorModeOptions::WholePalette, Attribute0Register::SpriteShapeOptions::Square);

	//TODO: assert that these indices fit into their registers
	if (auto* AffineAttributes = Owner->GetAffineOAMByIndex(AffineOAMIndex))
	{
		Attributes->Attribute1.SetData(0, AffineOAMIndex, Attribute1Register::SpriteSizeOptions::S32);
		AffineAttributes->Pa = 1;
		AffineAttributes->Pb = 0;
		AffineAttributes->Pc = 0;
		AffineAttributes->Pd = 1;
	}
	else
	{	
		Attributes->Attribute1.SetData(0, false, false, Attribute1Register::SpriteSizeOptions::S32);
	}

	//TODO: make priority a param
	Attributes->Attribute2.SetData(LoadedTileIndex, Attribute2Register::PriorityOptions::VeryLow, PaletteAssetIndex != PaletteManager::INDEX_INVALID ? PaletteAssetIndex : 0);
}

Sprite::Sprite(Sprite&& MovedFrom)
	: Owner(std::exchange(MovedFrom.Owner, nullptr))
	, Attributes(std::exchange(MovedFrom.Attributes, nullptr))
	, Animations(MovedFrom.Animations)
	, CurrentSpriteAsset(std::exchange(MovedFrom.CurrentSpriteAsset, nullptr))
	, CurrentAnimationIndex(MovedFrom.CurrentAnimationIndex)
	, ShouldFlipHorizontal(MovedFrom.ShouldFlipHorizontal)
	, PaletteAssetIndex(MovedFrom.PaletteAssetIndex)
	, CurrentFrameIndex(MovedFrom.CurrentFrameIndex)
	, CurrentFrameCounter(MovedFrom.CurrentFrameCounter)
	, HalfWidth(MovedFrom.HalfWidth)
	, HalfHeight(MovedFrom.HalfHeight)
{
}

Sprite &Sprite::operator=(Sprite&& MovedFrom)
{
	Owner = std::exchange(MovedFrom.Owner, nullptr);
	Attributes = std::exchange(MovedFrom.Attributes, nullptr);
	Animations = MovedFrom.Animations;
	CurrentSpriteAsset = std::exchange(MovedFrom.CurrentSpriteAsset, nullptr);
	CurrentAnimationIndex = MovedFrom.CurrentAnimationIndex;
	ShouldFlipHorizontal = MovedFrom.ShouldFlipHorizontal;
	PaletteAssetIndex = MovedFrom.PaletteAssetIndex;
	CurrentFrameIndex = MovedFrom.CurrentFrameIndex;
	CurrentFrameCounter = MovedFrom.CurrentFrameCounter;
	HalfWidth = MovedFrom.HalfWidth;
	HalfHeight = MovedFrom.HalfHeight;
	return *this;
}

Sprite::~Sprite()
{
	if (Owner != nullptr && Attributes != nullptr)
	{
		Owner->ReleaseOAM(*Attributes);
		Owner->ReleaseAffineOAM(Attributes->Attribute1.Get<Attribute1Register::AffineIndex>());
		Owner->UnloadTiles(static_cast<std::int32_t>(Attributes->Attribute2.Get<Attribute2Register::TileIndex>()));
		Owner->RemoveFromPalette(PaletteAssetIndex);
	}
	//TODO: depending on how we loaded our palette, look to unload it. more bookkeeping in palette, I guess
	// note: I was thinking about palette banks vs full palette loads
}

void Sprite::SetPosition(const Point2D& Position)
{
	Attributes->Attribute0.Set<Attribute0Register::Y>(Position.Y - HalfHeight);
	Attributes->Attribute1.Set<Attribute1Register::X>(Position.X - HalfWidth);
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

	auto ObjectMode = Attributes->Attribute0.Get<Attribute0Register::ObjectMode>();
	if (ObjectMode == Attribute0Register::ObjectModeOptions::Normal)
	{
		Attributes->Attribute1.Set<Attribute1Register::HorizontalFlip>(ShouldFlipHorizontal);
	}
	else if (auto* Affine = Owner->GetAffineOAMByIndex(Attributes->Attribute1.Get<Attribute1Register::AffineIndex>()))
	{
		Affine->Pa = ShouldFlipHorizontal ? -1 : 1;
		Affine->Pd = 1;
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
		{
			CurrentFrameCounter = 0;
		}
	}

	auto* SpriteAsset = Animations[CurrentAnimationIndex][CurrentFrameIndex].Asset;
	if (SpriteAsset != CurrentSpriteAsset)
	{
		Owner->UnloadTiles(Attributes->Attribute2.Get<Attribute2Register::TileIndex>());

		CurrentSpriteAsset = SpriteAsset;

		auto LoadedTileIndex{ Owner->LoadTiles(*CurrentSpriteAsset) };
		assert(LoadedTileIndex != SpriteManager::INDEX_INVALID);
		Attributes->Attribute2.Set<Attribute2Register::TileIndex>(LoadedTileIndex);
	}
}
