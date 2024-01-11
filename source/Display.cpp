#include "Display/Background.hpp"
#include "Display/Display.hpp"
#include "Display/DisplayRegisters.hpp"

Display::Display()
{
    // TODO: set these in the DisplayControl ctor?
    DisplayControl->OBJCharacterVRAMMapping = 1; // 1D object mode
    DisplayControl->BackgroundFlags = static_cast<std::uint16_t>(BackgroundLayerFlags::Object)
        | static_cast<std::uint16_t>(BackgroundLayerFlags::Background0);
}

Sprite Display::LoadSprite(std::span<Animation> Animations, const PaletteBankAsset& PaletteAsset, std::int32_t CurrentFrame)
{
    auto* OAM{ Sprites.RequestOAM() };
    assert(OAM != nullptr);

    auto LoadedPaletteIndex{ Sprites.AddToPalette(PaletteAsset) };
    assert(LoadedPaletteIndex != PaletteManager::INDEX_INVALID);

    return Sprite{ Sprites, *OAM, Animations, LoadedPaletteIndex, CurrentFrame };
}

Sprite Display::LoadSprite(std::span<Animation> Animations, const PaletteAsset& PaletteAsset, std::int32_t CurrentFrame)
{
    auto* OAM{ Sprites.RequestOAM() };
    assert(OAM != nullptr);

    Sprites.SetPalette(PaletteAsset);

    return Sprite{ Sprites, *OAM, Animations, PaletteManager::INDEX_INVALID, CurrentFrame };
}

Background Display::LoadBackground(const BackgroundTileAsset& BackgroundAsset, const PaletteAsset& PaletteAsset, const BackgroundMapAsset& MapAsset)
{
    auto BackgroundIndex{ Backgrounds.LoadTiles(BackgroundAsset) };
    auto TileMapBaseIndex{ Backgrounds.LoadMap(MapAsset, BackgroundIndex) };
    auto& ControlRegister{ Backgrounds.GetControlRegister(BackgroundIndex) };
    auto& Offset{ Backgrounds.GetBackgroundOffset(BackgroundIndex) };
    Backgrounds.SetPalette(PaletteAsset);
    return Background{ Backgrounds, BackgroundIndex, TileMapBaseIndex, BackgroundAsset.Dimensions, ControlRegister, Offset };
}
