#include "Display/SpriteManager.hpp"
#include "Hardware/DMA.hpp"

__attribute__((section(".iwram"), long_call))
void SpriteManager::WriteOAM()
{
    // TODO: only copy as much as needed based on the size up to the highest OAM or affine OAM
    DMA::Copy(OAMBufferAddress, reinterpret_cast<void*>(OAM_ADDRESS), DMAChannel::DMA3, 256);
}
