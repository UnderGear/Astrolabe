#pragma once

#include "BackgroundManager.hpp"
#include "DisplayRegisters.hpp"
#include "Math/Point.hpp"

#include <utility>

struct Vector2D;

class Background
{
	BackgroundManager* Owner;
	std::int32_t BackgroundIndex{ BackgroundManager::INDEX_INVALID };
	std::int32_t MapBlockIndex{ BackgroundManager::INDEX_INVALID };

	BackgroundControlRegister::RegularBackgroundDimensions Dimensions;

	volatile BackgroundControlRegister* ControlRegister;
	volatile BackgroundOffset* Offset;

	Point2D ScreenOffset{ Point::Origin };

public:
	explicit Background(BackgroundManager& InOwner,
		std::int32_t InBackgroundIndex,
		std::int32_t InMapBlockIndex,
		BackgroundControlRegister::RegularBackgroundDimensions InDimensions,
		volatile BackgroundControlRegister& InControlRegister,
		volatile BackgroundOffset& InOffset);

	Background(Background&&);
	Background& operator =(Background&&);

	Background(const Background&) = delete;
	Background& operator =(const Background&) = delete;

	~Background();

	void MoveOffset(const Vector2D& MoveAmount);
	void SetOffset(const Point2D& NewScreenOffset);

	std::pair<int, int> GetDimensions() const;
};
