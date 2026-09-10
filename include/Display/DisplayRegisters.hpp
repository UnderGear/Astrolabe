#pragma once

#include <array>
#include <bit>
#include <cstdint>
#include <utility>

#include "Hardware/Register.hpp"
#include "Math/Fixed.hpp"

struct DisplayControlRegister : Register<std::uint16_t>
{
	enum class DisplayModeOptions
	{
		Mode0,
		Mode1,
		Mode2,
		Mode3,
		Mode4,
		Mode5,
	};
	using DisplayMode = RegisterAccess<std::uint16_t, DisplayModeOptions, 0b00000000'00000111>;

	// true if there is a GBC cartridge inserted
	using GBC = RegisterAccess<std::uint16_t, bool, 0b00000000'00001000, RegisterAccessType::ReadOnly>; 

	enum class DisplayPageSelectOptions
	{
		Page0,
		Page1,
	};
	using DisplayPageSelect = RegisterAccess<std::uint16_t, DisplayPageSelectOptions, 0b00000000'00010000>;

	using HBlankIntervalFree = RegisterAccess<std::uint16_t, bool, 0b00000000'00100000>; // allow access to OAM during H-Blank

	enum class OBJCharacterVRAMMappingOptions
	{
		Mapping2D,
		Mapping1D,
	};
	using OBJCharacterVRAMMapping = RegisterAccess<std::uint16_t, OBJCharacterVRAMMappingOptions, 0b00000000'01000000>;

	using ForcedBlank = RegisterAccess<std::uint16_t, bool, 0b00000000'10000000>;
	
	using Background0 = RegisterAccess<std::uint16_t, bool, 0b00000001'00000000>;
	using Background1 = RegisterAccess<std::uint16_t, bool, 0b00000010'00000000>;
	using Background2 = RegisterAccess<std::uint16_t, bool, 0b00000100'00000000>;
	using Background3 = RegisterAccess<std::uint16_t, bool, 0b00001000'00000000>;
	using ObjectLayer = RegisterAccess<std::uint16_t, bool, 0b00010000'00000000>;

	using Window0 = RegisterAccess<std::uint16_t, bool, 0b00100000'00000000>;	
	using Window1 = RegisterAccess<std::uint16_t, bool, 0b01000000'00000000>;
	using ObjectWindow = RegisterAccess<std::uint16_t, bool, 0b10000000'00000000>;

	constexpr explicit DisplayControlRegister(
		DisplayModeOptions ModeValue,
		bool Background0Value,
		bool Background1Value,
		bool Background2Value,
		bool Background3Value,
		bool ObjectLayerValue,
		bool Window0Value,
		bool Window1Value,
		bool ObjectWindowValue)
	{
		SetBatch<
			DisplayMode,
			DisplayPageSelect,
			HBlankIntervalFree,
			OBJCharacterVRAMMapping,
			Background0,
			Background1,
			Background2,
			Background3,
			ObjectLayer,
			Window0,
			Window1,
			ObjectWindow>(
				ModeValue,
				DisplayPageSelectOptions::Page0,
				false,
				OBJCharacterVRAMMappingOptions::Mapping2D,
				Background0Value,
				Background1Value,
				Background2Value,
				Background3Value,
				ObjectLayerValue,
				Window0Value,
				Window1Value,
				ObjectWindowValue
			);
	}
};

struct DisplayStatusRegister : Register<std::uint16_t>
{
	using VBlankStatus = RegisterAccess<std::uint16_t, bool, 0b00000000'00000001, RegisterAccessType::ReadOnly>;
	using HBlankStatus = RegisterAccess<std::uint16_t, bool, 0b00000000'00000010, RegisterAccessType::ReadOnly>;
	using VCountTriggerStatus = RegisterAccess<std::uint16_t, bool, 0b00000000'00000100, RegisterAccessType::ReadOnly>;
	using VBlankInterruptRequest = RegisterAccess<std::uint16_t, bool, 0b00000000'00001000>;
	using HBlankInterruptRequest = RegisterAccess<std::uint16_t, bool, 0b00000000'00010000>;
	using VCountInterruptRequest = RegisterAccess<std::uint16_t, bool, 0b00000000'00100000>;
	using VCountTriggerValue = RegisterAccess<std::uint16_t, std::uint16_t, 0b11111111'00000000, RegisterAccessType::ReadOnly>;
};

struct VerticalCountRegister : Register<std::uint16_t>
{
	using VerticalCount = RegisterAccess<std::uint16_t, std::uint8_t, 0b00000000'00000000, RegisterAccessType::ReadOnly>;
};


// These live in OAM_ADDRESS
// note that these things overlay one another in memory. 4 ObjectAttributes interleave in 1 ObjectAttributesAffine
// we have space for 128 ObjectAttributes and 32 ObjectAttributesAffine, meaning that out of our 128 sprites, 32 may be affine at a time

struct Attribute0Register : Register<std::uint16_t>
{
	using Y = RegisterAccess<std::uint16_t, std::uint16_t, 0b00000000'11111111>;

	enum class ObjectModeOptions
	{
		Normal,
		Affine,
		Hidden,
		AffineDoubleArea
	};
	using ObjectMode = RegisterAccess<std::uint16_t, ObjectModeOptions, 0b00000011'00000000>;

	enum class GraphicsModeOptions
	{
		Normal,
		Alpha,
		ObjectWindow, // serves as a mask for BGs and sprites?
		Forbidden
	};
	using GraphicsMode = RegisterAccess<std::uint16_t, GraphicsModeOptions, 0b00001100'00000000>;

	using MosaicMode = RegisterAccess<std::uint16_t, bool, 0b00010000'00000000>;

	enum class ColorModeOptions
	{
		PaletteBank, // 16 colors per bank, this mode addresses a single bank
		WholePalette // 256 total colors can fit in palette memory, this mode can address them all
	};
	using ColorMode = RegisterAccess<std::uint16_t, ColorModeOptions, 0b00100000'00000000>;

	enum class SpriteShapeOptions
	{
		Square,
		Wide, //TODO: do I have wide/tall mixed up?
		Tall,
	};
	using SpriteShape = RegisterAccess<std::uint16_t, SpriteShapeOptions, 0b11000000'00000000>;

	void SetData(std::uint16_t YValue, ObjectModeOptions ObjectModeValue, GraphicsModeOptions GraphicsModeValue, bool MosaicEnabledValue, ColorModeOptions ColorModeValue, SpriteShapeOptions SpriteShapeValue) volatile
	{
		SetBatch<Y, ObjectMode, GraphicsMode, MosaicMode, ColorMode, SpriteShape>(YValue, ObjectModeValue, GraphicsModeValue, MosaicEnabledValue, ColorModeValue, SpriteShapeValue);
	}
};

struct Attribute1Register : Register<std::uint16_t>
{
	using X = RegisterAccess<std::uint16_t, std::uint16_t, 0b00000001'11111111>;

	// Depending on the selected ObjectMode in register 0, this register can take a couple different forms
	// Normal:
	using HorizontalFlip = RegisterAccess<std::uint16_t, bool, 0b00010000'00000000>;
	using VerticalFlip = RegisterAccess<std::uint16_t, bool, 0b00100000'00000000>;

	// Affine/AffineDoubleArea:
	using AffineIndex = RegisterAccess<std::uint16_t, std::uint16_t, 0b00111110'00000000>;

	enum class SpriteSizeOptions
	{
		S8,
		S16,
		S32,
		S64,
	};
	using SpriteSize = RegisterAccess<std::uint16_t, SpriteSizeOptions, 0b11000000'00000000>;

	// Normal Mode
	void SetData(std::uint16_t XValue, bool HorizontalFlipValue, bool VerticalFlipValue, SpriteSizeOptions SpriteSizeValue) volatile
	{
		SetBatch<X, HorizontalFlip, VerticalFlip, SpriteSize>(XValue, HorizontalFlipValue, VerticalFlipValue, SpriteSizeValue);
	}

	// Affine Modes
	void SetData(std::uint16_t XValue, std::uint16_t AffineIndexValue, SpriteSizeOptions SpriteSizeValue) volatile
	{
		SetBatch<X, AffineIndex, SpriteSize>(XValue, AffineIndexValue, SpriteSizeValue);
	}
};

// Size/shape for non-affine sprites
// S i z e    
// h |    |   00 |  01   |  10   |  11   |
// a | 00 |  8x8 | 16x16 | 32x32 | 64x64 |
// p | 01 | 16x8 | 32x8  | 32x16 | 64x32 |
// e | 10 | 8x16 | 8x32  | 16x32 | 32x64 |
// WOW this compiler really doesn't let you get away with any template argument deduction on these initializers
inline constexpr std::array<std::array<std::pair<std::uint16_t, std::uint16_t>, 4>, 3> SpriteDimensions
{
	std::array<std::pair<std::uint16_t, std::uint16_t>, 4>{ std::pair<std::uint16_t, std::uint16_t>{ 8, 8 }, std::pair<std::uint16_t, std::uint16_t>{ 16, 16 }, std::pair<std::uint16_t, std::uint16_t>{ 32, 32 }, std::pair<std::uint16_t, std::uint16_t>{ 64, 64 } },
	std::array<std::pair<std::uint16_t, std::uint16_t>, 4>{ std::pair<std::uint16_t, std::uint16_t>{ 16, 8 }, std::pair<std::uint16_t, std::uint16_t>{ 32, 8 }, std::pair<std::uint16_t, std::uint16_t>{ 32, 16 }, std::pair<std::uint16_t, std::uint16_t>{ 64, 32 } },
	std::array<std::pair<std::uint16_t, std::uint16_t>, 4>{ std::pair<std::uint16_t, std::uint16_t>{ 8, 16 }, std::pair<std::uint16_t, std::uint16_t>{ 8, 32 }, std::pair<std::uint16_t, std::uint16_t>{ 16, 32 }, std::pair<std::uint16_t, std::uint16_t>{ 32, 64 } }
};

inline constexpr std::pair<std::uint16_t, uint16_t> GetSpriteDimensions(Attribute0Register::SpriteShapeOptions Shape, Attribute1Register::SpriteSizeOptions Size)
{
	return SpriteDimensions[static_cast<std::size_t>(Shape)][static_cast<std::size_t>(Size)];
}

struct Attribute2Register : Register<std::uint16_t>
{
	// Index into tile memory
	using TileIndex = RegisterAccess<std::uint16_t, std::uint16_t, 0b00000011'11111111>;

	// Higher priority sprites draw first and can get overdrawn. Sprites draw over backgrounds of equal priority.
	enum class PriorityOptions
	{
		VeryLow,
		Low,
		Medium,
		High
	};
	using Priority = RegisterAccess<std::uint16_t, PriorityOptions, 0b00001100'00000000>;

	// Only used when in 16 color mode
	using PaletteBank = RegisterAccess<std::uint16_t, std::uint16_t, 0b11110000'00000000>;

	void SetData(std::uint16_t TileIndexValue, PriorityOptions PriorityValue, std::uint16_t PaletteBankValue) volatile
	{
		SetBatch<TileIndex, Priority, PaletteBank>(TileIndexValue, PriorityValue, PaletteBankValue);
	}
};

struct alignas(std::uint32_t) ObjectAttributes
{
	volatile Attribute0Register Attribute0;
	volatile Attribute1Register Attribute1;
	volatile Attribute2Register Attribute2;
	volatile std::uint16_t Fill;

	ObjectAttributes()
	{
		Attribute0.Set<Attribute0Register::ObjectMode>(Attribute0Register::ObjectModeOptions::Hidden);
	}
};

//TODO: make this a register that spans multiple std::uint16_ts?
struct alignas(std::uint32_t) ObjectAttributesAffine
{
	volatile std::array<std::uint16_t, 3> Fill0;
	i8f8_t Pa;
	volatile std::array<std::uint16_t, 3> Fill1;
	i8f8_t Pb;
	volatile std::array<std::uint16_t, 3> Fill2;
	i8f8_t Pc;
	volatile std::array<std::uint16_t, 3> Fill3;
	i8f8_t Pd;
};

//TODO:
// One register per background, so 4 total
struct BackgroundControlRegister : Register<std::uint16_t>
{
	// Higher priority backgrounds draw first and can get overdrawn. Sprites draw over backgrounds of equal priority.
	enum class PriorityOptions
	{
		VeryLow,
		Low,
		Medium,
		High
	};
	using Priority = RegisterAccess<std::uint16_t, PriorityOptions, 0b00000000'00000011>;

	using TileBlockBaseIndex = RegisterAccess<std::uint16_t, std::uint16_t, 0b00000000'00001100>;
	
	using MosaicEnabled = RegisterAccess<std::uint16_t, bool, 0b00000000'01000000>;

	enum class ColorModeOptions
	{
		PaletteBank, // 16 colors per bank, this mode addresses a single bank
		WholePalette // 256 total colors can fit in palette memory, this mode can address them all
	};
	using ColorMode = RegisterAccess<std::uint16_t, ColorModeOptions, 0b00000000'10000000>;

	using TileMapBlockBaseIndex = RegisterAccess<std::uint16_t, std::uint16_t, 0b00011111'00000000>;
	
	using AffineWrappingEnabled = RegisterAccess<std::uint16_t, bool, 0b00100000'00000000>;

	// Regular vs Affine backgrounds have different dimensions
	// BackgroundSize (regular backgrounds)
	//    | tiles | pixels  |
	// 00 | 32x32 | 256x256 |
	// 01 | 64x32 | 512x256 |
	// 10 | 32x64 | 512x256 |
	// 11 | 64x64 | 512x512 |
	enum class RegularBackgroundDimensions
	{
		t32xt32,
		t64xt32,
		t32xt64,
		t64xt64,
	};
	// BackgroundSize (affine backgrounds)
	//    |  tiles  |  pixels   |
	// 00 |  16x16  |  128x128  |
	// 01 |  32x32  |  256x256  |
	// 10 |  64x64  |  512x512  |
	// 11 | 128x128 | 1024x1024 |
	enum class AffineBackgroundDimensions
	{
		t16x16,
		t32x32,
		t64x64,
		t128x128
	};

	using RegularBackgroundSize = RegisterAccess<std::uint16_t, RegularBackgroundDimensions, 0b11000000'00000000>;
	using AffineBackgroundSize = RegisterAccess<std::uint16_t, AffineBackgroundDimensions, 0b11000000'00000000>;
};

// These live in our background VRAM interspersed with actual tile data
// Use the BG's control register to configure where to look for the two in the layout
// Apparently it's customary to put tile maps at the end and tile data up front
struct BackgroundTileMapEntry : Register<std::uint16_t>
{
	using TileIndex = RegisterAccess<std::uint16_t, std::uint16_t, 0b00000011'11111111>;
	using HorizontalFlip = RegisterAccess<std::uint16_t, bool, 0b00000100'00000000>;
	using VerticalFlip = RegisterAccess<std::uint16_t, bool, 0b00001000'00000000>;
	using PaletteBank = RegisterAccess<std::uint16_t, std::uint16_t, 0b11110000'00000000>;
};

// a background tile is 8x8 pixels, so the screen is 30 tiles wide and 20 tiles tall
// a background tile map is 16x16-128x128 tiles
// Memory | 0x6000000 | 0x6004000 | 0x6008000 | 0x600C000 |
// Tiles  |     0     |     1     |     2     |     3     |
//  Maps  |   0 - 7   |  8 - 15   |  16 - 23  |  24 - 31  |

// array of background offsets
// write only
struct BackgroundOffset : Register<std::uint32_t, RegisterAccessType::WriteOnly>
{
	using X = RegisterAccess<std::uint32_t, std::int16_t, 0b00000000'00000000'11111111'11111111, RegisterAccessType::WriteOnly>;
	using Y = RegisterAccess<std::uint32_t, std::int16_t, 0b11111111'11111111'00000000'00000000, RegisterAccessType::WriteOnly>;
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
