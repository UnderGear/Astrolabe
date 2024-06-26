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
struct AnimationFrame
{
    const SpriteTileAsset* Asset{ nullptr };
    std::int32_t FrameDuration{ 0 };
};
using Animation = std::vector<AnimationFrame>;
using AnimationSuite = std::span<const Animation>;

class Sprite
{
    SpriteManager& Owner;
    ObjectAttributes& Attributes; // this is the one in the buffer, not directly in vram

    AnimationSuite Animations;
    const SpriteTileAsset* CurrentSpriteAsset;
    std::int32_t CurrentAnimationIndex{ 0 };
    bool ShouldFlipHorizontal{ false };
    std::int32_t PaletteAssetIndex{ 0 };

    std::int32_t CurrentFrameIndex{ 0 };
    std::int32_t CurrentFrameCounter{ 0 };

    std::int32_t HalfWidth{ 0 };
    std::int32_t HalfHeight{ 0 };

public:
    //TODO: pass in position and other OAM params
    //TODO: only allow the display to hand these out?
    explicit Sprite(SpriteManager& InOwner, ObjectAttributes& InAttributes, const AnimationSuite& InAnimations, std::int32_t InPaletteAssetIndex);

    Sprite(Sprite&&) = default;
    Sprite& operator =(Sprite&&) = default;

    Sprite(const Sprite&) = delete;
    Sprite& operator =(const Sprite&) = delete;

    ~Sprite();

    // provide some access to the OAM
    void SetPosition(const Point2D& Position);

    void SetSpriteAnimationIndex(std::int32_t AnimationIndex);

    void SetShouldFlipHorizontal(bool InShouldFlipHorizontal);

    void Tick();

    std::int32_t GetHalfWidth() const { return HalfWidth; }
    std::int32_t GetHalfHeight() const { return HalfHeight; }
};
 