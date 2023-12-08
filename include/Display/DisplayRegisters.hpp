#pragma once

#include <array>
#include <cstdint>

enum class DisplayMode
{
	Mode0,
	Mode1,
	Mode2,
	Mode3,
	Mode4,
	Mode5,
};

enum class BackgroundLayerFlags
{
	None		= 0b00000,
	Background0 = 0b00001,
	Background1 = 0b00010,
	Background2 = 0b00100,
	Background3 = 0b01000,
	Object 		= 0b10000,
};

enum class WindowDisplayFlags
{
	None 	= 0b000,
	Window0 = 0b001,
	Window1 = 0b010,
	OBJ		= 0b100,
};

struct DisplayControlRegister
{
	std::uint16_t Mode : 3; // DisplayMode
	std::uint16_t Reserved : 1;
	std::uint16_t DisplayPageSelect : 1; // for modes 4, 5 only. toggle between pages at runtime to switch out the screen
	std::uint16_t HBlankIntervalFree : 1; // 1: allow access to OAM during H-Blank
	std::uint16_t OBJCharacterVRAMMapping : 1; // 0 2D, 1 1D
	std::uint16_t ForcedBlank : 1;
	std::uint16_t BackgroundFlags : 5; // BackgroundLayerFlags
	std::uint16_t WindowFlags : 3; // WindowDisplayFlags

	constexpr explicit DisplayControlRegister(DisplayMode InMode, BackgroundLayerFlags InBackgroundFlags, WindowDisplayFlags InWindowFlags = WindowDisplayFlags::None) noexcept
		: Mode(static_cast<std::uint16_t>(InMode)), Reserved(0), DisplayPageSelect(0), HBlankIntervalFree(0)
		, OBJCharacterVRAMMapping(0), ForcedBlank(0), BackgroundFlags(static_cast<std::uint16_t>(InBackgroundFlags))
		, WindowFlags(static_cast<std::uint16_t>(InWindowFlags))
	{ }
};

struct DisplayStatusRegister
{
	std::uint16_t VBlankStatus : 1;
	std::uint16_t HBlankStatus : 1;
	std::uint16_t VCountTriggerStatus : 1;
	std::uint16_t VBlankInterruptRequest : 1;
	std::uint16_t HBlankInterruptRequest : 1;
	std::uint16_t VCountInterruptRequest : 1;
	std::uint16_t Padding : 2;
	std::uint16_t VCountTriggerValue : 8;
};

struct VerticalCountRegister
{
	std::uint16_t VerticalCount : 8;
	std::uint16_t Padding : 8;
};


// These live in OAM_ADDRESS
// note that these things overlay one another in memory. 4 ObjectAttributes interleave in 1 ObjectAttributesAffine
// we have space for 128 ObjectAttributes and 32 ObjectAttributesAffine


enum class Attribute0ObjectMode
{
	Normal,
	Affine,
	Hidden,
	AffineDoubleArea
};

enum class Attribute0GraphicsMode
{
	Normal,
	Alpha,
	ObjectWindow, // serves as a mask for BGs and sprites?
	Forbidden
};

enum class Attribute0SpriteShape
{
	Square,
	Wide, //TODO: do I have wide/tall mixed up?
	Tall,
};

struct Attribute0Register
{
	std::uint16_t YCoordinate : 8; // top of the sprite
	std::uint16_t ObjectMode : 2;
	std::uint16_t GraphicsMode : 2;
	std::uint16_t MosaicEnabled : 1;
	std::uint16_t ColorMode : 1; // 0: 16 colors, 1: 256 colors
	std::uint16_t SpriteShape : 2; // Attribute0SpriteShape
};

struct Attribute1RegisterStandard
{
	std::uint16_t XCoordinate : 9; // left edge of sprite
	std::uint16_t Padding : 3;
	std::uint16_t HorizontalFlip : 1;
	std::uint16_t VerticalFlip : 1;
	std::uint16_t SpriteSize : 2;
};

struct Attribute1RegisterAffine
{
	std::uint16_t Padding0 : 9;
	std::uint16_t AffineIndex : 5;
	std::uint16_t Padding1 : 2;
};

// For non-affine sprites
// s i z e    
// h |    |   00 |  01   |  10   |  11   |
// a | 00 |  8x8 | 16x16 | 32x32 | 64x64 |
// p | 01 | 16x8 | 32x8  | 32x16 | 64x32 |
// e | 10 | 8x16 | 8x32  | 16x32 | 32x64 |


struct Attribute1Register
{
	// variant does extra bookkeeping, so we can't plop one of those in this register. maybe we should just bit_cast if we want to avoid union
	union
	{
		Attribute1RegisterStandard Standard;
		Attribute1RegisterAffine Affine;
	};
};

struct Attribute2Register
{
	std::uint16_t TileIndex : 10;
	std::uint16_t Priority : 2; // higher priority sprites draw first and can get overdrawn. sprites draw over backgrounds of equal priority.
	std::uint16_t PaletteBank : 4; // when using 16 color mode only
};

struct alignas(std::uint32_t) ObjectAttributes
{
	Attribute0Register Attribute0;
	Attribute1Register Attribute1;
	Attribute2Register Attribute2;
	volatile std::int16_t Fill;

	ObjectAttributes()
	{
		Attribute0.ObjectMode = static_cast<std::uint16_t>(Attribute0ObjectMode::Hidden);
	}
};

struct alignas(std::uint32_t) ObjectAttributesAffine
{
	volatile std::array<std::uint16_t, 3> Fill0;
	std::int16_t Pa;
	volatile std::array<std::uint16_t, 3> Fill1;
	std::int16_t Pb;
	volatile std::array<std::uint16_t, 3> Fill2;
	std::int16_t Pc;
	volatile std::array<std::uint16_t, 3> Fill3;
	std::int16_t Pd;
};
