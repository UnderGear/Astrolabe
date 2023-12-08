#pragma once

#include "Point.hpp"
#include "Vector.hpp"

struct Rect
{
    Point2D Location{ Point::Origin };
    Vector2D Extents{ Vector::Zero };

    //TODO: get center?
};
