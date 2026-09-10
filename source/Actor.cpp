#include "Display/Display.hpp"
#include "GameFramework/Actor.hpp"

Actor::Actor(Display& TargetDisplay, const AnimationSuite& AnimSuite, const PaletteAsset& Pal, Point2D InPosition)
	: Appearance(TargetDisplay.LoadSprite(AnimSuite, Pal, Attribute0Register::ObjectModeOptions::Affine)), Position(InPosition)
{
	Appearance.SetPosition(Position);
}
