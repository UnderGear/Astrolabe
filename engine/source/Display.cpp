#include "Display/Background.hpp"
#include "Display/Display.hpp"
#include "Display/DisplayRegisters.hpp"
#include "Hardware/Interrupt.hpp"

Display::Display()
{
	// TODO: set these in the DisplayControl ctor?
	DisplayControl->Set<DisplayControlRegister::OBJCharacterVRAMMapping>(DisplayControlRegister::OBJCharacterVRAMMappingOptions::Mapping1D);
	DisplayControl->Set<DisplayControlRegister::Background0>(true);
	DisplayControl->Set<DisplayControlRegister::ObjectLayer>(true);

	// request vblank interrupts
	DisplayStatus->Set<DisplayStatusRegister::VBlankInterruptRequest>(true);
	// enable the vblank interrupt
	Interrupts::EnableInterrupt(Interrupts::InterruptType::VBlank);
	Interrupts::AddHandler(Interrupts::InterruptType::VBlank, OnVBlank);
}

Sprite Display::LoadSprite(const AnimationSuite& Animations, const PaletteBankAsset& PaletteAsset, Attribute0Register::ObjectModeOptions ObjectMode)
{
	auto* OAM{ Sprites.RequestOAM() };
	assert(OAM != nullptr);

	auto LoadedPaletteIndex{ Sprites.AddToPalette(PaletteAsset) };
	assert(LoadedPaletteIndex != PaletteManager::INDEX_INVALID);

	std::int32_t AffineOAMIndex = SpriteManager::INDEX_INVALID;
	if (ObjectMode == Attribute0Register::ObjectModeOptions::Affine)
	{
		AffineOAMIndex = Sprites.RequestAffineOAM();
		assert(AffineOAMIndex != SpriteManager::INDEX_INVALID);
	}

	return Sprite{ Sprites, *OAM, Animations, LoadedPaletteIndex, ObjectMode, AffineOAMIndex };
}

Sprite Display::LoadSprite(const AnimationSuite& Animations, const PaletteAsset& PaletteAsset, Attribute0Register::ObjectModeOptions ObjectMode)
{
	auto* OAM{ Sprites.RequestOAM() };
	assert(OAM != nullptr);

	Sprites.SetPalette(PaletteAsset);

	std::int32_t AffineOAMIndex = SpriteManager::INDEX_INVALID;
	if (ObjectMode == Attribute0Register::ObjectModeOptions::Affine)
	{
		AffineOAMIndex = Sprites.RequestAffineOAM();
		assert(AffineOAMIndex != SpriteManager::INDEX_INVALID);
	}

	return Sprite{ Sprites, *OAM, Animations, PaletteManager::INDEX_INVALID, ObjectMode, AffineOAMIndex };
}

//TODO: move to level manager
Background Display::LoadBackground(const BackgroundTileAsset& BackgroundAsset, const PaletteAsset& PaletteAsset, const BackgroundMapAsset& MapAsset)
{
	auto BackgroundIndex{ Backgrounds.LoadTiles(BackgroundAsset) };
	auto TileMapBaseIndex{ Backgrounds.LoadMap(MapAsset, BackgroundIndex) };
	auto& ControlRegister{ Backgrounds.GetControlRegister(BackgroundIndex) };
	auto& Offset{ Backgrounds.GetBackgroundOffset(BackgroundIndex) };
	Backgrounds.SetPalette(PaletteAsset);
	return Background{ Backgrounds, BackgroundIndex, TileMapBaseIndex, BackgroundAsset.Dimensions, ControlRegister, Offset };
}
