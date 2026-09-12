#include "Display/Display.hpp"
#include "GameFramework/Actor.hpp"

Actor::Actor(Display& TargetDisplay, const AnimationSuite& AnimSuite, const PaletteAsset& Pal, Point2D InPosition)
	: Appearance(TargetDisplay.LoadSprite(AnimSuite, Pal, Attribute0Register::ObjectModeOptions::Affine)), Position(InPosition)
{
	Appearance.SetPosition(Position);
}

void Actor::UpdateInput(const Vector2D &Input, bool InIsRunPressed)
{
	IsRunPressed = InIsRunPressed;
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

void Actor::Tick()
{
	Position += Velocity; //TODO: worry about delta time later

	//TODO: determine if movement is blocked. adjust velocity and position accordingly
	//TODO: maybe a pending move to handle?
}

void Actor::UpdateSprite(const Point2D &RelativePosition)
{
	CurrentAnimationSuite = Velocity == Vector::Zero ? AnimationSuiteType::Idle : (IsRunPressed ? AnimationSuiteType::Run : AnimationSuiteType::Walk);

	auto& FacingInfo{ FacingArrayIndexing[static_cast<std::size_t>(Facing)] };
	auto Index{ AnimationSuiteIndexOffsets[static_cast<std::size_t>(CurrentAnimationSuite)] + FacingInfo.BaseIndex };

	Appearance.SetShouldFlipHorizontal(FacingInfo.ShouldFlipHorizontal);
	Appearance.SetSpriteAnimationIndex(Index);

	Appearance.SetPosition(RelativePosition);
	Appearance.Tick();
}
