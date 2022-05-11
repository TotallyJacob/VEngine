#pragma once

#include "GLFW/glfw3.h"

#define NOMINMAX
#include <windows.h>

namespace vengine
{
class Timer
{
    private:

        LARGE_INTEGER ticks, frequency;
        double        frequencyMs;

    public:

        Timer();

        void getMsElapsed(LARGE_INTEGER& current, LARGE_INTEGER& previous, double& elapsed) const;
        void getMsElapsed(LARGE_INTEGER& previous, double& elapsed) const;
        void currentTime(LARGE_INTEGER* value) const;
};
} // namespace vengine
