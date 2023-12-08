#include "Display/Display.hpp"
#include "Display/Sprite.hpp"

Display::Display()
{
    // TODO: set these in the DisplayControl ctor?
    DisplayControl->OBJCharacterVRAMMapping = 1; // 1D object mode
    DisplayControl->BackgroundFlags = static_cast<std::uint16_t>(BackgroundLayerFlags::Object);
}

Sprite Display::LoadSprite(std::vector<const SpriteTileAsset*> TileAssets, const PaletteBankAsset& PaletteAsset, std::int32_t CurrentFrame)
{
    auto* OAM{ Sprites.RequestOAM() };
    assert(OAM != nullptr);
    std::vector<std::int32_t> LoadedTileIndices;
    for (const auto* TileAsset : TileAssets)
    {
        auto LoadedTileIndex{ Sprites.LoadTiles(*TileAsset) };
        assert(LoadedTileIndex != SpriteManager::INDEX_INVALID);
        LoadedTileIndices.push_back(LoadedTileIndex);
    }
    auto LoadedPaletteIndex{ Sprites.AddToPalette(PaletteAsset) };
    assert(LoadedPaletteIndex != PaletteManager::INDEX_INVALID);

    return Sprite{ Sprites, *OAM, LoadedTileIndices, LoadedPaletteIndex, CurrentFrame };
}
