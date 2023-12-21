#include "Actor.hpp"
#include "Assets/MetroidSprite.hpp"
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

	//TODO: I don't like the heap allocation here. what else can we pass through?
	std::vector<const SpriteTileAsset*> MetroidAssets{ &MetroidTileAsset, &MetroidBoxTileAsset };
	//TODO: I'm considering switching to a reference counted pointer for the return of LoadSprite
	auto MetroidSprite{ DisplayMode.LoadSprite(MetroidAssets, MetroidPaletteAsset, CurrentFrame) };
	Actor Metroid{ std::move(MetroidSprite), Point2D{ static_cast<i24f8_t>((SCREEN_WIDTH / 2)), static_cast<i24f8_t>((SCREEN_HEIGHT / 2)) } };

	auto TestBG{ DisplayMode.LoadBackground(TestBackgroundAsset, TestBackgroundPaletteAsset, TestBackgroundMapAsset) };

	while (true)
	{
		DisplayMode.VSync(); //TODO: I think there's a better way of doing this. maybe an interrupt or something?

		MyInput.Tick();
		//TODO: update actors or whatever we're going to call them
		//Metroid.Velocity = MyInput.GetDPadInput();
		TestBG.MoveOffset(MyInput.GetDPadInput());
		Metroid.Tick(CurrentFrame);

		DisplayMode.Tick();

		++CurrentFrame;
	}
}
