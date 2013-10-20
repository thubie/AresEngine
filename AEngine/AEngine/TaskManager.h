#pragma once

#include"WorkerThread.h"
#include"CSLock.h"
#include"Task.h"

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

        void EnqueueTask(Task* task);
        Task* DequeueTask();

    private:
        WorkerThread* m_pWorkerThreads;
        unsigned int m_createdThreads;
        
        static void TESTCOUNTER(TaskData* inputTaskData)
        {
            int counter = 0;
            TaskData taskData = *inputTaskData;
            int maxCount = *(int*)(taskData.parameter1);
            for(int i = 0; i < maxCount; ++i)
            {
                counter++;
            }
        }

        Task** m_pTaskQueue;
        Task* m_testTask;
        int m_testData;
        unsigned int m_head;
        unsigned int m_tail;
        static const unsigned int m_queueSize = 2048;
        CSLock m_EnqueuLock;
        CSLock m_DequeueLock;
    };

}