#pragma once

#include "BackgroundManager.hpp"
#include "DisplayRegisters.hpp"

class Background
{
	BackgroundManager& Owner;
	std::int32_t BackgroundIndex;
	std::int32_t MapBlockIndex;

	RegularBackgroundDimensions Dimensions;

	volatile BackgroundControlRegister& ControlRegister;
	volatile BackgroundOffset& Offset;

public:
	explicit Background(BackgroundManager& InOwner,
		std::int32_t InBackgroundIndex,
		std::int32_t InMapBlockIndex,
		RegularBackgroundDimensions InDimensions,
		volatile BackgroundControlRegister& InControlRegister,
		volatile BackgroundOffset& InOffset);

	~Background();
};
