#include <algorithm>
#include <array>

#include "Actor.hpp"
#include "Assets/isaac.hpp"
#include "Assets/isaac_palette.hpp"
#include "Assets/TestBackground.hpp"
#include "Display/Background.hpp"
#include "Display/Display.hpp"
#include "Display/Sprite.hpp"
#include "Input.hpp"
#include "Math/Fixed.hpp"
#include "Math/Point.hpp"
#include "Math/Vector.hpp"
#include "Random.hpp"

int main()
{
	Display DisplayMode;
	Input MyInput;
	Random<std::int32_t> MyRandom{ 5, -1, 1 };
	int CurrentFrame{ 0 };

	std::span<const Animation> TestAnims{ isaac_anims.begin(), isaac_anims.end() };
	auto TestSprite{ DisplayMode.LoadSprite(TestAnims, isaac_palette) };
	Actor TestActor{ std::move(TestSprite), Point2D{ static_cast<i24f8_t>((SCREEN_WIDTH / 2)), static_cast<i24f8_t>((SCREEN_HEIGHT / 2)) } };

	auto TestBG{ DisplayMode.LoadBackground(TestBackgroundAsset, TestBackgroundPaletteAsset, TestBackgroundMapAsset) };

	while (true)
	{
		DisplayMode.VSync(); //TODO: I think there's a better way of doing this. maybe an interrupt or something?

		MyInput.Tick();
		//TODO: update actors or whatever we're going to call them
		TestActor.Velocity = MyInput.GetDPadInput();
		TestBG.MoveOffset(MyInput.GetDPadInput());
		TestActor.UpdateFacing(MyInput.GetDPadInput());
		TestActor.Tick();

		DisplayMode.Tick();

		++CurrentFrame;
	}
}
