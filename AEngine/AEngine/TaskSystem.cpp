#include"TaskSystem.h"



TaskSystem::TaskSystem()
{
    m_pTaskQueue = nullptr;
    m_pWorkerThreads = nullptr;    
    m_createdThreads = 0;
}

TaskSystem::~TaskSystem()
{}

TaskSystem::TaskSystem(const TaskSystem& other)
{
    this->m_createdThreads = other.m_createdThreads;
    this->m_pWorkerThreads = other.m_pWorkerThreads;
}

bool TaskSystem::Initialize()
{
    m_pTaskQueue = new ConcurrentLLQueue<ITask*>();

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    unsigned int numProcessors = sysInfo.dwNumberOfProcessors;

    //For single core machines one producer and one consumer thread
    if(numProcessors == 1) 
        m_createdThreads = 2;
    else 
    m_createdThreads = numProcessors;
    m_pWorkerThreads = new WorkerThread[m_createdThreads];
    m_pThreadHandles = new HANDLE[m_createdThreads];

    if(m_pWorkerThreads == nullptr) 
    return false;

    for(unsigned int i = 0; i < m_createdThreads; ++i)
    {
        m_pThreadHandles[i] = m_pWorkerThreads[i].StartWorkerThread(i,this);
    }

    return true;
}

void TaskSystem::Shutdown()
{
    DWORD ExitCode = 0;
    for(unsigned int i = 0; i < m_createdThreads; ++i)
    {
    m_pWorkerThreads[i].EndWorkerThread();
    }

    m_pTaskQueue->emptyQueue();

}

void TaskSystem::EnqueueTask(ITask* task)
{
    m_pTaskQueue->Enqueue(task);
}

ITask* TaskSystem::DequeueTask()
{
    return m_pTaskQueue->Dequeue();
}

