#include"TaskSystem.h"

TaskSystem::TaskSystem()
{
    m_pTaskQueue        = nullptr;
    m_pWorkerThreads    = nullptr;    
    m_createdThreads    = 0;
    m_Distributing      = false;
}

TaskSystem::~TaskSystem()
{}

void TaskSystem::Initialize(unsigned int MaxThreads)
{
    m_pTaskQueue = new LinkListQueue<Task*>();

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    unsigned int numProcessors = sysInfo.dwNumberOfProcessors;

    //For single core machines one producer and one consumer thread
    if(numProcessors == 1) 
        m_createdThreads = 1; 
        
    if(numProcessors > MaxThreads)
        numProcessors = MaxThreads;

    m_createdThreads = numProcessors;
    m_pWorkerThreads = new WorkerThread[m_createdThreads];
    assert(!(m_pWorkerThreads == nullptr)); 
    m_pThreadHandles = new HANDLE[m_createdThreads];
    assert(!(m_pThreadHandles == nullptr));
    
    for(unsigned int i = 0; i < m_createdThreads; ++i)
    {
        m_pThreadHandles[i] = m_pWorkerThreads[i].Initialize(i,this);
    }
}

void TaskSystem::StartDistributing()
{
    m_Distributing = true;
}

void TaskSystem::PausedDistributing()
{
    m_Distributing = false;
}

void TaskSystem::ResumeDistributing()
{
    m_Distributing = true;
}

void TaskSystem::Shutdown()
{
    for(unsigned int i = 0; i < m_createdThreads; ++i)
    {
        m_pWorkerThreads[i].EndWorkerThread();
    }
}

void TaskSystem::EnqueueTask(Task* task)
{
    m_pTaskQueue->Enqueue(task);
}

Task* TaskSystem::DequeueTask()
{
    return m_pTaskQueue->Dequeue();
}
