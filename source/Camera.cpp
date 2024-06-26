#include "Camera.hpp"

void Camera::Tick()
{
	if (Target != nullptr)
	{
		Position = Bounds.ClampPoint(Target->Position);
	}
}
