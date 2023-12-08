#pragma once

#include <cstdint>
#include <vector>

#include "DisplayRegisters.hpp"
#include "SpriteManager.hpp"

struct Point2D;

enum class SpriteAnimationBehavior
{
	OneShot,
	Looping,
	PingPong,
};

class Sprite
{
    SpriteManager& Owner;
    ObjectAttributes& Attributes; // this is the one in the buffer, not directly in vram

    //TODO: I would much rather have something purely on the stack here.
    std::vector<std::int32_t> TileAssetIndices;
    std::int32_t PaletteAssetIndex;
    std::int32_t InitialFrame;

    std::int32_t CurrentFrameIndex{ 0 };

public:
    //TODO: pass in position and other OAM params
    //TODO: only allow the display to hand these out?
    explicit Sprite(SpriteManager& InOwner, ObjectAttributes& InAttributes, std::vector<std::int32_t> InTileAssetIndices, std::int32_t InPaletteAssetIndex, std::int32_t InInitialFrame);

    ~Sprite();

    // provide some access to the OAM
    void SetPosition(const Point2D& Position);

    void Tick(std::int32_t CurrentFrame);
};
 