#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <span>

#include "Assets/Asset.hpp"
#include "DisplayRegisters.hpp"
#include "MemoryMap.hpp"
#include "Palette.hpp"

// based on video mode we have different background types available
// mode | BG0 | BG1 | BG2 | BG3 |
//   0  | reg | reg | reg | reg |
//   1  | reg | reg | aff |  -  |
//   2  |  -  |  -  | aff | aff |

struct BackgroundTileAsset : public Asset<std::span<const std::uint32_t>>
{
    RegularBackgroundDimensions Dimensions{ RegularBackgroundDimensions::t32xt32 };
};

//TODO: is this the right shape for these?
using BackgroundMapAsset = Asset<std::span<const std::uint16_t>>;

using BackgroundTileBlock = std::array<std::uint32_t, 4096>;
struct BackgroundTileBlockData
{
    BackgroundTileBlock::iterator NextAvailable; // A block entry always starts at the beginning of the block's memory
    std::int32_t AssetID{ BackgroundTileAsset::ID_INVALID };
    bool IsValid{ true }; //TODO: set these flags based on mode (see the chart at the top of the file)

    bool IsAvailable() const { return IsValid && AssetID == BackgroundTileAsset::ID_INVALID; }
};

struct BackgroundTileMapEntryData
{
    std::int32_t AssetID{ BackgroundMapAsset::ID_INVALID };
    std::int32_t BeginIndex{ 0 };
    //std::int32_t Length{ 0 };
};

class BackgroundManager
{
    static constexpr std::int32_t INDEX_INVALID{ -1 };

    static constexpr std::size_t BackgroundCount{ 4 };
    static constexpr std::size_t AffineBackgroundCount{ 2 };

    std::array<volatile BackgroundControlRegister, BackgroundCount>* ControlRegisters{ reinterpret_cast<std::array<volatile BackgroundControlRegister, BackgroundCount>*>(BG_CONTROL_ADDRESS) };
    std::array<volatile BackgroundOffset, BackgroundCount>* OffsetRegisters{ reinterpret_cast<std::array<volatile BackgroundOffset, BackgroundCount>*>(BG_OFFSET_ADDRESS) };
    std::array<volatile BackgroundAffineParams, AffineBackgroundCount>* AffineRegisters{ reinterpret_cast<std::array<volatile BackgroundAffineParams, AffineBackgroundCount>*>(BG_AFFINE_ADDRESS) };

	PaletteManager BackgroundPaletteManager{ reinterpret_cast<void*>(BACKGROUND_PALETTE_ADDRESS) };

    // DANGER ZONE
    // TileMapEntries and TileBlocks are overlapping memory. We need to be VERY careful about what we allow access to
    
    // 4 blocks of raw tile data
    std::array<BackgroundTileBlock, BackgroundCount>* TileBlocks{ reinterpret_cast<std::array<BackgroundTileBlock, BackgroundCount>*>(VRAM_ADDRESS) };

    // 32 allowable tile map blocks, 2048 bytes each = 1024 entries per map block
    static constexpr std::int32_t TileMapBlockCount{ 32 };
    static constexpr std::int32_t TileMapEntriesPerBlock{ 1024 };
    static constexpr std::int32_t MaxTileMapEntries{ TileMapBlockCount * TileMapEntriesPerBlock };
    //TODO: should this just be backed with a std::uint16_t instead? or maybe even 32 for loading speed?
    std::array<std::uint16_t, MaxTileMapEntries>* TileMapEntries{ reinterpret_cast<std::array<std::uint16_t, MaxTileMapEntries>*>(VRAM_ADDRESS) };
    
    std::array<BackgroundTileBlockData, BackgroundCount> LoadedTileBlocks;
    std::array<BackgroundTileMapEntryData, BackgroundCount> LoadedTileMaps;

public:

    void SetPalette(const PaletteAsset& ToSet);
    void ClearPalette();
    std::int32_t AddToPalette(const PaletteBankAsset& ToAdd);
    void RemoveFromPalette(std::int32_t Index);

    // Returns the base tile block index for the loaded tiles
    std::int32_t LoadTiles(const BackgroundTileAsset& ToAdd);
    void UnloadTiles(std::int32_t Index);

    // Returns the base tile map block index
    //TODO: I guess we'd do animations by modifying the map at runtime?
    std::int32_t LoadMap(const BackgroundMapAsset& ToAdd, std::int32_t TileBlockIndex);
    void UnloadMap(std::int32_t TileBlockIndex);

    volatile BackgroundControlRegister& GetControlRegister(std::int32_t BackgroundIndex);
    volatile BackgroundOffset& GetBackgroundOffset(std::int32_t BackgroundIndex);
};
