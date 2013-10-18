#include"HRTimer.h"

namespace AresEngine
{
    HRTimer::HRTimer()
    {
        m_Start.QuadPart = m_Stop.QuadPart = 0;
    }
    
    HRTimer::~HRTimer()
    {

    }

    //Start the timer
    void HRTimer::Start()
    {
        QueryPerformanceCounter(&m_Start);
    }

    void HRTimer::Stop()
    {
        QueryPerformanceCounter(&m_Stop);
    }

    LONGLONG HRTimer::GetStartCounter()
    {
        return m_Start.QuadPart;
    }
    
    LONGLONG HRTimer::GetStopCounter()
    {
        return m_Stop.QuadPart;
    }

    LONGLONG HRTimer::GetElapsed()
    {
        return (m_Stop.QuadPart - m_Start.QuadPart);
    }
    
    double HRTimer::GetElapsedAsSeconds()
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        return ((double)GetElapsed() / (double)frequency.QuadPart);
    }
}