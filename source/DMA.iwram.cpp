#include <cstdint>

#include "Hardware/DMA.hpp"

__attribute__((section(".iwram"), long_call))
void DMA::Copy(const void* SourceAddress, void* DestinationAddress, DMAChannel Channel, std::uint32_t Count)
{
    DMAControlRegister ControlRegister
    {
        Count,
        DMADestinationAdjustmentMode::Increment,
        DMASourceAdjustmentMode::Increment,
        false,
        DMAChunkSize::Word,
        DMATimingMode::Immediate,
        false,
        true
    };

    Transfer(SourceAddress, DestinationAddress, Channel, ControlRegister);
}

__attribute__((section(".iwram"), long_call))
void DMA::Transfer(const void* SourceAddress, void* DestinationAddress, DMAChannel Channel, const DMAControlRegister& ControlParams)
{
    auto& Registers = (*DMA::DMARegisters)[static_cast<std::size_t>(Channel)];
    Registers.ControlRegister.ChunkCount = 0; // Clear out any ongoing transfers
    Registers.SourceAddress = SourceAddress;
    Registers.DestinationAddress = DestinationAddress;

    // Now we need to stomp the entire register in one instruction. I'd like to know if there's a cleaner way
    *const_cast<DMAControlRegister*>(&Registers.ControlRegister) = ControlParams;
}
