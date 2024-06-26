#pragma once

#include "Display/Background.hpp"
#include "Math/Box.hpp"

class Level
{

	//TODO: load up a level asset, which will contain background(s)
	// bounds
	// elevations
	// collision (maybe can enter based on a bit flag of the cardinal directions (and diagonals separately?))
	Box Bounds;
	Background BG; //TODO: construct it in-place to avoid having to std::move

	//TODO: multiple backgrounds/layers
	//TODO: elevation data?
	//TODO: collision data
	
public:
	Level(const Box& InBounds, Background&& InBackground)
		: Bounds(InBounds), BG(std::move(InBackground)) {}

};
