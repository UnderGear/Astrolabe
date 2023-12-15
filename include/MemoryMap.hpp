#pragma once

#include <cstdint>

//TODO: maybe time to use a namespace

constexpr inline std::uint32_t IO_ADDRESS{ 0x04000000 };
constexpr inline std::uint32_t DISPLAY_CONTROL_ADDRESS{ 0x04000000 };
constexpr inline std::uint32_t DISPLAY_STATUS_ADDRESS{ 0x04000004 };
constexpr inline std::uint32_t DISPLAY_SCANLINE_COUNT_ADDRESS{ 0x04000006 };
constexpr inline std::uint32_t BG_CONTROL_ADDRESS{ 0x04000008 };
constexpr inline std::uint32_t BG_OFFSET_ADDRESS{ 0x04000010 };
constexpr inline std::uint32_t BG_AFFINE_ADDRESS{ 0x04000020 };
constexpr inline std::uint32_t INPUT_ADDRESS{ 0x04000130 };
constexpr inline std::uint32_t INPUT_INTERRUPT_ADDRESS{ 0x04000132 };

// conceptually, both palettes contain 256 entries of Colors (see Color.hpp)
// but with 2 colors packed into every 32 bits for CPU processing speed instead of operating on std::uint16_t
constexpr inline std::uint32_t BACKGROUND_PALETTE_ADDRESS{ 0x05000000 };
constexpr inline std::uint32_t SPRITE_PALETTE_ADDRESS{ 0x05000200 };



constexpr inline std::uint32_t VRAM_ADDRESS{ 0x06000000 }; // in bitmap modes, this is the bitmap that gets drawn on the screen. background tiles start here otherwise
constexpr inline std::uint32_t SPRITE_LOW_BLOCK_ADDRESS{ 0x06010000 }; // block 4, not available in bitmap modes (3-5) because VRAM overlaps this memory
constexpr inline std::uint32_t SPRITE_HIGH_BLOCK_ADDRESS{ 0x06014000 }; // block 5. note that sprite indexing starts from the lower block's start. this holds indices 512-1023



constexpr inline std::uint32_t OAM_ADDRESS{ 0x07000000 };

