#pragma once

#include <cstdint>
#include <memory>

#include "MemoryMap.hpp"

struct InterruptRegister
{
	std::uint16_t VBlank: 1;
	std::uint16_t HBlank: 1;
	std::uint16_t VCount: 2;
	std::uint16_t Timers: 4; //TODO: maybe split into individual bits per timer or maybe an array?
	std::uint16_t Serial: 1;
	std::uint16_t DMAs: 4; //TODO: again, 4 DMA channels
	std::uint16_t Keypad: 1;
	std::uint16_t Cartridge: 1; // interrupt on removal, yikes
	std::uint16_t Padding: 2{ 0 };
};

struct InterruptMasterEnableRegister
{
	std::uint16_t DisableAll: 1; // set 0 to disable all
};

// ohhh, looks like we need to set the whole register to that single bit when acking an interrupt
// your ISR BETTER do that.


namespace Interrupts
{
	static inline std::unique_ptr<volatile InterruptRegister> InterruptEnableRegister
	{
		new(reinterpret_cast<void*>(INTERRUPT_ENABLE_ADDRESS)) InterruptRegister
	};

	static inline std::unique_ptr<volatile InterruptRegister> InterruptRequestFlagsRegister
	{
		new(reinterpret_cast<void*>(INTERRUPT_REQUEST_FLAGS_ADDRESS)) InterruptRegister
	};

	static std::unique_ptr<volatile InterruptMasterEnableRegister> InterruptMasterEnabledRegister
	{
		new(reinterpret_cast<void*>(INTERRUPT_MASTER_ENABLE_ADDRESS)) InterruptMasterEnableRegister
	};

	using IRQHandlerT = void(*)();

	static inline IRQHandlerT* Handler
	{
		new(reinterpret_cast<IRQHandlerT*>(INTERRUPT_MAIN_REGISTER_ADDRESS)) IRQHandlerT
	};

	static inline void SetIRQHandler(IRQHandlerT InHandler)
	{
		*Handler = InHandler;
	}

};

//TODO: interrupt switchboard instead of just handling the hblank

__attribute__((section(".iwram"), long_call))
void HBlank();
