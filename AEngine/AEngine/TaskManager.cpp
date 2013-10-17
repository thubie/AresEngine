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

         if(numProcessors == 1)     //For single core machines one producer and one consumer thread
             m_createdThreads = 2;

         else if(numProcessors == 2) //For dual core machines one producer and one consumer thread
             m_createdThreads = 2;
         //For machines with more then 2 cores make  X cores - 1 threads to give the OS some resources.
         else
             m_createdThreads = numProcessors - 1;
     
             
         m_pWorkerThreads = new WorkerThread[m_createdThreads];

         if(m_pWorkerThreads == nullptr) {
            return false;
         }

         for(int i = 0; i < m_createdThreads; ++i)
         {
             m_pWorkerThreads[i].StartWorkerThread();
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