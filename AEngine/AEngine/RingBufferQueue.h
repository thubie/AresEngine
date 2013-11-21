#pragma once
#include"Lock.h"
    
template<class Type>
class RingBufferQueue
{
public:
    //Construct Circulair Buffer Queue 
    RingBufferQueue()
    {
        m_pEnqueueLock = new Lock();
        m_pDequeueLock = new Lock();
        m_RingBuffer = new Type[m_InitialSize];
        m_RingBufferLength = m_InitialSize;
        m_Head = 0;
        m_Tail = 0;
        m_RingBufferLength = m_InitialSize;
        m_TaskCount = 0;
    }

    //Go through the list and delete all nodes.
    ~RingBufferQueue()
    {
        delete[] m_RingBuffer;
        delete m_pDequeueLock;
        delete m_pEnqueueLock;
    }

    //Enqueue a new node onto the queue 
    void Enqueue(Type object)
    {
        m_pEnqueueLock->EnterLock();
        {
            int newTail = m_Tail + 1;
            if(newTail == m_RingBufferLength)
            {
                newTail = 0;
            }
            m_RingBuffer[m_Tail] = object;
            m_Tail = newTail;
            m_TaskCount++;
        }
        m_pEnqueueLock->LeaveLock();
    }

    //Dequeue and return a new node.
    Type Dequeue()
    {
        Type value;
        int newHead;
        m_pDequeueLock->EnterLock(); //Enter lock
        {
            if(m_Head == m_Tail)
            {
                m_pDequeueLock->LeaveLock();
                return nullptr;
            }

            value = m_RingBuffer[m_Head];                
            newHead = m_Head  + 1;                
            if(newHead == m_RingBufferLength)
                newHead = 0;

            m_Head = newHead;
            m_TaskCount--;
        }
        m_pDequeueLock->LeaveLock(); //Release lock
        return value;
    }

    inline unsigned int GetTaskCount()
    {
        return m_TaskCount;
    }

    inline bool QueueIsEmpty()
    {
        return m_TaskCount <= 0;
    }

private: 
    const static int    m_InitialSize = 2048;
    Type*               m_RingBuffer;
    unsigned int        m_Head;
    unsigned int        m_Tail;
    unsigned int        m_RingBufferLength;
    unsigned int        m_TaskCount;
    Lock*               m_pEnqueueLock;
    Lock*               m_pDequeueLock;
};
