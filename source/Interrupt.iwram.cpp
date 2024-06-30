#include "Interrupt.hpp"

__attribute__((section(".iwram"), long_call))
void HBlank()
{
	//TODO: set that entire interrupt handled register to one hot with our specific bit high?
	//*(reinterpret_cast<std::uint16_t*>(INTERRUPT_REQUEST_FLAGS_ADDRESS)) = 1;
	Interrupts::InterruptRequestFlagsRegister->HBlank = 1;
}
