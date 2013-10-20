#pragma once 

namespace AresEngine
{
    class ITask
    {
    public:
        virtual ~ITask(){}
        virtual void RunTask() = 0;
    };
}