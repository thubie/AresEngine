#include"HRTimer.h"


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
    
LONGLONG HRTimer::GetCurrentTick()
{
    QueryPerformanceCounter(&m_Current);
    return m_Current.QuadPart;
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

double HRTimer::GetGameRunTime()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    LONGLONG current = GetCurrentTick();
    double elapsed = current - m_Start.QuadPart; 
    return (elapsed / (double)frequency.QuadPart);
}
