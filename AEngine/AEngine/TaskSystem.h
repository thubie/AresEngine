#pragma once

#include"WorkerThread.h"
#include"ConcurrentLLQueue.h"
#include"ConcurrentCBQueue.h"
#include"CSLock.h"
#include"ITask.h"


class WorkerThread;

class TaskSystem
{
public:
    TaskSystem();
    ~TaskSystem();
    TaskSystem(const TaskSystem& other);
        
    unsigned int GetNodeCount();
    bool Initialize();
    void Shutdown();

    void EnqueueTask(ITask* task);
    ITask* DequeueTask();

private:
    WorkerThread* m_pWorkerThreads;
    unsigned int m_createdThreads;
    unsigned int m_ProcessorsCount;
    ConcurrentLLQueue<ITask*>* m_pTaskQueue;
    HANDLE* m_pThreadHandles;
};

