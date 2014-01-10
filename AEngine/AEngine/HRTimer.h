#pragma once

#include<Windows.h>

class HRTimer
{
public:
    HRTimer();
    ~HRTimer();

    void Start();
    void Stop();

    LONGLONG GetStartCounter();
    LONGLONG GetStopCounter();
    LONGLONG GetCurrentCounter();

    LONGLONG GetElapsed();
    double GetElapsedAsSeconds();
    double GetGameRunTime();

private:
    LONGLONG GetCurrentTick();

private:
    LARGE_INTEGER m_Start, m_Current, m_Stop;
};
