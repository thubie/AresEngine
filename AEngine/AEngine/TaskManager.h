#pragma once
#include"WorkerThread.h"

namespace AresEngine
{

    class TaskManager
    {
    public:
        TaskManager();
        ~TaskManager();
        TaskManager(const TaskManager& other);

        bool Initialize();
        void Shutdown();

    private:
        WorkerThread* m_pWorkerThreads;
        unsigned int m_createdThreads;
    };


}