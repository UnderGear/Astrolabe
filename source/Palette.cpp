#include <algorithm>

#include "Display/Palette.hpp"

PaletteManager::PaletteManager(void* PaletteMemoryAddress)
{
    Palette = std::unique_ptr<RawPaletteBanks>
    {
        new(PaletteMemoryAddress) RawPaletteBanks{ }
    };
};

//TODO: keep track of gaps on removal and the last-added one like Sprite?
std::int32_t PaletteManager::AddToPalette(const PaletteBankAsset& PaletteAsset)
{
    // If this asset is already loaded, increment its count and return the bank index
    auto FoundExisting{ std::ranges::find(PaletteBanks, PaletteAsset.ID, &PaletteBankData::AssetID) };
    if (FoundExisting != PaletteBanks.end())
    {
        FoundExisting->ReferenceCount++;
        return std::distance(PaletteBanks.begin(), FoundExisting);
    }

    // If there's space for our bank, add this ID with a count of 1, load it to the palette, and return the new index
    auto FoundEmpty{ std::ranges::find_if(PaletteBanks, [](const PaletteBankData& Data) { return !Data.IsValid(); }) };
    if (FoundEmpty != PaletteBanks.end())
    {
        *FoundEmpty = { 1, PaletteAsset.ID };
        auto EmptyIndex{ std::distance(PaletteBanks.begin(), FoundEmpty) };
        (*Palette)[EmptyIndex] = PaletteAsset.Data;
        return EmptyIndex;
    }

    // Otherwise we're all full
    return INDEX_INVALID;
}

void PaletteManager::RemoveFromPalette(std::int32_t Index)
{
    //TODO: bounds checking on that index
    auto& FoundBank{ PaletteBanks[Index] };
    FoundBank.ReferenceCount--;
    if (FoundBank.ReferenceCount <= 0)
    {
        FoundBank.AssetID = PaletteBankAsset::ID_INVALID;
        (*Palette)[Index] = {}; //TODO: what's the down side of leaving this in here and stomping it later?
    }
}
