#pragma once

#include <cstdint>
#include <span>
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

//TODO: frame durations?
//TODO: include dimensions as template parameters on the sprite tile asset, then on the sprite?
using Animation = std::vector<const SpriteTileAsset*>;

class Sprite
{
    SpriteManager& Owner;
    ObjectAttributes& Attributes; // this is the one in the buffer, not directly in vram

    //TODO: I would much rather have something purely on the stack here.
    std::span<Animation> Animations;
    const SpriteTileAsset* CurrentSpriteAsset;
    std::int32_t CurrentAnimationIndex{ 0 };
    bool ShouldFlipHorizontal{ false };
    std::int32_t PaletteAssetIndex;
    std::int32_t InitialFrame;

    std::int32_t CurrentFrameIndex{ 0 };
    std::int32_t CurrentFrameCounter{ 0 };

    std::int32_t HalfWidth{ 0 };
    std::int32_t HalfHeight{ 0 };

public:
    //TODO: pass in position and other OAM params
    //TODO: only allow the display to hand these out?
    explicit Sprite(SpriteManager& InOwner, ObjectAttributes& InAttributes, std::span<Animation> InAnimations, std::int32_t InPaletteAssetIndex, std::int32_t InInitialFrame);

    ~Sprite();

    // provide some access to the OAM
    void SetPosition(const Point2D& Position);

    void SetSpriteAnimationIndex(std::int32_t AnimationIndex);

    void SetShouldFlipHorizontal(bool InShouldFlipHorizontal);

    void Tick();
};
 