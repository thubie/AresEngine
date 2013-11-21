#pragma once

#include<Windows.h>
#include<process.h> 
#include<assert.h>
#include"TaskSystem.h"
#include"Tasks.h"
#include"Logger.h"

class TaskSystem;

class WorkerThread
{
public:
    WorkerThread();
    ~WorkerThread();

    HANDLE Initialize(unsigned int threadID,TaskSystem* taskManager);
    void EndWorkerThread();

private:
    DWORD ExecuteTasks();

    static unsigned int __stdcall DoTasks(LPVOID pThis)
    {
        return ((WorkerThread*)pThis)->ExecuteTasks();
    }

private:
    unsigned int m_threadID;
    DWORD m_exitCode;
    TaskSystem* m_pTaskSystem;
    HANDLE m_threadHandle;
    bool m_running;
};
