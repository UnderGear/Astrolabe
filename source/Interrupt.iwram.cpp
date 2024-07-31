#include "Interrupt.hpp"

namespace Interrupts
{
	__attribute__((section(".iwram"), long_call))
	void MasterInterrupt()
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
}
