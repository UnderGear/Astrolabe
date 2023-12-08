#include "Input.hpp"

void Input::Tick()
{
    PreviousInput = CurrentInput;
    // Invert the raw value from the register because 0 is down and 1 is up
    CurrentInput = ~(*InputValues) & KeyMask;

    DPadInput = Vector::Zero;
    //TODO: note that I've flipped the Y axis inputs for screen space
    if (IsKeyDown(InputKey::Up))
    {
        DPadInput.Y -= 1;
    }
    if (IsKeyDown(InputKey::Down))
    {
        DPadInput.Y += 1;
    }
    if (IsKeyDown(InputKey::Left))
    {
        DPadInput.X -= 1;
    }
    if (IsKeyDown(InputKey::Right))
    {
        DPadInput.X += 1;
    }

    //TODO: callbacks for state change?
}

bool Input::IsKeyDown(InputKey Key) const
{
    return (CurrentInput & static_cast<std::uint32_t>(Key)) != 0;
}

bool Input::IsKeyUp(InputKey Key) const
{
    return (CurrentInput & static_cast<std::uint32_t>(Key)) == 0;
}

const Vector2D& Input::GetDPadInput() const
{
    return DPadInput;
}
