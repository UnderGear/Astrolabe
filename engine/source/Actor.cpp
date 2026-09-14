#include "Display/Display.hpp"
#include "GameFramework/Actor.hpp"

Actor::Actor(Display& TargetDisplay, const AnimationSuite& AnimSuite, const PaletteAsset& Pal, Circle InCollision)
	: Appearance(TargetDisplay.LoadSprite(AnimSuite, Pal, Attribute0Register::ObjectModeOptions::Affine)), Collision(InCollision)
{
	Appearance.SetPosition(Collision.Center);
}

void Actor::UpdateInput(const Vector2D& Input, bool InIsRunPressed)
{
	auto MovementVelocity{ Input * WalkSpeed };
	if (InIsRunPressed)
	{
		MovementVelocity *= RunMultiplier;
	}

	Velocity = MovementVelocity;

	IsRunPressed = InIsRunPressed;
	auto& Y{ Input.Y };
	auto& X{ Input.X };

	if (Y < 0_i24f8)
	{
		if (X > 0_i24f8)
		{
			Facing = FacingDirection::NorthEast;
		}
		else if (X < 0_i24f8)
		{
			Facing = FacingDirection::NorthWest;
		}
		else
		{
			Facing = FacingDirection::North;
		}
	}
	else if (Y > 0_i24f8)
	{
		if (X > 0_i24f8)
		{
			Facing = FacingDirection::SouthEast;
		}
		else if (X < 0_i24f8)
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
		if (X > 0_i24f8)
		{
			Facing = FacingDirection::East;
		}
		else if (X < 0_i24f8)
		{
			Facing = FacingDirection::West;
		}
	}
}

void Actor::Tick()
{
	Collision.Center += Velocity; //TODO: worry about delta time later

	//TODO: determine if movement is blocked. adjust velocity and position accordingly
	//TODO: maybe a pending move to handle?
}

void Actor::UpdateSprite(const Point2D& RelativePosition)
{
	CurrentAnimationSuite = Velocity == Vector::Zero ? AnimationSuiteType::Idle : (IsRunPressed ? AnimationSuiteType::Run : AnimationSuiteType::Walk);

	auto& FacingInfo{ FacingArrayIndexing[static_cast<std::size_t>(Facing)] };
	auto Index{ AnimationSuiteIndexOffsets[static_cast<std::size_t>(CurrentAnimationSuite)] + FacingInfo.BaseIndex };

	Appearance.SetShouldFlipHorizontal(FacingInfo.ShouldFlipHorizontal);
	Appearance.SetSpriteAnimationIndex(Index);

	Appearance.SetPosition(RelativePosition);
	Appearance.Tick();
}

Point2D Actor::GetPosition() const
{
	return Collision.Center;
}

const Circle &Actor::GetCollision() const
{
	return Collision;
}
