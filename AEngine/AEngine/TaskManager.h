#pragma once

#include"WorkerThread.h"
#include"CSLock.h"
#include"ITask.h"

namespace AresEngine
{
    class WorkerThread;

    class TaskManager
    {
    public:
        TaskManager();
        ~TaskManager();
        TaskManager(const TaskManager& other);

        bool Initialize();
        void Shutdown();

        void EnqueueTask(ITask* task);
        ITask* DequeueTask();

    private:
        WorkerThread* m_pWorkerThreads;
        unsigned int m_createdThreads;
        ITask** m_pTaskQueue;
        unsigned int m_head;
        unsigned int m_tail;
        static const unsigned int m_queueSize = 2048;
        CSLock m_EnqueuLock;
        CSLock m_DequeueLock;
    };

}