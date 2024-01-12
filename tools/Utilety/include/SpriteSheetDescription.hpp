#pragma once

#include <filesystem>
#include <vector>

#include <SpriteDimension.hpp>

struct SpriteAnimationSetDescription
{
	std::filesystem::path FilePath;
	int AnimFrameCount;
	int AnimCount;
	std::vector<std::vector<int>> AnimIndices;
	std::vector<int> FrameDurations;
	std::vector<std::string> AnimSuffixes;
};

struct SpritesheetDescription
{
	SpriteDimension SpriteWidth;
	SpriteDimension SpriteHeight;
	int TotalAnimationCount{ 0 };
	std::vector<SpriteAnimationSetDescription> AnimationSetDescriptions;
};
