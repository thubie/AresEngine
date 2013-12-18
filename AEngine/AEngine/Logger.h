#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include<Windows.h>
#include<intrin.h>

namespace Logger
{
    // This in memory logger is based on blog post 
    // http://preshing.com/20120522/lightweight-in-memory-logging/

    struct Event
    {
        DWORD threadID;       //Thread ID
        const char* msg;    //Message string
        DWORD param;          //A parameter can mean everything like execution time
    };

    static const int BUFFER_SIZE = 65536;
    extern Event g_events[BUFFER_SIZE];
    extern LONG g_pos;

    inline void Log(const char* msg, DWORD threadID, DWORD param)
    {
        //Get next event index
        LONG index = _InterlockedIncrement(&g_pos);
        //Write an event at this index
        Event* e = g_events + (index & (BUFFER_SIZE - 1)); //Wrap to buffer size
        e->threadID = threadID;
        e->msg = msg;
        e->param = param;
    }
}

#define LOG(m, id, p) Logger::Log(m, id, p); //Logging macro