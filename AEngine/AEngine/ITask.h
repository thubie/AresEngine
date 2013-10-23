#pragma once 


class ITask
{
public:
    virtual ~ITask(){}
    virtual void RunTask() = 0;
};
