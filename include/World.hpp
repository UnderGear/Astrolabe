#pragma once

class World
{
    int FrameNumber { 0 };

public:
    static constexpr int FIXED_DELTA{ 60 };



    void Tick();
};
