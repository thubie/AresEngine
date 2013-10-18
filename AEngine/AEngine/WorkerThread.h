#pragma once

#include<Windows.h>
#include <process.h> 
#include<stdio.h>
//#include"TaskManager.h"
#include"CSLock.h"
#include"ThreadLogger.h"

namespace AresEngine
{
    class WorkerThread
    {
        //functions
    public:
        WorkerThread();
        ~WorkerThread();

        void StartWorkerThread(unsigned int threadID);
        void SetThreadAffinity();

        DWORD EndWorkerThread();

    private:
        void SetThreadId(unsigned int threadID);

        //This function will call the DoTasks()
        //_beginthreadex requires static unsigned int __stdcall functionname(LPVOID);
        static unsigned int __stdcall cDoTasks(LPVOID pThis)
        {
            return ((WorkerThread*)pThis)->DoTasks();
        }

        DWORD DoTasks();

        //members
    private:
        //TaskManager* m_pTaskManager;
        HANDLE m_threadHandle;
        CSLock m_taskLock;
        bool m_running;
        unsigned int m_threadID;
        DWORD m_exitCode;
        ThreadLogger* m_pThreadLogger;
    };
}