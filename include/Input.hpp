#pragma once

#include <cstdint>
#include <memory>

#include "MemoryMap.hpp"
#include "Vector.hpp"

enum class InputKey : std::uint32_t
{
    A =         0b0000000000000001,
    B =         0b0000000000000010,
    Select =    0b0000000000000100,
    Start =     0b0000000000001000,
    Right =     0b0000000000010000,
    Left =      0b0000000000100000,
    Up =        0b0000000001000000,
    Down =      0b0000000010000000,
    RB =        0b0000000100000000,
    LB =        0b0000001000000000,
};

constexpr inline std::uint32_t KeyMask{ 0b0000001111111111 };

using InputRegister = std::uint16_t;

struct InputInterruptRegister
{
    std::uint16_t A : 1;
    std::uint16_t B : 1;
    std::uint16_t Select : 1;
    std::uint16_t Start : 1;
    std::uint16_t Right : 1;
    std::uint16_t Left : 1;
    std::uint16_t Up : 1;
    std::uint16_t Down : 1;
    std::uint16_t RB : 1;
    std::uint16_t LB : 1;
    std::uint16_t Padding : 4;
    std::uint16_t IRQEnable : 1;
    std::uint16_t Comparitor : 1; // 0 OR, 1 AND comparison of all set keys required to raise the interrupt
};

class Input
{
    std::unique_ptr<const volatile InputRegister> InputValues
    {
        new(reinterpret_cast<void*>(INPUT_ADDRESS)) InputRegister{ }
    };

    std::unique_ptr<volatile InputInterruptRegister> InputInterruptControl
    {
        new(reinterpret_cast<void*>(INPUT_INTERRUPT_ADDRESS)) InputInterruptRegister{ }
    };

    std::uint32_t CurrentInput{ 0 };
    std::uint32_t PreviousInput{ 0 };

    Vector2D DPadInput{ Vector::Zero };

public:

    void Tick();

    bool IsKeyDown(InputKey Key) const;

    bool IsKeyUp(InputKey Key) const;

    const Vector2D& GetDPadInput() const;
};
