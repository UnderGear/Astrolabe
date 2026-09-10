#include <cstdint>

#include "Hardware/DMA.hpp"

__attribute__((section(".iwram"), long_call))
void DMA::Copy(const void* SourceAddress, void* DestinationAddress, Channel Channel, std::uint16_t Count)
{
    ControlRegister Control
    {
        Count,
        DestinationAdjustmentMode::Increment,
        SourceAdjustmentMode::Increment,
        false,
        ChunkSizeMode::Word,
        TimingMode::Immediate,
        false,
        true
    };

    Transfer(SourceAddress, DestinationAddress, Channel, Control);
}

__attribute__((section(".iwram"), long_call))
void DMA::Transfer(const void* SourceAddress, void* DestinationAddress, Channel Channel, const ControlRegister& ControlParams)
{
    auto& Registers = (*DMA::Registers)[static_cast<std::size_t>(Channel)];
    Registers.Control.Set<ControlRegister::ChunkCount>(0); // Clear out any ongoing transfers
    Registers.SourceAddress = SourceAddress;
    Registers.DestinationAddress = DestinationAddress;

    //TODO:
    // Now we need to stomp the entire register in one instruction. I'd like to know if there's a cleaner way
    *const_cast<ControlRegister*>(&Registers.Control) = ControlParams;
}
