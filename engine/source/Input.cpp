#include "Hardware/Input.hpp"
#include "Math/Fixed.hpp"

void Input::Tick()
{
	PreviousInput = CurrentInput;
	PreviousDPadInput = DPadInput;

	// Invert the raw value from the register because 0 is down and 1 is up
	CurrentInput = ~(InputValues->GetRaw()) & KeyMask;

	DPadInput = Vector::Zero;
	//TODO: note that I've flipped the Y axis inputs for screen space
	if (IsKeyDown(InputKey::Up))
	{
		--DPadInput.Y;
	}
	if (IsKeyDown(InputKey::Down))
	{
		++DPadInput.Y;
	}
	if (IsKeyDown(InputKey::Left))
	{
		--DPadInput.X;
	}
	if (IsKeyDown(InputKey::Right))
	{
		++DPadInput.X;
	}

	DPadInput.Normalize();

	//TODO: callbacks for state change?
}

bool Input::IsKeyDown(InputKey Key) const
{
	return (CurrentInput & static_cast<std::uint16_t>(Key)) != 0;
}

bool Input::IsKeyUp(InputKey Key) const
{
	return (CurrentInput & static_cast<std::uint16_t>(Key)) == 0;
}

bool Input::WasKeyDown(InputKey Key) const
{
	return (PreviousInput & static_cast<std::uint16_t>(Key)) != 0;
}

bool Input::WasKeyUp(InputKey Key) const
{
	return (PreviousInput & static_cast<std::uint16_t>(Key)) == 0;
}

bool Input::WasKeyPressed(InputKey Key) const
{
	return WasKeyUp(Key) && IsKeyDown(Key);
}

bool Input::WasKeyReleased(InputKey Key) const
{
	return WasKeyDown(Key) && IsKeyUp(Key);
}

Vector2D Input::GetDPadInput() const
{
	return DPadInput;
}

Vector2D Input::GetPreviousDPadInput() const
{
	return PreviousDPadInput;
}
