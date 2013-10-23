#include"CSLock.h"


CSLock::CSLock()
{
    InitializeCriticalSection(&m_lock);
}

CSLock::~CSLock()
{
    DeleteCriticalSection(&m_lock);
}

void CSLock::EnterLock()
{
    EnterCriticalSection(&m_lock);
}

void CSLock::LeaveLock()
{
    LeaveCriticalSection(&m_lock);
}