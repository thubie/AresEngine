#pragma once

#include"WorkerThread.h"
#include"LinkListQueue.h"
#include"RingBufferQueue.h"
#include"Lock.h"
#include"Tasks.h"


class WorkerThread;

class TaskSystem
{
public:
    TaskSystem();
    ~TaskSystem();
    
    void Initialize(unsigned int MaxThreads);    
    void Shutdown();
    void StartDistributing();
    void PausedDistributing();
    void ResumeDistributing();
    void EnqueueTask(Task* task);
    Task DequeueTask();

    inline bool QueueIsEmpty()
    {
        return m_pTaskQueue->QueueIsEmpty();
    }

    inline unsigned int GetTaskCount()
    {
        return m_pTaskQueue->GetCount();
    }

    inline bool DistributingTasks()
    {
        return m_Distributing;
    }

private:
    WorkerThread* m_pWorkerThreads;
    unsigned int m_createdThreads;
    unsigned int m_ProcessorsCount;
    //LinkListQueue<Task*>* m_pTaskQueue;
    LinkListQueue<Task>* m_pTaskQueue;
    HANDLE* m_pThreadHandles;
    bool m_Distributing;
};

