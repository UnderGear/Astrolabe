#pragma once

class World
{
    int FrameNumber{ 0 };

public:
    static constexpr int FIXED_DELTA{ 60 };

    //TODO: looks like it's time to do something with this


    void Tick();
};
