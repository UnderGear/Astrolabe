#pragma once

#include <array>
#include <cstdint>
#include <memory>

#include "MemoryMap.hpp"

enum class DMADestinationAdjustmentMode
{
    Increment, // Increment destination after each transfer
    Decrement, // Decrement destination after each transfer
    Fixed, // Destination is fixed
    Reload, // Increment destination during transfer, reset it afterwards
};

enum class DMASourceAdjustmentMode
{
    Increment, // Increment source after each transfer
    Decrement, // Decrement source after each transfer
    Fixed, // Source is fixed
};

enum class DMAChunkSize
{
    HalfWord, // 16b
    Word, // 32b
};

enum class DMATimingMode
{
    Immediate, // Start immediately (will actually take 2 cycles)
    VBlank, // Starts on VBlank
    HBlank, // Starts on HBlank
    Refresh, // TODO: look up how this works
};

struct DMAControlRegister
{
    std::uint32_t ChunkCount : 16{ 0 }; // Number of chunks to transfer
    std::uint32_t Padding0 : 5{ 0 };
    std::uint32_t DestinationAdjustment : 2{ 0 }; // DMADestinationAdjustmentMode
    std::uint32_t SourceAdjustment : 2{ 0 }; // DMASourceAdjustmentMode
    std::uint32_t Repeat : 1{ 0 }; // 1 will cause repeated triggering if using VBlank or HBlank DMATimingMode
    std::uint32_t ChunkSize : 1{ 1 }; // DMAChunkSize
    std::uint32_t Padding1 : 1{ 0 };
    std::uint32_t TimingMode : 2{ 0 }; // DMATimingMode
    std::uint32_t RaiseInterrupt : 1{ 0 }; // Raise an interrupt on finished if 1
    std::uint32_t Enable : 1{ 0 }; // Enable/disable DMA on the channel

    constexpr explicit DMAControlRegister() = default;

    constexpr explicit DMAControlRegister(
        std::uint32_t InChunkCount,
        DMADestinationAdjustmentMode InDestinationAdjustment,
        DMASourceAdjustmentMode InSourceAdjustment,
        bool InRepeat,
        DMAChunkSize InChunkSize,
        DMATimingMode InTimingMode,
        bool InRaiseInterrupt,
        bool InEnable)
        : ChunkCount(InChunkCount)
        , DestinationAdjustment(static_cast<std::uint32_t>(InDestinationAdjustment))
        , SourceAdjustment(static_cast<std::uint32_t>(InSourceAdjustment))
        , Repeat(static_cast<std::uint32_t>(InRepeat))
        , ChunkSize(static_cast<std::uint32_t>(InChunkSize))
        , TimingMode(static_cast<std::uint32_t>(InTimingMode))
        , RaiseInterrupt(static_cast<std::uint32_t>(InRaiseInterrupt))
        , Enable(static_cast<std::uint32_t>(InEnable))
	{ }
};

struct DMARegister
{
    const void* SourceAddress;
    void* DestinationAddress;
    DMAControlRegister ControlRegister;
};

enum class DMAChannel
{
    DMA0, // Highest priority, only usable on internal RAM
    DMA1, // Sound transfer
    DMA2, // Sound transfer
    DMA3, // Low priority, general-purpose copies. Generally used for bitmap or tile data
};

namespace DMA
{
    static constexpr std::size_t DMAChannelCount{ 4 };

    static inline std::unique_ptr<std::array<volatile DMARegister, DMAChannelCount>> DMARegisters
	{
		new(reinterpret_cast<void*>(DMA_0_ADDRESS)) std::array<volatile DMARegister, DMAChannelCount>
	};

    __attribute__((section(".iwram"), long_call))
    void Copy(const void* SourceAddress, void* DestinationAddress, DMAChannel Channel, std::uint32_t Count);

    __attribute__((section(".iwram"), long_call))
    void Transfer(const void* SourceAddress, void* DestinationAddress, DMAChannel Channel, const DMAControlRegister& ControlParams);
}
