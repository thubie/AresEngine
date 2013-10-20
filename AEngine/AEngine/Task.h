#pragma once 

namespace AresEngine
{
    struct TaskData;

    typedef void(*TaskFunction)(TaskData*);
    
    struct TaskData
    {
        void* parameter1;
        void* parameter2;
        void* parameter3;
        void* parameter4;
        void* parameter5;
    };

    class Task
    {
    public:
        Task();
        Task(TaskFunction task, TaskData* taskData);
        Task(const Task& other);
        ~Task();
        TaskData* GetTaskData();
        TaskFunction GetTaskFunction();
    private:
        TaskData* m_pTaskData;
        TaskFunction m_fpTaskData;
    };
}