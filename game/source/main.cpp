#include <algorithm>
#include <array>
#include <cstdint>

#include "Display/Background.hpp"
#include "Display/Display.hpp"
#include "Display/Sprite.hpp"
#include "GameFramework/Actor.hpp"
#include "GameFramework/Camera.hpp"
#include "GameFramework/Level.hpp"
#include "GameFramework/World.hpp"
#include "Hardware/BIOS.hpp"
#include "Hardware/Input.hpp"
#include "Hardware/Interrupt.hpp"
#include "Math/Box.hpp"
#include "Math/Circle.hpp"
#include "Math/Fixed.hpp"
#include "Math/Point.hpp"
#include "Math/Random.hpp"
#include "Math/Vector.hpp"

#include "assets/brin.hpp"
#include "assets/brin_palette.hpp"
#include "assets/isaac.hpp"
#include "assets/jenna.hpp"
#include "assets/sprite_palette_palette.hpp"

int main()
{
	// Enable interrupts
	Interrupts::MainEnable();

	World TestWorld;
	auto& TestStationary{ TestWorld.SpawnActor(isaac_animsuite, sprite_palette_palette, Circle{ Display::SCREEN_CENTER, 25_i24f8 }) };
	auto& TestActor{ TestWorld.SpawnActor(jenna_animsuite, sprite_palette_palette, Circle{ Display::SCREEN_CENTER + Vector2D{ 50_i24f8, 50_i24f8 }, 25_i24f8}) };

	Input MyInput;
	Random<std::int32_t> MyRandom{ 5, -1, 1 };
	[[maybe_unused]]std::uint32_t CurrentFrame{ 0 };

	//TODO: bundle the background, level bounds, and level together a little tighter, along the lines of Actor. that should also call the appropriate dtor/tear down logic
	auto& TestBG{ TestWorld.LoadBackground(brin_tiles, brin_palette, brin_map) };

	auto [LevelWidthTiles, LevelHeightTiles]{ TestBG.GetDimensions() };
	constexpr std::int32_t TileDimension{ 8 }; //TODO: this will depend on the BackgroundControlRegister::BackgroundSize used and may not even be square
	Box LevelBounds{ Point::Origin, Point2D{ LevelWidthTiles * TileDimension, LevelHeightTiles * TileDimension } };
	Level TestLevel{ LevelBounds, std::move(TestBG) }; //TODO: this constructor should be rethought to avoid move semantics

	Camera Cam{ LevelBounds, Display::SCREEN_WIDTH, Display::SCREEN_HEIGHT, &TestActor };

	while (true)
	{
		BIOS::VBlankWait();

		MyInput.Tick();
		//TODO: update actors or whatever we're going to call them

		//TODO: this kind of input logic handling needs to live in some dedicated controller or something
		auto DPadInput{ MyInput.GetDPadInput() };
		auto IsBDown{ MyInput.IsKeyDown(InputKey::B) };
		TestActor.UpdateInput(DPadInput, IsBDown);

		TestWorld.Tick();
		Cam.Tick();

		//TODO: come up with a cleaner way of updating their render locations
		// maybe different BGs should be able to have different bounds within the level.
		// there would be a little more offset math involved here, or maybe they'd just have their own bounds member
		TestBG.SetOffset(LevelBounds.Origin + (Cam.GetPosition() - Cam.GetDrawOffset()));

		// Backgrounds and sprites have to operate in different spaces, hence the awkwardly different calculation here
		// Really, backgrounds are the weird ones.
		TestActor.UpdateSprite(Cam.GetDrawOffset() + (TestActor.GetPosition() - Cam.GetPosition()));
		TestStationary.UpdateSprite(Cam.GetDrawOffset() + (TestStationary.GetPosition() - Cam.GetPosition()));

		++CurrentFrame;
	}
}
