#pragma once

#include <array>
#include <vector>

#include "Actor.hpp"
#include "Display/Background.hpp"
#include "Display/Display.hpp"
#include "Math/Box.hpp"
#include "Math/Circle.hpp"

class World
{
	int FrameNumber{ 0 };

	Display DisplayMode;

	//TODO: spatial partition
	std::vector<Box> Boxes;
	std::vector<Circle> Circles;

	std::vector<Actor> Actors;
	std::vector<Background> Backgrounds;

public:
	static constexpr int FIXED_DELTA{ 60 };
	World()
	{
		Actors.reserve(5);
	}

	Actor& SpawnActor(AnimationSuite AnimSuite, const PaletteAsset& Pal, Circle InCollision)
	{
		return Actors.emplace_back(DisplayMode, AnimSuite, Pal, InCollision);
	}

	Background& LoadBackground(const BackgroundTileAsset& BackgroundAsset, const PaletteAsset& PaletteAsset, const BackgroundMapAsset& MapAsset)
	{
		return Backgrounds.emplace_back(DisplayMode.LoadBackground(BackgroundAsset, PaletteAsset, MapAsset));
	}

	void Tick();
};
