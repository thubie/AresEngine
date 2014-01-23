#include"WorkerThread.h"


WorkerThread::WorkerThread()
{
    m_pTaskSystem = nullptr;
    m_threadHandle = nullptr;
    m_running = false;
    m_threadID = 0;
    m_exitCode = 0;
}

WorkerThread::~WorkerThread()
{
    m_pTaskSystem = nullptr;
}

//Starts and creates a thread in suspended mode.
HANDLE WorkerThread::Initialize(unsigned int threadID,TaskSystem* taskManager)
{
    m_pTaskSystem = taskManager;
    m_threadID = threadID;

    m_threadHandle = (HANDLE)_beginthreadex(NULL,0,&DoTasks,(void*)this,NULL,NULL);
    assert(!(m_threadHandle == nullptr));
    //SetThreadAffinity
    DWORD_PTR threadMask = (0x01 << m_threadID); //shift bit to left m_threadID times.
    SetThreadAffinityMask(m_threadHandle,threadMask);
    DWORD_PTR processorId;
    processorId = SetThreadAffinityMask(m_threadHandle,threadMask);
    m_running = true;
       
    return m_threadHandle;
}

//Ends the thread by putting running flag to false.
void WorkerThread::EndWorkerThread()
{
    m_running = false; //kill off the loop
    WaitForSingleObject(m_threadHandle,INFINITE); //wait for thread to finish up working
    GetExitCodeThread(m_threadHandle, &m_exitCode);
    CloseHandle(m_threadHandle); //Closes the thread object
}

DWORD WorkerThread::ExecuteTasks()
{
    Task* currentTask = nullptr;
    
    while(m_running)
    {
        if(!m_pTaskSystem->DistributingTasks())
        {
            Sleep(1);
            continue;
        }

        
        if(currentTask == nullptr)
        {
            Sleep(1);
        }
        else
        {
        #ifdef _DEBUG
            LARGE_INTEGER beginStamp; 
            QueryPerformanceCounter(&beginStamp);
            LOG("Starting Task",m_threadID,100);
        #endif
            /*currentTask = m_pTaskSystem->DequeueTask();
            currentTask.taskFunction(currentTask.pTaskData, currentTask.thisPointer);
            if(currentTask.callback != nullptr)
            {
                currentTask.callback(currentTask.thisPointer, &currentTask);
            }*/

            currentTask->taskFunction(currentTask->pTaskData, currentTask->thisPointer);
            if(currentTask->callback != nullptr)
            {
                currentTask->callback(currentTask->thisPointer,currentTask);
            }
        
        #ifdef _DEBUG
            LARGE_INTEGER endStamp; 
            QueryPerformanceCounter(&endStamp);
            LOG("Finished Task",m_threadID,(DWORD)endStamp.QuadPart - beginStamp.QuadPart);
        #endif  

            //delete currentTask; //put this in the callback function
            currentTask = nullptr;
        }
        currentTask = m_pTaskSystem->DequeueTask();
    }
    return 0;
}
