#include "Display/Display.hpp"
#include "Display/Sprite.hpp"

Display::Display()
{
    // TODO: set these in the DisplayControl ctor?
    DisplayControl->OBJCharacterVRAMMapping = 1; // 1D object mode
    DisplayControl->BackgroundFlags = static_cast<std::uint16_t>(BackgroundLayerFlags::Object);
}

Sprite Display::LoadSprite(const SpriteTileAsset& TileAsset, const PaletteBankAsset& PaletteAsset)
{
    auto* OAM{ Sprites.RequestOAM() };
    assert(OAM != nullptr);
    auto LoadedTileIndex{ Sprites.LoadTiles(TileAsset) };
    assert(LoadedTileIndex != SpriteManager::INDEX_INVALID);
    auto LoadedPaletteIndex{ Sprites.AddToPalette(PaletteAsset) };
    assert(LoadedPaletteIndex != PaletteManager::INDEX_INVALID);

    return Sprite{ Sprites, *OAM, LoadedTileIndex, LoadedPaletteIndex };
}
