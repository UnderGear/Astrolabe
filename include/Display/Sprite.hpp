#pragma once

#include "DisplayRegisters.hpp"
#include "SpriteManager.hpp"

class Mode0;
struct Point2D;

class Sprite
{
    SpriteManager& Owner;
    ObjectAttributes& Attributes; // this is the one in the buffer, not directly in vram

    std::int32_t TileAssetIndex;
    std::int32_t PaletteAssetIndex;

public:
    //TODO: pass in position and other OAM params
    //TODO: only allow the display to hand these out?
    explicit Sprite(SpriteManager& InOwner, ObjectAttributes& InAttributes, std::int32_t InTileAssetIndex, std::int32_t InPaletteAssetIndex);

    ~Sprite();

    // provide some access to the OAM
    void SetPosition(const Point2D& Position);
};
