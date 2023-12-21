#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <span>

#include "Assets/Asset.hpp"

inline constexpr std::int32_t MaxPaletteBanks{ 16 };
using RawPaletteBank = std::array<std::uint32_t, 8>;
using RawPaletteBanks = std::array<RawPaletteBank, MaxPaletteBanks>;

using PaletteBankAsset = Asset<RawPaletteBank>;

using RawPalette = std::array<std::uint32_t, 128>;
using PaletteAsset = Asset<std::span<const std::uint32_t>>;

class PaletteManager
{
public:
    static constexpr std::int32_t INDEX_INVALID{ -1 };

private:
    struct PaletteBankDatum
    {
        std::int32_t ReferenceCount{ 0 };
        std::int32_t AssetID{ PaletteBankAsset::ID_INVALID };

        bool IsValid() const { return AssetID != PaletteBankAsset::ID_INVALID; }
    };

    // Palette bookkeeping. We only have up to 16 banks, so I'll just linear search through this until it becomes a problem
    std::array<PaletteBankDatum, MaxPaletteBanks> PaletteBankData;

    // hardware memory buffer for palettes
    RawPaletteBanks* PaletteBanks;
    RawPalette* Palette;

public:

    explicit PaletteManager(void* PaletteMemoryAddress);

    void SetPalette(const PaletteAsset& ToSet);
    void ClearPalette();

    // Loads the asset into one of our 16 available palette banks
    // Returns the palette bank ID for use in OAM
    std::int32_t AddToPalette(const PaletteBankAsset& ToAdd);

    void RemoveFromPalette(std::int32_t Index);
};
