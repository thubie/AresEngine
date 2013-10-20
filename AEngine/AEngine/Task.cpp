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
