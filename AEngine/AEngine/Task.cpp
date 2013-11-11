#include"Task.h"

namespace AresEngine
{
    Task::Task()
    {
        m_pTaskData = nullptr;
        m_fpTaskData = nullptr;
    }

    Task::Task(TaskFunction task, TaskData* taskData)
    {
        m_pTaskData = taskData;
        m_fpTaskData = task;
    }

    Task::Task(const Task& other)
    {
        this->m_pTaskData = other.m_pTaskData;
        this->m_fpTaskData = other.m_fpTaskData;
    }

    Task::~Task()
    {
        if(m_pTaskData != nullptr)
            delete m_pTaskData;
        
        if(m_fpTaskData != nullptr)
            delete m_pTaskData task;
    }

    TaskData* Task::GetTaskData()
    {
        return m_pTaskData;
    }

    TaskFunction Task::GetTaskFunction()
    {
        return m_fpTaskData;
    }
}
