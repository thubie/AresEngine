#pragma once

#include<Windows.h>


class Lock
{
public:
    Lock();
    ~Lock();
    void EnterLock();
    void LeaveLock();

private:
    CRITICAL_SECTION m_lock;
};
