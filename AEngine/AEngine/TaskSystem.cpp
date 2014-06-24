#include"TaskSystem.h"
#include"AEngine.h"

TaskSystem& TaskSystem::GetTaskSystem()
{
    //The only instance
    //Guaranteed to be lazy initialized
    //Guaranteed that it will be destroyed correctly
    static TaskSystem instance;
    return instance;
}

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

void TaskSystem::ResetTaskQueue()
{
    this->m_pTaskQueue->ResetQueue();
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

void TaskSystem::SetNumThreads(unsigned int numThreads)
{
    //Shutdown currently running threads..
    for(unsigned int i = 0; i < m_createdThreads; ++i)
    {
        m_pWorkerThreads[i].EndWorkerThread();
    }
    if(m_pWorkerThreads != nullptr)
        delete[] m_pWorkerThreads;

    if(m_pThreadHandles != nullptr)
        delete[] m_pThreadHandles;

    
    m_pWorkerThreads = new WorkerThread[numThreads];
    assert(!(m_pWorkerThreads == nullptr)); 
    m_pThreadHandles = new HANDLE[numThreads];
    assert(!(m_pThreadHandles == nullptr));
    
    for(unsigned int i = 0; i < numThreads; ++i)
    {
        m_pThreadHandles[i] = m_pWorkerThreads[i].Initialize(i,this);
    }
    
    this->m_createdThreads = numThreads;
}

unsigned int TaskSystem::GetNumThreads()
{
    return this->m_createdThreads;
}

//////////////////////////////////////////////////
//Lua Tasksystem exposed functions
//////////////////////////////////////////////////
int TaskSystem::SetNumThreads(lua_State* luaVM)
{
    int sizeStack = lua_gettop(luaVM);
    Sleep(200); //Need to really find a way to 
    if(!lua_islightuserdata(luaVM, -2))
    {
        //check if stack element is userdata
        //print error message to Visual studio debug output window.
        bool test = true;
    }
    unsigned int numThreads = lua_tointeger(luaVM,-1);
    TaskSystem* system = (TaskSystem*)lua_touserdata(luaVM, -2);
    system->SetNumThreads(numThreads);
    return 0;
}


