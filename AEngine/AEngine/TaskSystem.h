#pragma once

#include"WorkerThread.h"
#include"LinkListQueue.h"
#include"RingBufferQueue.h"
//#include"Lock.h"
#include"Tasks.h"
#include"lua_inc.h"


class WorkerThread;

class TaskSystem
{
public:
    static TaskSystem& GetTaskSystem();
    void Initialize(unsigned int MaxThreads);
    void Shutdown();
    void SetNumThreads(unsigned int numThreads); 
    void ResetTaskQueue();
    unsigned int GetNumThreads();
    void StartDistributing();
    void PausedDistributing();
    void ResumeDistributing();
    void EnqueueTask(Task* task);
    Task* DequeueTask();

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
    
    //Exposed to lua
    static int SetNumThreads(lua_State* luaVM);

private:
    TaskSystem(){};
    TaskSystem(TaskSystem const& copy); //Dont implement
    TaskSystem& operator=(TaskSystem const& copy); //Dont Implment;
    ~TaskSystem(){};

private:
    WorkerThread* m_pWorkerThreads;
    unsigned int m_createdThreads;
    unsigned int m_ProcessorsCount;
    LinkListQueue<Task*>* m_pTaskQueue;
    HANDLE* m_pThreadHandles;
    bool m_Distributing;
};

