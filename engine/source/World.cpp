#include "GameFramework/World.hpp"

void World::Tick()
{
	for (auto& ToTick : Actors)
	{
		ToTick.Tick();
	}
}
