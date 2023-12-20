#pragma once

#include "BackgroundManager.hpp"
#include "DisplayRegisters.hpp"

class Background
{
	BackgroundManager& Owner;
	std::int32_t BackgroundIndex;
	std::int32_t MapBlockIndex;

    std::int32_t PaletteAssetIndex;

	RegularBackgroundDimensions Dimensions;

	BackgroundControlRegister& ControlRegister;
	BackgroundOffset& Offset;

public:
	explicit Background(BackgroundManager& InOwner,
		std::int32_t InBackgroundIndex,
		std::int32_t InMapBlockIndex,
    	std::int32_t InPaletteAssetIndex,
		RegularBackgroundDimensions InDimensions,
		BackgroundControlRegister& InControlRegister,
		BackgroundOffset& InOffset);

	~Background();
};
