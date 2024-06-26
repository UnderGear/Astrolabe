#pragma once

#include "BackgroundManager.hpp"
#include "DisplayRegisters.hpp"
#include "Math/Point.hpp"

struct Vector2D;

class Background
{
	BackgroundManager& Owner;
	std::int32_t BackgroundIndex;
	std::int32_t MapBlockIndex;

	RegularBackgroundDimensions Dimensions;

	volatile BackgroundControlRegister& ControlRegister;
	volatile BackgroundOffset& Offset;

	Point2D ScreenOffset{ Point::Origin };

public:
	explicit Background(BackgroundManager& InOwner,
		std::int32_t InBackgroundIndex,
		std::int32_t InMapBlockIndex,
		RegularBackgroundDimensions InDimensions,
		volatile BackgroundControlRegister& InControlRegister,
		volatile BackgroundOffset& InOffset);

	~Background();

	void MoveOffset(const Vector2D& MoveAmount);
	void SetOffset(const Point2D& NewScreenOffset);
};
