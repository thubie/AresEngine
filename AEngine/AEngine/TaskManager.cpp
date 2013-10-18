#include"TaskManager.h"

namespace AresEngine
{

     TaskManager::TaskManager()
     {
         m_pWorkerThreads = nullptr;

     }

     TaskManager::~TaskManager()
     {}

     TaskManager::TaskManager(const TaskManager& other)
     {

     }

     bool TaskManager::Initialize()
     {
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

         for(int i = 1; i <= m_createdThreads; ++i)
         {
             m_pWorkerThreads[i - 1].StartWorkerThread(i);
         }

         return true;
     }

     void TaskManager::Shutdown()
     {
         DWORD ExitCount = 0;
         for(int i = 0; i < m_createdThreads; ++i)
         {
             ExitCount += m_pWorkerThreads[i].EndWorkerThread();
         }
     }
}