#include"Lock.h"


Lock::Lock()
{
    InitializeCriticalSection(&m_lock);
}

Lock::~Lock()
{
    DeleteCriticalSection(&m_lock);
}

void Lock::EnterLock()
{
    EnterCriticalSection(&m_lock);
}

void Lock::LeaveLock()
{
    LeaveCriticalSection(&m_lock);
}