#include"HRTimer.h"


HRTimer::HRTimer()
{
    m_Start.QuadPart = 0;
    m_Stop.QuadPart = 0;
    m_Current.QuadPart = 0;
    m_Last.QuadPart = 0;
}
    
HRTimer::~HRTimer()
{

}

//Starts the timer
void HRTimer::Start()
{
    QueryPerformanceCounter(&m_Start);
    m_Last = m_Start;
}

//Stops the timer
void HRTimer::Stop()
{
    QueryPerformanceCounter(&m_Stop);
}

//returns the start cycle
LONGLONG HRTimer::GetStartCounter()
{
    return m_Start.QuadPart;
}
 
//returns the current cycle
LONGLONG HRTimer::GetCurrentTick()
{
    QueryPerformanceCounter(&m_Current);
    return m_Current.QuadPart;
}

//returns the Stop cycle
LONGLONG HRTimer::GetStopCounter()
{
    return m_Stop.QuadPart;
}

//returns elapsed as difference between start and stop cycle .
LONGLONG HRTimer::GetElapsed()
{
    return (m_Stop.QuadPart - m_Start.QuadPart);
}

//returns elapsed as difference in seconds between start and stop 
double HRTimer::GetElapsedAsSeconds()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return ((double)GetElapsed() / (double)frequency.QuadPart);
}

//returns the total time this timer has been running in seconds
double HRTimer::GetGameRunTime()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    LONGLONG current = GetCurrentTick();
    double elapsed = (double)current - m_Start.QuadPart; 
    return (elapsed / (double)frequency.QuadPart);
}

//returns the time passed in seconds since the last time this function was called
double HRTimer::GetDeltaTime()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    LONGLONG current = GetCurrentTick();
    LONGLONG elapsed = current - m_Last.QuadPart;
    double result = (elapsed / (double)frequency.QuadPart);
    m_Last.QuadPart = current;
    return result;
}