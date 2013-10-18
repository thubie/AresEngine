#pragma once

#include<Windows.h>

namespace AresEngine
{
    class HRTimer
    {
    public:
        HRTimer();
        ~HRTimer();

        void Start();
        void Stop();

        LONGLONG GetStartCounter();
        LONGLONG GetStopCounter();

        LONGLONG GetElapsed();
        double GetElapsedAsSeconds();

    private:
        LONGLONG GetCurrentTick();

    private:
        LARGE_INTEGER m_Start, m_Current, m_Stop;
    };
}