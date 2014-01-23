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
        m_pRingBuffer = new Type[length];
        m_ringBufferLength = length;
        m_head = 0;
        m_tail = 0;
        m_count = 0;
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
            m_pRingBuffer[m_head] = object;
            m_head++;
            m_count++;
            if(m_head == m_ringBufferLength)
                m_head = 0; //Put head at index 0 if at end 
        }
        m_pEnqueueLock->LeaveLock();
    }

    //Dequeue and return a new node. 
    //The main game thread has to first query Getcount() to check if queue isn't empty
    Type Dequeue()
    {
        Type value;        
        value = m_pRingBuffer[m_tail];
        m_tail++;
        m_count--;
        if(m_tail == m_ringBufferLength)
            m_tail = 0;

        return value;
    }

    inline unsigned int GetCount()
    {
        return m_count;
    }

    inline bool QueueIsEmpty()
    {
        bool empty = (m_count == 0);
        return empty;
    }

private: 
    Type*               m_pRingBuffer;
    unsigned int        m_head;
    unsigned int        m_tail;
    unsigned int        m_ringBufferLength;
    unsigned int        m_count;
    Lock*               m_pEnqueueLock;
    Lock*               m_pDequeueLock;
};
