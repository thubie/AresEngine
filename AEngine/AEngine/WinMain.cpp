#include"AEngine.h"
#include<stdio.h>
#include<iostream>
#include"WorkerThread.h"

void MyCreateThreads(HANDLE* threadArray, DWORD* threadIdArray, unsigned short numProcs,FILE* file);
DWORD WINAPI LogThreads(LPVOID lpParameter);
using namespace AresEngine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pSCmdLine,int iCmdShow)
{
    AEngine* pAEngine;
    bool result;
    FILE* plogFile;
    WorkerThread* testWorkerThread;
    fopen_s(&plogFile,"../LogFile.txt","w");
    
    //Used to discover how many physical cores there are
    SYSTEM_INFO sysInfo;
    int numPhysicalCores;
    HANDLE* hThreads;
    DWORD* threadIds;

    //////////////////////////////////////////////////////////////

    //Create the app object
    pAEngine = new AEngine();
    testWorkerThread = new WorkerThread();

    


    if(!pAEngine)
        return 0;

    //Intialize and check it then run
    result = pAEngine->Initialize();
    testWorkerThread->StartWorkerThread();
    fprintf(plogFile,"Test application initalize..\n");
    


    ///////////////////////////////////////////////////////////////////

    if(result)
    {
        pAEngine->Run();
    }
    testWorkerThread->EndWorkerThread();

    fprintf(plogFile,"Test application shutdown..\n");

    fclose(plogFile);
    
    return 0;
}

