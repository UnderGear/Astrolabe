#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#include "Hardware/Register.hpp"
#include "MemoryMap.hpp"

namespace Interrupts
{

	struct InterruptMainEnableRegister : Register<std::uint16_t>
	{
		using Enabled = RegisterAccess<std::uint16_t, bool, 0b00000000'00000001>; // set 0 to disable all
	};

	// Also just using raw access on this register
	struct InterruptRegister : Register<std::uint16_t>
	{
	};

	enum class InterruptType
	{
		VBlank,
		HBlank,
		VCount,
		Timer0,
		Timer1,
		Timer2,
		Timer3,
		Serial,
		DMA0,
		DMA1,
		DMA2,
		DMA3,
		Keypad,
		Cartridge,
	};

	static inline std::unique_ptr<volatile InterruptRegister> InterruptEnableRegister
	{
		new(reinterpret_cast<void*>(MemoryMap::INTERRUPT_ENABLE_ADDRESS)) InterruptRegister
	};

	static inline std::unique_ptr<volatile InterruptRegister> InterruptRequestFlagsRegister
	{
		new(reinterpret_cast<void*>(MemoryMap::INTERRUPT_REQUEST_FLAGS_ADDRESS)) InterruptRegister
	};

	static inline std::unique_ptr<volatile InterruptRegister> BIOSFlagsRegister
	{
		new(reinterpret_cast<void*>(MemoryMap::INTERRUPT_BIOS_FLAGS_ADDRESS)) InterruptRegister
	};

	static std::unique_ptr<volatile InterruptMainEnableRegister> InterruptMainEnabledRegister
	{
		new(reinterpret_cast<void*>(MemoryMap::INTERRUPT_MAIN_ENABLE_ADDRESS)) InterruptMainEnableRegister
	};

	__attribute__((section(".iwram"), long_call))
	void MainInterrupt();

	using IRQHandler = void(*)();
	static inline IRQHandler* MainHandler
	{
		new(reinterpret_cast<IRQHandler*>(MemoryMap::INTERRUPT_MAIN_REGISTER_ADDRESS)) IRQHandler{ MainInterrupt }
	};
	
	static constexpr std::uint16_t HandlerCount{ 14 };
	static std::array<std::vector<IRQHandler>, HandlerCount> Handlers;

	inline void MainEnable()
	{
		InterruptMainEnabledRegister->Set<InterruptMainEnableRegister::Enabled>(true);
	}

	inline void MainDisable()
	{
		InterruptMainEnabledRegister->Set<InterruptMainEnableRegister::Enabled>(false);
	}

	inline void EnableInterrupt(InterruptType Type)
	{
		Interrupts::InterruptEnableRegister->GetMutableRaw() |= (1 << static_cast<std::uint16_t>(Type));
	}

	inline void DisableInterrupt(InterruptType Type)
	{
		Interrupts::InterruptEnableRegister->GetMutableRaw() &= ~(1 << static_cast<std::uint16_t>(Type));
	}

	__attribute__((section(".iwram"), long_call))
	void AddHandler(InterruptType Type, IRQHandler Handler);

	__attribute__((section(".iwram"), long_call))
	void RemoveHandler(InterruptType Type, IRQHandler Handler);

	__attribute__((section(".iwram"), long_call))
	void ClearHandlers(InterruptType Type);
};
