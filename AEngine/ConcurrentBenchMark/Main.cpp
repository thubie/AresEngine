#include<stdio.h>
#include"HRTimer.h"
#include"TaskSystem.h"
#include"CounterTask.h"

int main()
{
    const volatile int taskCount    = 10000;
    const unsigned int maxThreads   = 8;
    TaskSystem* pTaskSystem         = new TaskSystem;
    HRTimer* pTimer                 = new HRTimer;
    CounterTask* pCounterTask       = new CounterTask;
    Task** TaskSerial               = new Task*[taskCount];
    double timeSerial               = 0.0;
    double timeConcurrent           = 0.0;
    volatile LONG TasksFinished     = 0;
    
    //ConcurrentTest
    pTaskSystem->Initialize(maxThreads);
    printf("Using %i threads \n", maxThreads);
    for(int i = 0; i < taskCount; ++i)
    {
        pTaskSystem->EnqueueTask(pCounterTask->GetCountTask());
    }

    
    pTimer->Start();
    pTaskSystem->StartDistributing();
    while(TasksFinished < taskCount)
    {
        TasksFinished = pCounterTask->m_FinishedTasks;
    }
    pTaskSystem->Shutdown();
    pTimer->Stop();
    timeConcurrent = pTimer->GetElapsedAsSeconds();
    printf("Concurrent Time: %f seconds\n" , timeConcurrent);

    //Serial test
    for(int i = 0; i < taskCount; ++i)
    {
        TaskSerial[i] = pCounterTask->GetCountTask();
    }   

    pTimer->Start();
    for(int i = 0; i < taskCount; ++i)
    {
        Task* task = TaskSerial[i];
        task->taskFunction(task->pTaskData,task->thisPointer);        
    }
    pTimer->Stop();
    timeSerial = pTimer->GetElapsedAsSeconds();
    printf("Serial time: %f seconds\n",timeSerial);

    //Delete task
    if(TaskSerial != nullptr)
    {
        delete[] TaskSerial;
    }
    
    if(pTaskSystem != nullptr)
    {
        delete pTaskSystem;
    }

    if(pTimer != nullptr)
    {
        delete pTimer;
    }

    printf("Done!!!\n");
    printf("Concurrent is %f times faster.", (timeSerial / timeConcurrent));

    //pause before leaving
    getchar();
    return 0;
}