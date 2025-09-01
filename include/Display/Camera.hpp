#pragma once

#include <cstdint>

#include "Actor.hpp"
#include "Math/Box.hpp"
#include "Math/Point.hpp"

class Camera
{
	Point2D DrawOffset;

	Box Bounds;

	Point2D Position;

	const Actor* Target = nullptr;

public:

	Camera(const Box& LevelBounds, std::int32_t WidthPixels, std::int32_t HeightPixels, const Actor* InTarget = nullptr)
		: DrawOffset({ static_cast<i24f8_t>(WidthPixels / 2), static_cast<i24f8_t>(HeightPixels / 2) })
		, Bounds(
			{ LevelBounds.Origin.X + DrawOffset.X, LevelBounds.Origin.Y + DrawOffset.Y },
			{ LevelBounds.Extents.X - DrawOffset.X, LevelBounds.Extents.Y - DrawOffset.Y })
		, Position(Bounds.GetCenter())
		, Target(InTarget)
	{
		//TODO: validation on level dimensions vs screen dimensions.
		// we may have no solution in some cases, where I guess we should just use a point at screen center
	}

	//TODO: interpolation from current location to target?
	void Tick();

	[[nodiscard]] const Point2D& GetPosition() const
	{
		return Position;
	}

	[[nodiscard]] const Point2D& GetDrawOffset() const
	{
		return DrawOffset;
	}
};
