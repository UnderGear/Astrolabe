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

	//auto TestSprite{ DisplayMode.LoadSprite(isaac_animsuite, isaac_palette) };
	Actor TestActor{ DisplayMode, isaac_animsuite, isaac_palette, Point2D{ static_cast<i24f8_t>((SCREEN_WIDTH / 2)), static_cast<i24f8_t>((SCREEN_HEIGHT / 2)) } };

	auto TestBG{ DisplayMode.LoadBackground(TestBackgroundAsset, TestBackgroundPaletteAsset, TestBackgroundMapAsset) };

	while (true)
	{
		DisplayMode.VSync(); //TODO: I think there's a better way of doing this. maybe an interrupt or something?

		MyInput.Tick();
		//TODO: update actors or whatever we're going to call them

		static constexpr i24f8_t RunMultiplier{ 2.f };

		auto DPadInput{ MyInput.GetDPadInput().GetNormalized() };
		auto IsBDown{ MyInput.IsKeyDown(InputKey::B) };
		if (IsBDown)
			DPadInput *= RunMultiplier;

		//TODO: create a world state, camera, attach camera to the character
		TestActor.Velocity = DPadInput;
		TestBG.MoveOffset(DPadInput);
		TestActor.UpdateInput(DPadInput, IsBDown);
		TestActor.Tick();

		DisplayMode.Tick();

		++CurrentFrame;
	}
}
