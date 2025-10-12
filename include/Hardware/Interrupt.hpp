#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#include "MemoryMap.hpp"

namespace Interrupts
{
	struct InterruptMasterEnableRegister
	{
		std::uint16_t Enabled: 1; // set 0 to disable all
	};

	// It seems a bit easier to do it this way than with a bit field struct
	using InterruptRegister = std::uint16_t;

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
		new(reinterpret_cast<void*>(INTERRUPT_ENABLE_ADDRESS)) InterruptRegister
	};

	static inline std::unique_ptr<volatile InterruptRegister> InterruptRequestFlagsRegister
	{
		new(reinterpret_cast<void*>(INTERRUPT_REQUEST_FLAGS_ADDRESS)) InterruptRegister
	};

	static inline std::unique_ptr<volatile InterruptRegister> BIOSFlagsRegister
	{
		new(reinterpret_cast<void*>(INTERRUPT_BIOS_FLAGS_ADDRESS)) InterruptRegister
	};

	static std::unique_ptr<volatile InterruptMasterEnableRegister> InterruptMasterEnabledRegister
	{
		new(reinterpret_cast<void*>(INTERRUPT_MASTER_ENABLE_ADDRESS)) InterruptMasterEnableRegister
	};

	__attribute__((section(".iwram"), long_call))
	void MasterInterrupt();

	using IRQHandler = void(*)();
	static inline IRQHandler* Handler
	{
		new(reinterpret_cast<IRQHandler*>(INTERRUPT_MAIN_REGISTER_ADDRESS)) IRQHandler{ MasterInterrupt }
	};

	static constexpr std::uint16_t HandlerCount{ 14 };
	static inline std::array<std::vector<IRQHandler>, HandlerCount> Handlers;

	inline void MasterEnable()
	{
		InterruptMasterEnabledRegister->Enabled = 1;
	}

	inline void MasterDisable()
	{
		InterruptMasterEnabledRegister->Enabled = 0;
	}

	inline void EnableInterrupt(InterruptType Type)
	{
		*Interrupts::InterruptEnableRegister |= (1 << static_cast<std::uint16_t>(Type));
	}

	inline void DisableInterrupt(InterruptType Type)
	{
		*Interrupts::InterruptEnableRegister &= ~(1 << static_cast<std::uint16_t>(Type));
	}

	inline void AddHandler(InterruptType Type, IRQHandler Handler)
	{
		auto& Bundle = Handlers[static_cast<std::uint16_t>(Type)];
		Bundle.push_back(Handler);
	}

	inline void RemoveHandler(InterruptType Type, IRQHandler Handler)
	{
		auto& Bundle{ Handlers[static_cast<std::uint16_t>(Type)] };
		[[maybe_unused]] auto Removed{ std::remove(Bundle.begin(), Bundle.end(), Handler) };
	}

	inline void ClearHandlers(InterruptType Type)
	{
		Handlers[static_cast<std::uint16_t>(Type)].clear();
	}
};
