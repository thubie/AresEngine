#include"TaskManager.h"

namespace AresEngine
{

     TaskManager::TaskManager()
     {
         m_pTaskQueue = nullptr;
         m_pWorkerThreads = nullptr;
         
         m_createdThreads = 0;
         m_head = 0;
         m_tail = 0;
     }

     TaskManager::~TaskManager()
     {}

     TaskManager::TaskManager(const TaskManager& other)
     {
         this->m_createdThreads = other.m_createdThreads;
         this->m_pWorkerThreads = other.m_pWorkerThreads;
     }

     bool TaskManager::Initialize()
     {
         m_pTaskQueue = new ITask*[m_queueSize];
         SYSTEM_INFO sysInfo;
         GetSystemInfo(&sysInfo);
         unsigned int numProcessors = sysInfo.dwNumberOfProcessors;

         //For single core machines one producer and one consumer thread
         if(numProcessors == 1)
         {
             m_createdThreads = 2;
         }
         else if(numProcessors == 2) //For dual core machines one producer and one consumer thread
         {
             m_createdThreads = 2;
         }
         else  //For machines with more then 2 cores make  X cores - 1 threads to give the OS some resources.
         {
             m_createdThreads = numProcessors - 1;
         }

         m_pWorkerThreads = new WorkerThread[m_createdThreads];

         if(m_pWorkerThreads == nullptr) 
         {
            return false;
         }

         for(unsigned int i = 1; i <= m_createdThreads; ++i)
         {
             m_pWorkerThreads[i - 1].StartWorkerThread(i,this);
         }

         return true;
     }

     void TaskManager::Shutdown()
     {
         
         for(unsigned int i = 0; i < m_createdThreads; ++i)
         {
             m_pWorkerThreads[i].EndWorkerThread();
         }
     }

     void TaskManager::EnqueueTask(ITask* task)
     {
         m_EnqueuLock.EnterLock();
         {
             unsigned int newTail = m_tail + 1;
             if(newTail == m_queueSize)
                 newTail = 0;

             //If full resize
             if(newTail == m_head)
             {
                 //Resize function;;
             }

             m_pTaskQueue[m_tail] = task;
             m_tail = newTail;
         }
         m_EnqueuLock.LeaveLock();
     }

     ITask* TaskManager::DequeueTask()
     {
         m_DequeueLock.EnterLock();
         {
             
             if(m_head == m_tail)
                return nullptr; //empty task queue

             ITask* task = m_pTaskQueue[m_head];
             unsigned int newHead = m_head + 1;
             if(newHead == m_queueSize)
                 newHead = 0;
             m_head = newHead;

             return task;
         }
         m_DequeueLock.LeaveLock();
     }

}