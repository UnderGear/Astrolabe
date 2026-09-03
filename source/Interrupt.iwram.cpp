#include "Hardware/Interrupt.hpp"

#include "Display/SpriteManager.hpp"

namespace Interrupts
{
	__attribute__((section(".iwram"), long_call))
	void MainInterrupt()
	{
		auto EnabledMask{ *Interrupts::InterruptEnableRegister };
		auto FlagMask{ *Interrupts::InterruptRequestFlagsRegister };

		auto TriggeredMask{ EnabledMask & FlagMask };

		// Set our interrupts as handled
		*Interrupts::InterruptRequestFlagsRegister = FlagMask;
		*Interrupts::BIOSFlagsRegister |= FlagMask;

		for (std::uint16_t i{ 0 }; i < HandlerCount; ++i)
		{
			if (TriggeredMask & (1 << i))
			{
				for (auto* ToCall : Interrupts::Handlers[i])
				{
					ToCall();
				}
			}
		}
	}

	__attribute__((section(".iwram"), long_call))
	void AddHandler(InterruptType Type, IRQHandler Handler)
	{
		auto& Bundle = Handlers[static_cast<std::uint16_t>(Type)].push_back(Handler);
	}

	__attribute__((section(".iwram"), long_call))
	void RemoveHandler(InterruptType Type, IRQHandler Handler)
	{
		auto& Bundle{ Handlers[static_cast<std::uint16_t>(Type)] };
		[[maybe_unused]] auto Removed{ std::remove(Bundle.begin(), Bundle.end(), Handler) };
	}

	__attribute__((section(".iwram"), long_call))
	void ClearHandlers(InterruptType Type)
	{
		Handlers[static_cast<std::uint16_t>(Type)].clear();
	}
}
