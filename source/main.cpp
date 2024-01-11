#include <algorithm>
#include <array>

#include "Actor.hpp"
#include "Assets/isaac_tiles.hpp"
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

	//TODO: come up with some tool for authoring animations and generate this kind of thing
	std::array<Animation, isaac_tiles.size()> Animations
	{
		Animation{  &isaac_tiles[0],  &isaac_tiles[1],  &isaac_tiles[2],  &isaac_tiles[3],  &isaac_tiles[4],  &isaac_tiles[5] }, // look S
		Animation{  &isaac_tiles[6],  &isaac_tiles[7],  &isaac_tiles[8],  &isaac_tiles[9], &isaac_tiles[10], &isaac_tiles[11] }, // look SE
		Animation{ &isaac_tiles[12], &isaac_tiles[13], &isaac_tiles[14], &isaac_tiles[15], &isaac_tiles[16], &isaac_tiles[17] }, // look E
		Animation{ &isaac_tiles[18], &isaac_tiles[19], &isaac_tiles[20], &isaac_tiles[21], &isaac_tiles[22], &isaac_tiles[23] }, // look NE
		Animation{ &isaac_tiles[24], &isaac_tiles[25], &isaac_tiles[26], &isaac_tiles[27], &isaac_tiles[28], &isaac_tiles[29] }, // look N
		Animation{ &isaac_tiles[30], &isaac_tiles[31], &isaac_tiles[32], &isaac_tiles[33], &isaac_tiles[34], &isaac_tiles[35] }, // walk S
		Animation{ &isaac_tiles[36], &isaac_tiles[37], &isaac_tiles[38], &isaac_tiles[39], &isaac_tiles[40], &isaac_tiles[41] }, // walk SE
		Animation{ &isaac_tiles[42], &isaac_tiles[43], &isaac_tiles[44], &isaac_tiles[45], &isaac_tiles[46], &isaac_tiles[47] }, // walk E
		Animation{ &isaac_tiles[48], &isaac_tiles[49], &isaac_tiles[50], &isaac_tiles[51], &isaac_tiles[52], &isaac_tiles[53] }, // walk NE
		Animation{ &isaac_tiles[54], &isaac_tiles[55], &isaac_tiles[56], &isaac_tiles[57], &isaac_tiles[58], &isaac_tiles[59] }, // walk N
		Animation{ &isaac_tiles[60], &isaac_tiles[61], &isaac_tiles[62], &isaac_tiles[63], &isaac_tiles[64], &isaac_tiles[65] }, // run S
		Animation{ &isaac_tiles[66], &isaac_tiles[67], &isaac_tiles[68], &isaac_tiles[69], &isaac_tiles[70], &isaac_tiles[71] }, // run SE
		Animation{ &isaac_tiles[72], &isaac_tiles[73], &isaac_tiles[74], &isaac_tiles[75], &isaac_tiles[76], &isaac_tiles[77] }, // run E
		Animation{ &isaac_tiles[78], &isaac_tiles[79], &isaac_tiles[80], &isaac_tiles[81], &isaac_tiles[82], &isaac_tiles[83] }, // run NE
		Animation{ &isaac_tiles[84], &isaac_tiles[85], &isaac_tiles[86], &isaac_tiles[87], &isaac_tiles[88], &isaac_tiles[89] }, // run N
	};

	std::span<Animation> TestAnims{ Animations.begin(), Animations.end() };
	auto TestSprite{ DisplayMode.LoadSprite(TestAnims, isaac_palette, CurrentFrame) };
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
