#pragma once

#include<Windows.h>
#include<process.h> 
#include<stdio.h>
#include"CSLock.h"
#include"TaskManager.h"
#include"Task.h"
#include"ThreadLogger.h"


namespace AresEngine
{
    class TaskManager;

    class WorkerThread
    {
    public:
        WorkerThread();
        WorkerThread(const WorkerThread& other);
        ~WorkerThread();

        void StartWorkerThread(unsigned int threadID,TaskManager* taskManager);
        DWORD EndWorkerThread();

    private:
        void SetThreadId(unsigned int threadID);
        void SetThreadAffinity();
        static unsigned int __stdcall cDoTasks(LPVOID pThis)
        {
            return ((WorkerThread*)pThis)->DoTasks();
        }
        DWORD DoTasks();

    private:
        TaskManager* m_pTaskManager;
        HANDLE m_threadHandle;
        CSLock m_taskLock;
        bool m_running;
        unsigned int m_threadID;
        DWORD m_exitCode;
        Task* m_pTask;
        ThreadLogger* m_pThreadLogger;
        DWORD_PTR m_ProcessorID;
    };
}