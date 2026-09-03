#include "Display/Display.hpp"

__attribute__((section(".iwram"), long_call))
void OnVBlank()
{
	SpriteManager::WriteOAM();
}
