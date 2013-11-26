#pragma once 

#include<Windows.h>
#include<intrin.h>
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

    static void FinishedCount(void* thisPointer, void* task)
    {
        CounterTask* self = static_cast<CounterTask*>(thisPointer);
        self->FinishedCountTask(task);
    }

    Task* GetCountTask();

private:
    void Count(TaskData* parameters);
    void FinishedCountTask(void* task);

private:
    unsigned long long m_MaxCount;
public:
    LONG m_FinishedTasks;
};
