#include"MessageQueue.h"

MessageQueue::MessageQueue(unsigned int maxSize)
{
    m_pEnqueueLock = new Lock();
    m_pDequeueLock = new Lock();
    m_pMessageQueue = new Message[maxSize];
    m_QueueLength = maxSize;
    m_Head = 0;
    m_Tail = 0;
    m_Count = 0;
}

MessageQueue::~MessageQueue()
{
    delete[] m_pMessageQueue;
}

void MessageQueue::Enqueue(Message message)
{
    m_pEnqueueLock->EnterLock();
    {
        m_pMessageQueue[m_Head] = message;
        m_Head++;
        m_Count++;
        if(m_Head == m_QueueLength)
            m_Head = 0; //Put head at index 0 if at end 
    }
    m_pEnqueueLock->LeaveLock();
}

Message MessageQueue::Dequeue()
{
    Message value;        
    value = m_pMessageQueue[m_Tail];
    m_Tail++;
    m_Count--;
    if(m_Tail == m_QueueLength)
        m_Tail = 0;

    return value;
}

unsigned int MessageQueue::GetCount()
{
    return m_Count;
}