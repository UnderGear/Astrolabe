#pragma once

#include "Fixed.hpp"
#include "Point.hpp"

struct Circle
{
	Point2D Center;
	i24f8_t Radius;

	constexpr Circle(const Point2D& InCenter, i24f8_t InRadius)
		: Center(InCenter), Radius(InRadius) {}

	constexpr bool Contains(const Point2D& TestPoint) const
	{
		return Center.SquareDistance(TestPoint) <= Radius * Radius;
	}

	constexpr bool Overlaps(const Circle& Other) const
	{
		return Center.SquareDistance(Other.Center) <= Radius * Radius + Other.Radius * Other.Radius;
	}
};
