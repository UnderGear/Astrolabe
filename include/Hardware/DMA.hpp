#pragma once

#include <array>
#include <cstdint>
#include <memory>

#include "Hardware/Register.hpp"
#include "MemoryMap.hpp"

namespace DMA
{
	enum class DestinationAdjustmentMode
	{
		Increment, // Increment destination after each transfer
		Decrement, // Decrement destination after each transfer
		Fixed, // Destination is fixed
		Reload, // Increment destination during transfer, reset it afterwards
	};

	enum class SourceAdjustmentMode
	{
		Increment, // Increment source after each transfer
		Decrement, // Decrement source after each transfer
		Fixed, // Source is fixed
	};

	enum class ChunkSizeMode
	{
		HalfWord, // 16b
		Word, // 32b
	};

	enum class TimingMode
	{
		Immediate, // Start immediately (will actually take 2 cycles)
		VBlank, // Starts on VBlank
		HBlank, // Starts on HBlank
		Refresh, // TODO: look up how this works
	};

	struct ControlRegister : Register<std::uint32_t>
	{
		// Number of chunks to transfer
		using ChunkCount = RegisterAccess<std::uint32_t, std::uint16_t, 0b00000000'00000000'11111111'11111111>;

		using DestinationAdjustment = RegisterAccess<std::uint32_t, DestinationAdjustmentMode, 0b00000000'01100000'00000000'00000000>;
		using SourceAdjustment = RegisterAccess<std::uint32_t, SourceAdjustmentMode, 0b00000001'10000000'00000000'00000000>;
		// will cause repeated triggering if using VBlank or HBlank DMATimingMode
		using Repeat = RegisterAccess<std::uint32_t, bool, 0b00000010'00000000'00000000'00000000>;
		using ChunkSize = RegisterAccess<std::uint32_t, ChunkSizeMode, 0b00000100'00000000'00000000'00000000>;
		using Timing = RegisterAccess<std::uint32_t, TimingMode, 0b00110000'00000000'00000000'00000000>;
		using RaiseFinishInterrupt = RegisterAccess<std::uint32_t, bool, 0b01000000'00000000'00000000'00000000>;
		using Enable = RegisterAccess<std::uint32_t, bool, 0b10000000'00000000'00000000'00000000>;

		constexpr explicit ControlRegister() = default;

		constexpr explicit ControlRegister(
			std::uint16_t InChunkCount,
			DestinationAdjustmentMode InDestinationAdjustment,
			SourceAdjustmentMode InSourceAdjustment,
			bool InRepeat,
			ChunkSizeMode InChunkSize,
			TimingMode InTimingMode,
			bool InRaiseInterrupt,
			bool InEnable)
		{
			SetBatch<ChunkCount, DestinationAdjustment, SourceAdjustment, Repeat, ChunkSize, Timing, RaiseFinishInterrupt, Enable>
			(
				InChunkCount, InDestinationAdjustment, InSourceAdjustment, InRepeat, InChunkSize, InTimingMode, InRaiseInterrupt, InEnable
			);
		}
	};

	struct ChannelRegister
	{
		const void* SourceAddress;
		void* DestinationAddress;
		ControlRegister Control;
	};

	enum class Channel
	{
		DMA0, // Highest priority, only usable on internal RAM
		DMA1, // Sound transfer
		DMA2, // Sound transfer
		DMA3, // Low priority, general-purpose copies. Generally used for bitmap or tile data
	};

	constexpr inline std::size_t ChannelCount{ 4 };

	static inline std::unique_ptr<std::array<volatile ChannelRegister, ChannelCount>> Registers
	{
		new(reinterpret_cast<void*>(MemoryMap::DMA_0_ADDRESS)) std::array<volatile ChannelRegister, ChannelCount>
	};

	__attribute__((section(".iwram"), long_call))
	void Copy(const void* SourceAddress, void* DestinationAddress, Channel Channel, std::uint16_t Count);

	__attribute__((section(".iwram"), long_call))
	void Transfer(const void* SourceAddress, void* DestinationAddress, Channel Channel, const ControlRegister& ControlParams);
}
