#pragma once

#include <cstdint>
#include <iterator>
#include <span>
#include <stack>
#include <vector>

#include "DisplayRegisters.hpp"
#include "MemoryMap.hpp"
#include "Palette.hpp"
#include "Assets/Asset.hpp"

// these are for our current color mode with 16 palettes
constexpr inline std::uint32_t CountPerTile{ 8 };
using Tile = std::array<std::uint32_t, CountPerTile>; // 4 8 bit color indices in each uint32, 8 of them
using TileBlock = std::array<Tile, 512>; // 512 blocks of normal width tiles
using TileBlockRaw = std::array<std::uint32_t, 4096>;
using DoubleTileBlock = std::array<Tile, 1024>; // 2 normal tile blocks back to back
using DoubleTileBlockRaw = std::array<std::uint32_t, 8192>;

//TODO: support wide tiles with other modes
//TODO: note that in sprites, indexing is based off of the normal tile sizes. you just end up with even indices of wide tiles
// but that is NOT the case for backgrounds for whatever reason.
// these are for the other color mode with a single giant palette
using WideTile = std::array<std::uint32_t, 16>; // 2 16 bit colors in each uint32, 16 of them
using WideTileBlock = std::array<WideTile, 256>; // 256 blocks of wide tiles

// there are 6 tile blocks. first 4 for backgrounds, then 2 for sprites
// in bitmap graphics modes, the first of the 2 sprite tile blocks is used by the VRAM, so only the second is available

using SpriteTileAsset = Asset<std::span<const std::uint32_t>>;

//TODO: template on storage type (for raw data array size) and tile type for indexing?
class SpriteManager
{
public:
    static constexpr std::int32_t INDEX_INVALID{ -1 };

private:

	PaletteManager SpritePaletteManager{ reinterpret_cast<void*>(SPRITE_PALETTE_ADDRESS) };

    // Pointer to our raw data in VRAM
    std::unique_ptr<DoubleTileBlockRaw> SpriteBlock;
    // Iterator to the next available spot to insert at the end of VRAM
	DoubleTileBlockRaw::iterator SpriteBlockIterator;

    // Bookkeeping to track a loaded tile asset, reference counted
    struct SpriteTileData
    {
        std::int32_t ReferenceCount{ 0 };
        std::int32_t AssetID{ PaletteBankAsset::ID_INVALID }; //TODO: probably should be the asset ID value, not palette
        std::int32_t BeginIndex{ 0 };
        std::int32_t Length{ 0 };

        std::int32_t GetTileIndex() const { return BeginIndex / CountPerTile; }
    };

    // All current loaded tile data by asset
    std::vector<SpriteTileData> LoadedTiles;

    // Bookkeeping to track a gap in our raw data
    struct TileGap
    {
        std::int32_t BeginIndex{ 0 };
        std::int32_t Length{ 0 };

        bool Shrink(std::int32_t Amount)
        {
            Length -= Amount;
            BeginIndex += Amount;

            return Length == 0;
        }
    };

    // All current gaps in raw data
    std::vector<TileGap> Gaps;

    std::vector<SpriteManager::TileGap>::iterator FindSuitableGap(std::int32_t Size);
    void MergeGaps();

    static constexpr std::uint32_t MaxOAMs{ 128 };
	using OAMT = std::array<ObjectAttributes, MaxOAMs>;
	OAMT* OAMMemory{ nullptr };
	using OAMAffineT = std::array<ObjectAttributesAffine, 32>;
	OAMAffineT* OAMAffineMemory{ nullptr };

	// in the example https://www.coranac.com/tonc/text/regobj.htm 8.4.5 he creates a double buffer
	//TODO: swap this into OAM during vblanks
	//TODO: does this need to be volatile?
	OAMT ObjectBuffer;
	OAMAffineT* ObjectBufferAffine{ std::bit_cast<OAMAffineT*>(&ObjectBuffer) };

    std::stack<ObjectAttributes*, std::vector<ObjectAttributes*>> AvailableObjectAttributes;

public:
    explicit SpriteManager(void* TileMemoryAddress);

    void Tick();

    void SetPalette(const PaletteAsset& ToSet);
    void ClearPalette();
    std::int32_t AddToPalette(const PaletteBankAsset& PaletteAsset);
    void RemoveFromPalette(std::int32_t Index);

	//TODO: should this be limited in sizes available?
    std::int32_t LoadTiles(const SpriteTileAsset& ToAdd);
    void UnloadTiles(std::int32_t Index);
    
    ObjectAttributes* RequestOAM();
    void ReleaseOAM(ObjectAttributes& OAM);
};
