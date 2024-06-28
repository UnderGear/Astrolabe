#pragma once

#include <cstdint>

//TODO: maybe time to use a namespace

// System ROM (BIOS, executable, not readable)
// 0x00000000-0x0003FFF (16kb), 32 bit


// EWRAM (External Work RAM, best place for game data and THUMB code, also where multiboot gets dumped and started)
// 0x02000000-0x0203FFFF (256kb), 16 bit


// IWRAM (Internal Work RAM, fastest available RAM, best place for ARM code)
// 0x03000000-0x03007FFF (32kb), 32 bit
constexpr inline std::uint32_t ISR_MAIN_REGISTER_ADDRESS{ 0x03007FFC };
//TODO: apparently 0x03007F9F is the end of the default interrupt stack


// IO RAM (memory-mapped registers)
// 0x04000000-0x040003FF (1kb), 32 bit
constexpr inline std::uint32_t IO_ADDRESS{ 0x04000000 };
constexpr inline std::uint32_t DISPLAY_CONTROL_ADDRESS{ 0x04000000 };
constexpr inline std::uint32_t DISPLAY_STATUS_ADDRESS{ 0x04000004 };
constexpr inline std::uint32_t DISPLAY_SCANLINE_COUNT_ADDRESS{ 0x04000006 };
constexpr inline std::uint32_t BG_CONTROL_ADDRESS{ 0x04000008 };
constexpr inline std::uint32_t BG_OFFSET_ADDRESS{ 0x04000010 };
constexpr inline std::uint32_t BG_AFFINE_ADDRESS{ 0x04000020 };
constexpr inline std::uint32_t TIMER_0_COUNTER_ADDRESS{ 0x04000100 };
constexpr inline std::uint32_t TIMER_0_CONTROL_ADDRESS{ 0x04000102 };
constexpr inline std::uint32_t TIMER_1_COUNTER_ADDRESS{ 0x04000104 };
constexpr inline std::uint32_t TIMER_1_CONTROL_ADDRESS{ 0x04000106 };
constexpr inline std::uint32_t TIMER_2_COUNTER_ADDRESS{ 0x04000108 };
constexpr inline std::uint32_t TIMER_2_CONTROL_ADDRESS{ 0x0400010A };
constexpr inline std::uint32_t TIMER_3_COUNTER_ADDRESS{ 0x0400010C };
constexpr inline std::uint32_t TIMER_3_CONTROL_ADDRESS{ 0x0400010E };
constexpr inline std::uint32_t INPUT_ADDRESS{ 0x04000130 };
constexpr inline std::uint32_t INPUT_INTERRUPT_ADDRESS{ 0x04000132 };
constexpr inline std::uint32_t INTERRUPT_ENABLE_ADDRESS{ 0x04000200 };
constexpr inline std::uint32_t INTERRUPT_REQUEST_FLAGS_ADDRESS{ 0x04000202 };
constexpr inline std::uint32_t INTERRUPT_MASTER_ENABLE_ADDRESS{ 0x04000208 };


// Palette RAM (background and sprite palette memory)
// 0x05000000-0x050003FF (1kb), 16 bit
// conceptually, both palettes contain 256 entries of Colors (see Color.hpp)
// but with 2 colors packed into every 32 bits for CPU processing speed instead of operating on std::uint16_t
constexpr inline std::uint32_t BACKGROUND_PALETTE_ADDRESS{ 0x05000000 };
constexpr inline std::uint32_t SPRITE_PALETTE_ADDRESS{ 0x05000200 };


// VRAM (frame buffers, tile data, tile maps)
// 0x06000000-0x06017FFF (96kb), 16 bit
constexpr inline std::uint32_t VRAM_ADDRESS{ 0x06000000 }; // in bitmap modes, this is the bitmap that gets drawn on the screen. background tiles start here otherwise
constexpr inline std::uint32_t SPRITE_LOW_BLOCK_ADDRESS{ 0x06010000 }; // block 4, not available in bitmap modes (3-5) because VRAM overlaps this memory
constexpr inline std::uint32_t SPRITE_HIGH_BLOCK_ADDRESS{ 0x06014000 }; // block 5. note that sprite indexing starts from the lower block's start. this holds indices 512-1023


// OAM (Object Attribute Mapping/Memory)
// 0x07000000-0x070003FF (1kb), 32 bit
constexpr inline std::uint32_t OAM_ADDRESS{ 0x07000000 };


// Game Pak ROM
// 0x08000000-?????????? (0-32mb), 16 bit
// size depends on hardware


// Cart RAM
// 0x0E000000-?????????? (0-64kb), 8 bit
// size depends on hardware
// SRAM or Flash ROM, for saving game data. usually 32kb or 64kb
