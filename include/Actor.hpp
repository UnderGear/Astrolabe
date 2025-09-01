#pragma once

#include <cstdint>

#include "Display/Sprite.hpp"
#include "Math/Point.hpp"
#include "Math/Vector.hpp"

class Display;

//TODO: these two are very specific to the test character. let's pull this out
enum class AnimationSuiteType
{
	Idle,
	Walk,
	Run,
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
	AnimationSuiteType CurrentAnimationSuite{ AnimationSuiteType::Idle };
	bool IsRunPressed{ false };

public:
	Point2D Position{ Point::Origin };
	Vector2D Velocity{ Vector::Zero };

	explicit Actor(Display& TargetDisplay, const AnimationSuite& AnimSuite, const PaletteAsset& Pal, Point2D InPosition = Point::Origin);

	void UpdateInput(const Vector2D& Input, bool InIsRunPressed)
	{
		IsRunPressed = InIsRunPressed;
		auto& Y{ Input.Y };
		auto& X{ Input.X };

		constexpr i24f8_t Zero{ 0 };
		if (Y < Zero)
		{
			if (X > Zero)
			{
				Facing = FacingDirection::NorthEast;
			}
			else if (X < Zero)
			{
				Facing = FacingDirection::NorthWest;
			}
			else
			{
				Facing = FacingDirection::North;
			}
		}
		else if (Y > Zero)
		{
			if (X > Zero)
			{
				Facing = FacingDirection::SouthEast;
			}
			else if (X < Zero)
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
			if (X > Zero)
			{
				Facing = FacingDirection::East;
			}
			else if (X < Zero)
			{
				Facing = FacingDirection::West;
			}
		}
	}

	void Tick()
	{
		Position += Velocity; //TODO: worry about delta time later

		//TODO: determine if movement is blocked. adjust velocity and position accordingly
		//TODO: maybe a pending move to handle?
	}

	void UpdateSprite(const Point2D& RelativePosition)
	{
		CurrentAnimationSuite = Velocity == Vector::Zero ? AnimationSuiteType::Idle : (IsRunPressed ? AnimationSuiteType::Run : AnimationSuiteType::Walk);

		auto& FacingInfo{ FacingArrayIndexing[static_cast<std::size_t>(Facing)] };
		auto Index{ AnimationSuiteIndexOffsets[static_cast<std::size_t>(CurrentAnimationSuite)] + FacingInfo.BaseIndex };

		Appearance.SetShouldFlipHorizontal(FacingInfo.ShouldFlipHorizontal);
		Appearance.SetSpriteAnimationIndex(Index);

		Appearance.SetPosition(RelativePosition);
		Appearance.Tick();
	}
};
