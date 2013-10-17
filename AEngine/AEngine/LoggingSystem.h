#include<stdio.h>
#include"CSLock.h"

namespace AresEngine
{
    class LoggingSystem
    {
    public:
        LoggingSystem();
        LoggingSystem(const LoggingSystem& other);
        ~LoggingSystem();

        void Initialize();
        void Shutdown();

        void LoggingMessage();
        void DebugMessage();
        void OutputWindowMessage();

        //OutputDebugString(L"Test Threads \n");
        //FILE* plogFile;
        //fopen(&plogFile,"../LogFile.txt","w");
        //fprintf(plogFile,"Test application initalize..\n");
        //fprintf(plogFile,"Test application shutdown..\n");
        //fclose(plogFile);
    };
}