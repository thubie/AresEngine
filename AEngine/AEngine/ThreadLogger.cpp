#include"ThreadLogger.h"


namespace AresEngine
{
    ThreadLogger::ThreadLogger(unsigned int threadID)
    {
        this->threadID = threadID;
        this->m_pLogFile = nullptr;
    }

    ThreadLogger::~ThreadLogger()
    {}

    void ThreadLogger::OpenLogFile()
    {
        char filepath[64];
        sprintf(filepath,"../TestLogs/Workerthread_%d.txt",threadID);
        this->m_pLogFile = fopen(filepath,"w");
    }

    void ThreadLogger::CloseLogFile()
    {
        fflush(m_pLogFile);
        fclose(this->m_pLogFile);
    }

    void ThreadLogger::LogThreadMessage(/*char* string,short len*/)
    {
        fprintf(this->m_pLogFile, "Testing from thread: %d \n",threadID);
        fflush(m_pLogFile);
    }
    
}