#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>

namespace AresEngine
{
    class ThreadLogger
    {
    public:
        ThreadLogger(unsigned int threadID);
        ~ThreadLogger();
        
        void OpenLogFile();
        void CloseLogFile();
        void LogThreadMessage(/*char* string,short len*/);

    private:
        FILE* m_pLogFile;
        unsigned int threadID;
    };
}