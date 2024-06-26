#pragma once

#include <algorithm>

#include "Fixed.hpp"
#include "Point.hpp"

struct Box
{
	// Min values
	Point2D Origin;
	// Max values
	Point2D Extents;

	// Don't require the user to pass in an Origin and Extents - figure it out for them
	constexpr Box(const Point2D& A, const Point2D& B)
	{
		auto [MinX, MaxX] = std::minmax(A.X, B.X);
		auto [MinY, MaxY] = std::minmax(A.Y, B.Y);
		Origin = { MinX, MinY };
		Extents = { MaxX, MaxY };
	}
	
	constexpr bool Contains(const Point2D& TestPoint) const
	{
		return TestPoint.X >= Origin.X && TestPoint.X <= Extents.X
			&& TestPoint.Y >= Origin.Y && TestPoint.Y <= Extents.Y;
	}

	constexpr Box Union(const Box& Other) const
	{
		auto MinX = std::min(Origin.X, Other.Origin.X);
		auto MaxX = std::max(Extents.X, Other.Extents.X);
		auto MinY = std::min(Origin.Y, Other.Origin.Y);
		auto MaxY = std::max(Extents.Y, Other.Extents.Y);

		Point2D Origin{ MinX, MinY };
		Point2D Extents{ MaxX, MaxY };
		return { Origin, Extents };
	}

	// Clamp the given point inside the bounds of the box
	constexpr Point2D ClampPoint(const Point2D& ToClamp) const
	{
		auto X = std::clamp(ToClamp.X, Origin.X, Extents.X);
		auto Y = std::clamp(ToClamp.Y, Origin.Y, Extents.Y);
		return { X, Y };
	}

	constexpr Point2D GetCenter() const
	{
		return { (Origin.X + Extents.X) / static_cast<i24f8_t>(2), (Origin.Y + Extents.Y) / static_cast<i24f8_t>(2) };
	}
};
