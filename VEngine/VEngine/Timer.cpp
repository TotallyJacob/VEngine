#include "Timer.h"

using namespace vengine;

Timer::Timer()
{
    QueryPerformanceFrequency(&frequency);

    frequencyMs = double(frequency.QuadPart) / 1000.0;
}

void Timer::currentTime(LARGE_INTEGER* value) const
{
    QueryPerformanceCounter(value);
}

void Timer::getMsElapsed(LARGE_INTEGER& current, LARGE_INTEGER& previous, double& elapsed) const
{
    elapsed = double(current.QuadPart - previous.QuadPart) / frequencyMs;
}
void Timer::getMsElapsed(LARGE_INTEGER& previous, double& elapsed) const
{

    LARGE_INTEGER current;
    currentTime(&current);
    getMsElapsed(current, previous, elapsed);
}
