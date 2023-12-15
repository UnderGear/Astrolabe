#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <span>

#include "Assets/Asset.hpp"
#include "MemoryMap.hpp"
#include "Palette.hpp"

// based on video mode we have different background types available
// mode | BG0 | BG1 | BG2 | BG3 |
//   0  | reg | reg | reg | reg |
//   1  | reg | reg | aff |  -  |
//   2  |  -  |  -  | aff | aff |

constexpr inline std::size_t BackgroundCount{ 4 };
constexpr inline std::size_t AffineBackgroundCount{ 2 };

//TODO: pull register stuff to another header

// One register per background, so 4 total
struct BackgroundControlRegister
{
    std::uint16_t Priority : 2;
    std::uint16_t TileBlockBase : 2;
    std::uint16_t Padding : 2;
    std::uint16_t MosaicEnabled : 1;
    std::uint16_t ColorMode : 1;
    std::uint16_t ScreenBlockBase : 5;
    std::uint16_t AffineWrappingEnabled : 1;
    std::uint16_t BackgroundSize : 2;
};

//TODO: I don't think I actually need this struct in the code base. let's just use raw 32 bit entries
// These live in our background VRAM interspersed with actual tile data
// Use the BG's control register to configure where to look for the two in the layout
// Apparently it's customary to put tile maps at the end and tile data up front
struct BackgroundTileMapEntry
{
    std::uint16_t TileIndex : 10;
    std::uint16_t HorizontalFlip : 1;
    std::uint16_t VerticalFlip : 1;
    std::uint16_t PaletteBank : 4;
};

// a background tile is 8x8 pixels, so the screen is 30 tiles wide and 20 tiles tall
// a background tile map is 16x16-128x128 tiles
// Memory | 0x6000000 | 0x6004000 | 0x6008000 | 0x600C000 |
// Tiles  |     0     |     1     |     2     |     3     |
//  Maps  |   0 - 7   |  8 - 15   |  16 - 23  |  24 - 31  |

// array of background offsets
// write only
struct alignas(std::int32_t) BackgroundOffset
{
    std::int16_t X{ 0 };
    std::int16_t Y{ 0 };
};

// TODO: worry about affine backgrounds later
// write only
struct alignas(std::int32_t) BackgroundAffineParams
{
    std::int16_t A{ 0 };
    std::int16_t B{ 0 };
    std::int16_t C{ 0 };
    std::int16_t D{ 0 };
    std::int32_t X{ 0 };
    std::int32_t Y{ 0 };
};

using BackgroundTileAsset = Asset<std::span<const std::uint32_t>>;

//TODO: is this the right shape for these?
using BackgroundMapAsset = Asset<std::span<BackgroundTileMapEntry>>;

class BackgroundManager
{
    std::unique_ptr<std::array<BackgroundControlRegister, BackgroundCount>> ControlRegisters;
    std::unique_ptr<std::array<BackgroundOffset, BackgroundCount>> OffsetRegisters;
    std::unique_ptr<std::array<BackgroundAffineParams, AffineBackgroundCount>> AffineRegisters;

	PaletteManager BackgroundPaletteManager{ reinterpret_cast<void*>(BACKGROUND_PALETTE_ADDRESS) };

    // DANGER ZONE
    // The next 2 members are overlapping memory. We need to be VERY careful about what we allow access to
    // 32 allowable tile map blocks, 2048 bytes each = 1024 entries per block
    std::array<std::array<BackgroundTileMapEntry, 1024>, 32>* TileMapEntries{ reinterpret_cast<std::array<std::array<BackgroundTileMapEntry, 1024>, 32>*>(VRAM_ADDRESS) };
    // 4 blocks of raw tile data
    std::array<std::array<std::uint32_t, 4096>, 4>* TileBlocks{ reinterpret_cast<std::array<std::array<std::uint32_t, 4096>, 4>*>(VRAM_ADDRESS) };
    
public:

    std::int32_t AddToPalette(const PaletteBankAsset& PaletteAsset);
    void RemoveFromPalette(std::int32_t Index);

    //TODO: load tiles like we do with sprite manager into the tile blocks
    std::int32_t LoadTiles(const BackgroundTileAsset& ToAdd);
    void UnloadTiles(std::int32_t Index);

    //TODO: load up tile maps into blocks from the other end (should this be for a full background at a time?)
    std::int32_t LoadMap(const BackgroundMapAsset& ToAdd);
    void UnloadMap(std::int32_t Index);

    //TODO: look to see if we can use some of the unclaimed space from our tile blocks to fit the tile maps
    // maybe we can try using unclaimed space from the end of the given tile block for our map blocks

    //TODO: update the control registers for the enabled background(s)

    //TODO: look at the sbb example
};
