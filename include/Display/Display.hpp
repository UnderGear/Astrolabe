#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cstdint>

#include "BackgroundManager.hpp"
#include "DisplayRegisters.hpp"
#include "MemoryMap.hpp"
#include "Palette.hpp"
#include "SpriteManager.hpp"

class Sprite;
class Background;

constexpr inline std::int32_t SCREEN_WIDTH{ 240 };
constexpr inline std::int32_t SCREEN_HEIGHT{ 160 };

//TODO: and maybe it's time to start using bit_cast instead of these unique pointers to memory
class Display
{
	std::unique_ptr<volatile DisplayControlRegister> DisplayControl
	{
		new(reinterpret_cast<void*>(DISPLAY_CONTROL_ADDRESS))
			DisplayControlRegister{ DisplayMode::Mode0, BackgroundLayerFlags::None, WindowDisplayFlags::None }
	};
	std::unique_ptr<volatile DisplayStatusRegister> DisplayStatus
	{
		new(reinterpret_cast<void*>(DISPLAY_STATUS_ADDRESS)) DisplayStatusRegister{ }
	};
	std::unique_ptr<volatile VerticalCountRegister> VerticalCount
	{
		new(reinterpret_cast<void*>(DISPLAY_SCANLINE_COUNT_ADDRESS)) VerticalCountRegister{ }
	};

	SpriteManager Sprites{ reinterpret_cast<void*>(SPRITE_LOW_BLOCK_ADDRESS) };
	BackgroundManager Backgrounds;

public:
	explicit Display();

	Sprite LoadSprite(const std::vector<const SpriteTileAsset*>& TileAssets, const PaletteBankAsset& PaletteAsset, std::int32_t CurrentFrame);
	Background LoadBackground(const BackgroundTileAsset& BackgroundAsset, const PaletteAsset& PaletteAsset, const BackgroundMapAsset& MapAsset);

	void VSync()
	{
		while (VerticalCount->VerticalCount >= 160);
		while (VerticalCount->VerticalCount < 160);
	}

	void Tick()
	{
		Sprites.Tick();
	}
};


// one of the bitmap modes, adapted from tonc's early examples. proooobably not to be used in actual code
// class Mode3
// {
// 	using Mode3VRAM = std::array<Color, SCREEN_WIDTH * SCREEN_HEIGHT>;

// 	std::unique_ptr<volatile DisplayControlRegister> DisplayControl;
// 	std::unique_ptr<Mode3VRAM> VideoMemory;

// public:
// 	explicit Mode3()
// 	{
// 		DisplayControl = std::unique_ptr<volatile DisplayControlRegister>
// 		{
// 			new(reinterpret_cast<void*>(DISPLAY_CONTROL_ADDRESS))
// 				DisplayControlRegister{ DisplayMode::Mode3, BackgroundLayerFlags::Background2 }
// 		};

// 		VideoMemory = std::unique_ptr<Mode3VRAM>
// 		{
// 			new(reinterpret_cast<void*>(VRAM_ADDRESS)) Mode3VRAM{ }
// 		};
// 	}

// 	// REQUIRES MODE 3 set the pixel at the specified coordinates to the specified color
// 	inline void DrawPoint(Point2D point, Color color)
// 	{
// 		// write to the VRAM directly
// 		(*VideoMemory)[point.Y * SCREEN_WIDTH + point.X] = color;
// 	}

// 	inline void Fill(Color color)
// 	{
// 		VideoMemory->fill(color);
// 	}
// };
