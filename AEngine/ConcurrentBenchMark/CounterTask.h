#pragma once 

#include<Windows.h>
#include"Tasks.h"

class CounterTask
{
public:
    CounterTask();
    CounterTask(const CounterTask& other);
    ~CounterTask();

    static void DoCount(TaskData* pData, void* thisPointer)
    {
        CounterTask* self = static_cast<CounterTask*>(thisPointer);
        self->Count(pData);
    }

    Task* GetCountTask();

private:
    void Count(TaskData* parameters);

private:
    unsigned long long m_MaxCount;
};
