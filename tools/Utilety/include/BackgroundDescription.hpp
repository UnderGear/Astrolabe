#pragma once

#include <filesystem>

enum class BackgroundType
{
	Regular,
	Affine,
};

struct BackgroundDescription
{
	std::filesystem::path FilePath;
	BackgroundType BGType;
};
