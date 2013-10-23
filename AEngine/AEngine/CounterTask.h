#pragma once 

#include"ITask.h"
#include<Windows.h>


class CounterTask : ITask
{
public:
    CounterTask();
    CounterTask(const CounterTask& other);
    ~CounterTask();

    void RunTask();

private:
    unsigned long long m_MaxCount;

};
