#include"WorkerThread.h"

namespace AresEngine
{
    WorkerThread::WorkerThread()
    {
        //m_pTaskManager = nullptr;
        m_threadHandle = NULL;
        m_running = false;
        m_threadID = 0;
        m_exitCode = 0;
    }

    WorkerThread::~WorkerThread()
    {}

    //Starts and creates the thread using _beginthreadex
    void WorkerThread::StartWorkerThread()
    {
        //m_taskLock = new CSLock();
        this->m_threadHandle = (HANDLE)_beginthreadex(NULL,0,&cDoTasks,(void*)this,NULL,&m_threadID);
        m_running = true;
        if(m_threadHandle == NULL)
        {
            //Need to add some more error checking?
            m_running = false;
        }
    }

    DWORD WorkerThread::EndWorkerThread()
    {
        if(m_threadHandle != NULL)
        {
            m_running = false; //kill off the loop
            WaitForSingleObject(m_threadHandle,INFINITE); //wait for thread to finish up working
            GetExitCodeThread( m_threadHandle, &m_exitCode); //Get exit code for later
            CloseHandle(m_threadHandle); //Closes the thread object
            m_threadHandle = nullptr;
            #ifdef _DEBUG
                OutputDebugString(L"Test Threads Closed \n");
            #endif // End _DEBUG
            
            return m_exitCode;
        }
    }

    DWORD WorkerThread::DoTasks()
    {
        //Implement Continuous loop to process tasks and get new task 
        //from taskmanager.

        //Test code
        while(m_running)
        {
            #ifdef _DEBUG
                OutputDebugString(L"Hallo from test thread\n");
                Sleep(500);
            #endif // End _DEBUG
            this->m_taskLock.EnterLock();
            {
                //Run task in here 
            }
            this->m_taskLock.LeaveLock();
        }
        return 0;
    }
}