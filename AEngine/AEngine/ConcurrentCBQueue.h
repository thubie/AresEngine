#pragma once
#include"CSLock.h"


    
template<class Type>
class ConcurrentCBQueue
{
public:
    //Construct Circulair Buffer Queue 
    ConcurrentCBQueue()
    {
        m_pEnqueueLock = new CSLock();
        m_pDequeueLock = new CSLock();
        m_CBuffer = new Type[m_InitialSize];
        m_CBufferLength = m_InitialSize;
        m_Head = 0;
        m_Tail = 0;
    }

    //Go through the list and delete all nodes.
    ~ConcurrentCBQueue()
    {
        delete m_CBuffer;
        delete m_pDequeueLock;
        delete m_pEnqueueLock;
    }


    //Enqueue a new node onto the queue 
    void Enqueue(Type object)
    {
        m_pEnqueueLock->EnterLock();
        {
            int newTail = m_Tail + 1;
            if(newTail == m_Head)
            {
            }

            m_CBuffer[m_Tail] = object;
            m_Tail = newTail;
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
                value = nullptr;
            else
                value = m_CBuffer[m_Head];
                
            newHead = m_Head  + 1;
                
            if(newHead == m_CBufferLength)
                newHead = 0;
                
            m_Head = newHead;
        }
        m_pDequeueLock->LeaveLock(); //Release lock

        return value;
    }


private: 
    const static int m_InitialSize = 256;
    Type* m_CBuffer;
    int m_Head;
    int m_Tail;
    int m_CBufferLength;
    CSLock* m_pEnqueueLock;
    CSLock* m_pDequeueLock;
};
