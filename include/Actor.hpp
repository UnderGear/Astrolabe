#pragma once

#include <cstdint>

#include "Display/Sprite.hpp"
#include "Math/Point.hpp"
#include "Math/Vector.hpp"

//TODO: should I cave and do an ECS like everyone else in the world?
class Actor
{
	Sprite Appearance;

public:
	Point2D Position{ Point::Origin };
	Vector2D Velocity{ Vector::Zero };

	explicit Actor(Sprite&& InApperance) : Appearance(std::move(InApperance)) { }

	void Tick(std::int32_t CurrentFrame)
	{
		Position += Velocity; //TODO: worry about delta time later
		
		// Position.X = std::clamp(Position.X, static_cast<std::int32_t>(0), SCREEN_WIDTH);
		// Position.Y = std::clamp(Position.Y, static_cast<std::int32_t>(0), SCREEN_HEIGHT);
		Appearance.SetPosition(Position);
		Appearance.Tick(CurrentFrame);
	}
};
