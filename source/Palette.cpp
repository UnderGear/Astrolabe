#include <algorithm>

#include "Display/Palette.hpp"

PaletteManager::PaletteManager(void* PaletteMemoryAddress)
{
    PaletteBanks = reinterpret_cast<RawPaletteBanks*>(PaletteMemoryAddress);
    Palette = reinterpret_cast<RawPalette*>(PaletteMemoryAddress);
};

void PaletteManager::SetPalette(const PaletteAsset& ToSet)
{
    std::ranges::copy(ToSet.Data, Palette->begin());
}

void PaletteManager::ClearPalette()
{
    std::ranges::fill(*Palette, 0);
}

//TODO: keep track of gaps on removal and the last-added one like Sprite?
std::int32_t PaletteManager::AddToPalette(const PaletteBankAsset& ToAdd)
{
    // If this asset is already loaded, increment its count and return the bank index
    auto FoundExisting{ std::ranges::find(PaletteBankData, ToAdd.ID, &PaletteBankDatum::AssetID) };
    if (FoundExisting != PaletteBankData.end())
    {
        FoundExisting->ReferenceCount++;
        return std::distance(PaletteBankData.begin(), FoundExisting);
    }

    // If there's space for our bank, add this ID with a count of 1, load it to the palette, and return the new index
    auto FoundEmpty{ std::ranges::find_if(PaletteBankData, [](const PaletteBankDatum& Data) { return !Data.IsValid(); }) };
    if (FoundEmpty != PaletteBankData.end())
    {
        *FoundEmpty = { 1, ToAdd.ID };
        auto EmptyIndex{ std::distance(PaletteBankData.begin(), FoundEmpty) };
        (*PaletteBanks)[EmptyIndex] = ToAdd.Data;
        return EmptyIndex;
    }

    // Otherwise we're all full
    return INDEX_INVALID;
}

void PaletteManager::RemoveFromPalette(std::int32_t Index)
{
    //TODO: bounds checking on that index
    auto& FoundBank{ PaletteBankData[Index] };
    FoundBank.ReferenceCount--;
    if (FoundBank.ReferenceCount <= 0)
    {
        FoundBank.AssetID = PaletteBankAsset::ID_INVALID;
        (*PaletteBanks)[Index] = {}; //TODO: what's the down side of leaving this in here and stomping it later?
    }
}
