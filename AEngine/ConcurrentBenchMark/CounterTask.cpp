#include"CounterTask.h"


CounterTask::CounterTask()
{
    m_MaxCount = 400000;
}

CounterTask::CounterTask(const CounterTask& other)
{
    m_MaxCount = other.m_MaxCount;
}
    
CounterTask::~CounterTask()
{}

Task* CounterTask::GetCountTask()
{
    Task*       newTask = new Task;
    TaskData*   newData = new TaskData;
    newData->parameter1 = &m_MaxCount;
    newData->parameter2 = nullptr;
    newData->parameter3 = nullptr;
    newData->parameter4 = nullptr;
    newData->parameter5 = nullptr;

    newTask->thisPointer    = this;
    newTask->pTaskData      = newData;
    newTask->callback       = nullptr;
    newTask->taskFunction   = DoCount;
    
    return newTask; 
}


void CounterTask::Count(TaskData* pData)
{
    int volatile counter = *(int*)pData->parameter1;
    int steps = 0;
    for(int i = 0; i < counter; ++i)
    {
        steps++;
    }
}

