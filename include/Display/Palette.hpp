#pragma once

#include <array>
#include <cstdint>
#include <memory>

#include "Assets/Asset.hpp"

inline constexpr std::int32_t MaxPaletteBanks{ 16 };
using RawPaletteBank = std::array<std::uint32_t, 8>;
using RawPaletteBanks = std::array<RawPaletteBank, MaxPaletteBanks>;

using PaletteBankAsset = Asset<RawPaletteBank>;

//TODO: support the other graphics mode that lets us use the entire palette memory as a single large palette instead of 16 banks
//using RawPaletteMemory = std::array<std::uint32_t, 128>; // 256 entries of 16 bit colors (packed 2 at a time)

class PaletteManager
{
public:
    static constexpr std::int32_t INDEX_INVALID{ -1 };

private:
    struct PaletteBankData
    {
        std::int32_t ReferenceCount{ 0 };
        std::int32_t AssetID{ PaletteBankAsset::ID_INVALID };

        bool IsValid() const { return AssetID != PaletteBankAsset::ID_INVALID; }
    };

    // Palette bookkeeping. We only have up to 16 banks, so I'll just linear search through this until it becomes a problem
    std::array<PaletteBankData, MaxPaletteBanks> PaletteBanks;

    // hardware memory buffer for palettes
    std::unique_ptr<RawPaletteBanks> Palette;

public:

    explicit PaletteManager(void* PaletteMemoryAddress);

    // Loads the asset into one of our 16 available palette banks
    // Returns the palette bank ID for use in OAM
    std::int32_t AddToPalette(const PaletteBankAsset& PaletteAsset);

    void RemoveFromPalette(std::int32_t Index);
};
