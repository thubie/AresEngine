#pragma once

namespace AresEngine
{

    struct TaskData
    {
        void* Parameter1;
        void* Parameter2;
        void* Parameter3;
        void* Parameter4;
    };
    
    struct Task
    {
        TaskData taskData;

    };

    class TaskManager
    {

    };

}