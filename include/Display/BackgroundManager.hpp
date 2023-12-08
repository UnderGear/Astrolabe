#pragma once

#include <cstdint>

//TODO we need to handle tiles and palettes

// based on video mode we have different background types available
// mode | BG0 | BG1 | BG2 | BG3 |
//   0  | reg | reg | reg | reg |
//   1  | reg | reg | aff |  -  |
//   2  |  -  |  -  | aff | aff |

struct BackgroundControlRegister
{
    std::uint16_t Priority : 2;
    std::uint16_t TileBlock : 2;
    std::uint16_t MosaicEnabled : 1;
    std::uint16_t ColorMode : 1;
    std::uint16_t ScreenBlock : 5;
    std::uint16_t AffineWrappingEnabled : 1;
    std::uint16_t BackgroundSize : 2;
};

struct BackgroundTile
{
    std::uint16_t TileIndex : 10;
    std::uint16_t HorizontalFlip : 1;
    std::uint16_t VerticalFlip : 1;
    std::uint16_t PaletteBank : 4;
};

class BackgroundManager
{

};
