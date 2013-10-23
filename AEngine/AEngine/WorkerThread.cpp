#include"WorkerThread.h"


WorkerThread::WorkerThread()
{
    m_pTaskManager = nullptr;
    m_threadHandle = nullptr;
    m_running = false;
    m_threadID = 0;
    m_exitCode = 0;
}

WorkerThread::~WorkerThread()
{}

//Starts and creates the thread.
HANDLE WorkerThread::StartWorkerThread(unsigned int threadID,TaskManager* taskManager)
{
    if(taskManager != nullptr)
        m_pTaskManager = taskManager;

    SetThreadId(threadID);

    m_threadHandle = (HANDLE)_beginthreadex(NULL,0,&cDoTasks,(void*)this,NULL,NULL);
    SetThreadAffinity();

    m_running = true;
    if(m_threadHandle == nullptr)
    {
        //Need to add some more error checking?
        m_running = false; //terminate the running while loop
    }

    return m_threadHandle;
}

//Set the threadID
void WorkerThread::SetThreadId(unsigned int threadID)
{
    this->m_threadID = threadID;
}

//Set the thread Affinity 
void WorkerThread::SetThreadAffinity()
{
    DWORD_PTR threadMask = (0x01 << m_threadID); //shift bit to left m_threadID times.
    SetThreadAffinityMask(m_threadHandle,threadMask);
    m_ProcessorID = SetThreadAffinityMask(m_threadHandle,threadMask);
}

//Ends the thread by putting running flag to false.
void WorkerThread::EndWorkerThread()
{
        m_running = false; //kill off the loop
        WaitForSingleObject(m_threadHandle,INFINITE); //wait for thread to finish up working
        CloseHandle(m_threadHandle); //Closes the thread object
}

DWORD WorkerThread::DoTasks()
{
    ITask* currentTask = nullptr;

    while(m_running)
    {
        if(currentTask == nullptr)
        {
            Sleep(1);
        }
        else
        {
                
            LOG("Starting Task",m_threadID,100);
            currentTask->RunTask();
            LOG("Finished Task",m_threadID,100);
            delete (CounterTask*) currentTask;
            currentTask = nullptr;
                
        }
        currentTask = m_pTaskManager->DequeueTask();
    }
    return 0;
}
