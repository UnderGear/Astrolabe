#pragma once

#include <cstdint>

#include "Display/Sprite.hpp"
#include "Math/Point.hpp"
#include "Math/Vector.hpp"

//TODO: these two are very specific to the test character. let's pull this out
enum class AnimationSuite
{
	Idle,
	Run,
	Walk,
};

inline constexpr std::array<std::size_t, 3> AnimationSuiteIndexOffsets
{
	0,
	5,
	10,
};

enum class FacingDirection
{
	South,
	SouthEast,
	East,
	NorthEast,
	North,
	NorthWest,
	West,
	SouthWest,
};

struct FacingData
{
	std::size_t BaseIndex{ 0 };
	bool ShouldFlipHorizontal{ false };
};

inline constexpr std::array<FacingData, 8> FacingArrayIndexing
{
	FacingData{ 0, false }, // South
	FacingData{ 1, false }, // SouthEast
	FacingData{ 2, false }, // East
	FacingData{ 3, false }, // NorthEast
	FacingData{ 4, false }, // North
	FacingData{ 3, true }, // NorthWest
	FacingData{ 2, true }, // West
	FacingData{ 1, true }, // SouthWest
};

//TODO: should I cave and do an ECS like everyone else in the world?
class Actor
{
	Sprite Appearance;
	FacingDirection Facing{ FacingDirection::South };
	AnimationSuite CurrentAnimationSuite{ AnimationSuite::Idle };

public:
	Point2D Position{ Point::Origin };
	Vector2D Velocity{ Vector::Zero };

	explicit Actor(Sprite&& InApperance, Point2D InPosition = Point::Origin)
		: Appearance(std::move(InApperance)), Position(InPosition)
	{
		Appearance.SetPosition(Position);
	}

	void UpdateFacing(const Vector2D& Input)
	{
		auto& Y{ Input.Y };
		auto& X{ Input.X };

		if (Y < 0)
		{
			if (X > 0)
			{
				Facing = FacingDirection::NorthEast;
			}
			else if (X < 0)
			{
				Facing = FacingDirection::NorthWest;
			}
			else
			{
				Facing = FacingDirection::North;
			}
		}
		else if (Y > 0)
		{
			if (X > 0)
			{
				Facing = FacingDirection::SouthEast;
			}
			else if (X < 0)
			{
				Facing = FacingDirection::SouthWest;
			}
			else
			{
				Facing = FacingDirection::South;
			}
		}
		else
		{
			if (X > 0)
			{
				Facing = FacingDirection::East;
			}
			else if (X < 0)
			{
				Facing = FacingDirection::West;
			}
		}
	}

	void Tick()
	{
		//Position += Velocity; //TODO: worry about delta time later
		CurrentAnimationSuite = Velocity == Vector::Zero ? AnimationSuite::Idle : AnimationSuite::Walk;

		auto FacingInfo{ FacingArrayIndexing[static_cast<std::size_t>(Facing)] };
		auto Index{ AnimationSuiteIndexOffsets[static_cast<std::size_t>(CurrentAnimationSuite)] + FacingInfo.BaseIndex };
		//TODO: look up the index to use
		Appearance.SetShouldFlipHorizontal(FacingInfo.ShouldFlipHorizontal);
		Appearance.SetSpriteAnimationIndex(Index);
		
		// Position.X = std::clamp(Position.X, static_cast<std::int32_t>(0), SCREEN_WIDTH);
		// Position.Y = std::clamp(Position.Y, static_cast<std::int32_t>(0), SCREEN_HEIGHT);
		Appearance.SetPosition(Position);
		Appearance.Tick();
	}
};
