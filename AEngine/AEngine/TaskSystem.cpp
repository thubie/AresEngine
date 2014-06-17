#include"TaskSystem.h"
#include"AEngine.h"

TaskSystem::~TaskSystem()
{}

void TaskSystem::Initialize(unsigned int MaxThreads)
{
    m_pTaskQueue        = nullptr;
    m_pWorkerThreads    = nullptr;    
    m_createdThreads    = 0;
    m_Distributing      = false;
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

    if(m_pTaskQueue != nullptr)
    {
        m_pTaskQueue->EmptyQueue();
        delete m_pTaskQueue;
    }

    if(m_pWorkerThreads != nullptr)
        delete[] m_pWorkerThreads;

    if(m_pThreadHandles != nullptr)
        delete[] m_pThreadHandles;
}

void TaskSystem::EnqueueTask(Task* pTask)
{
    m_pTaskQueue->Enqueue(pTask);
}

Task* TaskSystem::DequeueTask()
{
    return m_pTaskQueue->Dequeue();
}

int TaskSystem::SetNumWorkers(lua_State* pLua)
{
    int error = 0;
    int numWorkers = 0;
    error = lua_gettop(pLua);
    numWorkers = luaL_checkint(pLua,1);
    TaskSystem *ud = static_cast<TaskSystem*>(luaL_checkudata(pLua, LUA_GLOBALSINDEX, "TaskSystem"));  
    return 0;
}





