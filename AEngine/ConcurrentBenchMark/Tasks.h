#pragma once 

struct TaskData
{
    void* parameter1;
    void* parameter2;
    void* parameter3;
    void* parameter4;
    void* parameter5;
};

typedef void (*taskfunction)(TaskData*, void* thisPointer);
typedef void (*callback)(void* thisPointer, void* task);

struct Task
{
    void*           thisPointer;
    taskfunction    taskFunction;
    callback        callback;
    TaskData*       pTaskData;
};

