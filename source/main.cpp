#include <algorithm>
#include <array>

#include "Actor.hpp"
#include "Assets/brin.hpp"
#include "Assets/brin_palette.hpp"
#include "Assets/isaac.hpp"
#include "Assets/isaac_palette.hpp"
#include "Assets/TestBackground.hpp"
#include "BIOS.hpp"
#include "Camera.hpp"
#include "Display/Background.hpp"
#include "Display/Display.hpp"
#include "Display/Sprite.hpp"
#include "Input.hpp"
#include "Interrupt.hpp"
#include "Level.hpp"
#include "Math/Box.hpp"
#include "Math/Fixed.hpp"
#include "Math/Point.hpp"
#include "Math/Vector.hpp"
#include "Random.hpp"

inline constexpr BackgroundTileAsset BrinTilesAsset
{
	std::span<const std::uint32_t>(brinTiles.begin(), brinTiles.end()),
	0x00001,
	RegularBackgroundDimensions::t64xt32
};

inline constexpr BackgroundMapAsset BrinMapAsset
{
	std::span<const std::uint16_t>(brinMap.begin(), brinMap.end()),
	0x00001
};



int main()
{
	Display DisplayMode;

	// Enable interrupts
	Interrupts::MasterEnable();

	Input MyInput;
	Random<std::int32_t> MyRandom{ 5, -1, 1 };
	int CurrentFrame{ 0 };

	constexpr Point2D ScreenCenter{ static_cast<i24f8_t>((SCREEN_WIDTH / 2)), static_cast<i24f8_t>((SCREEN_HEIGHT / 2)) };
	Actor TestActor{ DisplayMode, isaac_animsuite, isaac_palette, ScreenCenter };




	//TODO: bundle the background, level bounds, and level together a little tighter, along the lines of Actor
	auto TestBG{ DisplayMode.LoadBackground(BrinTilesAsset, brin_palette, BrinMapAsset) };
	//auto TestBG{ DisplayMode.LoadBackground(TestBackgroundAsset, TestBackgroundPaletteAsset, TestBackgroundMapAsset) };

	int LevelWidthTiles{ 64 };
	int LevelHeightTiles{ 32 };
	constexpr int TileDimension{ 8 }; //TODO: this will depend on the BackgroundControlRegister::BackgroundSize used and may not even be square
	Box LevelBounds{ Point::Origin, Point2D{ static_cast<i24f8_t>(LevelWidthTiles * TileDimension), static_cast<i24f8_t>(LevelHeightTiles * TileDimension) } };
	Level TestLevel{ LevelBounds, std::move(TestBG) };

	Camera Cam{ LevelBounds, SCREEN_WIDTH, SCREEN_HEIGHT, &TestActor };

	while (true)
	{
		DisplayMode.VSync();

		MyInput.Tick();
		//TODO: update actors or whatever we're going to call them

		static constexpr i24f8_t RunMultiplier{ 2.f };

		auto DPadInput{ MyInput.GetDPadInput().GetNormalized() };
		auto IsBDown{ MyInput.IsKeyDown(InputKey::B) };
		if (IsBDown)
			DPadInput *= RunMultiplier;

		TestActor.Velocity = DPadInput;
		TestActor.UpdateInput(DPadInput, IsBDown);
		TestActor.Tick();

		Cam.Tick();

		//TODO: come up with a cleaner way of updating their render locations
		// maybe different BGs should be able to have different bounds within the level.
		// there would be a little more offset math involved here, or maybe they'd just have their own bounds member
		TestBG.SetOffset(LevelBounds.Origin + (Cam.GetPosition() - Cam.GetDrawOffset()));

		// Backgrounds and sprites have to operate in different spaces, hence the awkwardly different calculation here
		// Really, backgrounds are the weird ones.
		TestActor.UpdateSprite(Cam.GetDrawOffset() + (TestActor.Position - Cam.GetPosition()));

		DisplayMode.Tick();

		++CurrentFrame;
	}
}
