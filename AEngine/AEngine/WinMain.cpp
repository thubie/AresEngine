#include"AEngine.h"
#include<stdio.h>
#include<iostream>

void MyCreateThreads(HANDLE* threadArray, DWORD* threadIdArray, unsigned short numProcs,FILE* file);
DWORD WINAPI LogThreads(LPVOID lpParameter);
using namespace AresEngine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pSCmdLine,int iCmdShow)
{
    AEngine* pAEngine;
    bool result;
    FILE* plogFile;

    fopen_s(&plogFile,"../LogFile.txt","w");
    
    //Used to discover how many physical cores there are
    SYSTEM_INFO sysInfo;
    int numPhysicalCores;
    HANDLE* hThreads;
    DWORD* threadIds;

    //////////////////////////////////////////////////////////////

    //Create the app object
    pAEngine = new AEngine();

    if(!pAEngine)
        return 0;

    //Intialize and check it then run
    result = pAEngine->Initialize();

    fprintf(plogFile,"Test application initalize..\n");
    
    //Get physical core stats and log it.
    GetSystemInfo(&sysInfo);
    numPhysicalCores = sysInfo.dwNumberOfProcessors;
    fprintf(plogFile,"Number of cores: %d \n",numPhysicalCores);

    hThreads = new HANDLE[numPhysicalCores];
    threadIds = new DWORD[numPhysicalCores];

    MyCreateThreads(hThreads,threadIds,numPhysicalCores,plogFile);

    ///////////////////////////////////////////////////////////////////

    if(result)
    {
        pAEngine->Run();
    }

    fprintf(plogFile,"Test application shutdown..\n");

    fclose(plogFile);
}

void MyCreateThreads(HANDLE* threadArray, DWORD* threadIdArray, unsigned short numProcs,FILE* file)
{
    for(int i = 0; i < numProcs; ++i)
    {
        threadArray[i] = CreateThread(NULL,0,
            &LogThreads,&file,0,&threadIdArray[i]);
    }
}

DWORD WINAPI LogThreads(LPVOID lpParameter)
{
    unsigned long long busy = 0;
    while(busy < 4000000000000)
        ++busy;

    return 0;
}