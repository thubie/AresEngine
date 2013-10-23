#pragma once

#include<Windows.h>
#include<process.h> 
#include<stdio.h>
#include"CSLock.h"
#include"TaskManager.h"
#include"ITask.h"
#include"Logger.h"
#include"CounterTask.h"



class TaskManager;

class WorkerThread
{
public:
    WorkerThread();
    WorkerThread(const WorkerThread& other);
    ~WorkerThread();

    HANDLE StartWorkerThread(unsigned int threadID,TaskManager* taskManager);
    void EndWorkerThread();

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
    bool m_running;
    unsigned int m_threadID;
    DWORD m_exitCode;
    DWORD_PTR m_ProcessorID;
};
