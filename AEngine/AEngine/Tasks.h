#pragma once 

struct TaskData
{
    void* parameter1;
    void* parameter2;
    void* parameter3;
    void* parameter4;
    void* parameter5;
};

typedef void (*task)(TaskData*, void* thisPointer);
typedef void (*callback)(void* thisPointer, void* task);

struct Task
{
    void* thisPointer;
    task taskFunction;
    callback callback;
    TaskData* pTaskData;
    unsigned int taskId;
};

