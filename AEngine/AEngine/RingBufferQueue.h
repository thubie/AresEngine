#pragma once
#include"Lock.h"

//Multiple producer and single consumer concurrent fifo ringbuffer

template<class Type>
class RingBufferQueue
{
public:
    //Construct Circulair Buffer Queue 
    RingBufferQueue(unsigned int length)
    {
        m_pEnqueueLock = new Lock();
        m_pDequeueLock = new Lock();
        m_RingBuffer = new Type[length];
        m_RingBufferLength = length;
        m_Head = 0;
        m_Tail = 0;
        m_Count = 0;
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
            m_RingBuffer[m_head] = object;
            m_head++;
            m_Count++;
            if(m_head == m_RingBufferLength)
                m_head = 0; //Put head at index 0 if at end 
        }
        m_pEnqueueLock->LeaveLock();
    }

    //Dequeue and return a new node. 
    //The main game thread has to first query Getcount() to check if queue isn't empty
    Type Dequeue()
    {
        Type value;        
        value = m_RingBuffer[tail];
        m_tail++;
        m_count--;
        if(m_tail == m_RingBufferLength)
            m_tail = 0;

        return value;
    }

    inline unsigned int GetTaskCount()
    {
        return Count;
    }

private: 
    Type*               m_RingBuffer;
    unsigned int        m_Head;
    unsigned int        m_Tail;
    unsigned int        m_RingBufferLength;
    unsigned int        m_Count;
    Lock*               m_pEnqueueLock;
    Lock*               m_pDequeueLock;
};
