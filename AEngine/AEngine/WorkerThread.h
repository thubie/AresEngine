#pragma once

#include<Windows.h>
#include<process.h> 
#include<stdio.h>
#include"CSLock.h"
#include"TaskSystem.h"
#include"ITask.h"
#include"Logger.h"
#include"CounterTask.h"


class TaskSystem;

class WorkerThread
{
public:
    WorkerThread();
    WorkerThread(const WorkerThread& other);
    ~WorkerThread();

    HANDLE StartWorkerThread(unsigned int threadID,TaskSystem* taskManager);
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
    TaskSystem* m_pTaskSystem;
    HANDLE m_threadHandle;
    bool m_running;
    unsigned int m_threadID;
    DWORD m_exitCode;
    DWORD_PTR m_ProcessorID;
};
