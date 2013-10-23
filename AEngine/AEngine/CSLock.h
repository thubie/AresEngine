#pragma once

#include<Windows.h>


class CSLock
{
public:
    CSLock();
    ~CSLock();
    void EnterLock();
    void LeaveLock();

private:
    CRITICAL_SECTION m_lock;
};
