#include"WorkerThread.h"

namespace AresEngine
{
    WorkerThread::WorkerThread()
    {
        //m_pTaskManager = nullptr;
        m_threadHandle = nullptr;
        m_pThreadLogger = nullptr;
        m_running = false;
        m_threadID = 0;
        m_exitCode = 0;
    }

    WorkerThread::~WorkerThread()
    {}

    //Starts and creates the thread using _beginthreadex
    void WorkerThread::StartWorkerThread(unsigned int threadID)
    {
        //Set the threadID
        SetThreadId(threadID);

        //Begin Log Test code...
        m_pThreadLogger = new ThreadLogger(this->m_threadID);
        
        m_pThreadLogger->OpenLogFile();
        m_pThreadLogger->LogThreadMessage();
        //End Log code...

        m_threadHandle = (HANDLE)_beginthreadex(NULL,0,&cDoTasks,(void*)this,NULL,NULL);
        m_running = true;
        if(m_threadHandle == nullptr)
        {
            //Need to add some more error checking?
            m_running = false;
        }
    }

    void WorkerThread::SetThreadId(unsigned int threadID)
    {
        this->m_threadID = threadID;
    }

    void WorkerThread::SetThreadAffinity()
    {

    }

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
    }

    DWORD WorkerThread::DoTasks()
    {
        //Implement Continuous loop to process tasks and get new task 
        //from taskmanager.
        while(m_running)
        {
            m_taskLock.EnterLock();
            {
                //Run task in here 
            }
            m_taskLock.LeaveLock();
        }
        return 0;
    }
}