#include "Actor.hpp"
#include "Display/Display.hpp"

Actor::Actor(Display& TargetDisplay, const AnimationSuite& AnimSuite, const PaletteAsset& Pal, Point2D InPosition)
	: Appearance(TargetDisplay.LoadSprite(AnimSuite, Pal)), Position(InPosition)
{
	Appearance.SetPosition(Position);
}
