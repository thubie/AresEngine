#include"WorkerThread.h"

namespace AresEngine
{
    WorkerThread::WorkerThread()
    {
        m_pTaskManager = nullptr;
        m_threadHandle = nullptr;
        m_pThreadLogger = nullptr;
        m_running = false;
        m_threadID = 0;
        m_exitCode = 0;
    }

    WorkerThread::~WorkerThread()
    {}

    //Starts and creates the thread.
    void WorkerThread::StartWorkerThread(unsigned int threadID,TaskManager* taskManager)
    {
        if(taskManager != nullptr)
            m_pTaskManager = taskManager;

        SetThreadId(threadID);

        m_pThreadLogger = new ThreadLogger(this->m_threadID);
        m_pThreadLogger->OpenLogFile();

        m_threadHandle = (HANDLE)_beginthreadex(NULL,0,&cDoTasks,(void*)this,NULL,NULL);
        SetThreadAffinity();

        m_running = true;
        if(m_threadHandle == nullptr)
        {
            //Need to add some more error checking?
            m_running = false; //terminate the running while loop
        }
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
    DWORD WorkerThread::EndWorkerThread()
    {
        if(m_threadHandle != nullptr)
        {
            m_running = false; //kill off the loop
            WaitForSingleObject(m_threadHandle,INFINITE); //wait for thread to finish up working
            GetExitCodeThread( m_threadHandle, &m_exitCode); //Get exit code for later
            CloseHandle(m_threadHandle); //Closes the thread object
            m_threadHandle = nullptr;
            return m_exitCode;
        }

        if(m_pThreadLogger != nullptr)
        {
            m_pThreadLogger->CloseLogFile();
            delete m_pThreadLogger;
            m_pThreadLogger = nullptr;
        }

        return 0;
    }

    DWORD WorkerThread::DoTasks()
    {
        ITask* currentTask = nullptr;

        while(m_running)
        {
            currentTask = m_pTaskManager->DequeueTask();
            //Check if task is still a nullptr
            if(currentTask == nullptr)
            {
                Sleep(1); //sleep for 1 milisecond and 
            }
            else
            {
                currentTask->RunTask();
            }
            currentTask = nullptr;
            //When done we need to either decide if we want to delete or reuse the task object.
        }
        return 0;
    }
}