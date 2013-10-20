#include"CounterTask.h"

namespace AresEngine
{
    CounterTask::CounterTask()
    {
        m_MaxCount = 400000000000;
    }

    CounterTask::CounterTask(const CounterTask& other)
    {
        m_MaxCount = other.m_MaxCount;
    }
    
    CounterTask::~CounterTask()
    {}

    void CounterTask:: RunTask()
    {
        unsigned long long counter = 0;
        for(unsigned long long i = 0; i < m_MaxCount; ++i)
        {
            counter += 1;
        }
    }
}


