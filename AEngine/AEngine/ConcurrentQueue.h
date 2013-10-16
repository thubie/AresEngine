#pragma once
#include"CSLock.h"
#include<iostream>

namespace AresEngine
{
    template<class Type>
    class ConcurrentQueue
    {
        template <typename T>
        struct Node
        {
            T m_value;
            Node<T>* volatile m_pNext;
        };

    public:

        //Construct Queue with dummy node
        ConcurrentQueue()
        {
            m_phead = m_ptail = new Node<Type>();
        }

        //Go through the list and delete all nodes.
        ~ConcurrentQueue()
        {
        }

        bool emptyQueue()
        {
            Node<Type>* current = m_phead;
            Node<Type>* next;
            while(current != m_ptail)
            {
                next = current->m_pNext;
                delete current;
                current = next;
            }
            m_phead = nullptr;
        }

        //Enqueue a new node onto the queue 
        void Enqueue(Type object)
        {
            Node<Type>* newNode = new Node<Type>();
            newNode->m_value = object;
            m_enqueueLock.EnterLock();
            {
                m_ptail->m_pNext = newNode;
                m_ptail = newNode;
            }
            m_enqueueLock.LeaveLock();
        }

        //Dequeue and return a new node.
        Type Dequeue()
        {
            Type value;
            Node<Type>* pNext;
            pNext = nullptr;
            m_dequeueLock.EnterLock();
            {
                pNext = m_phead->m_pNext;
                if(pNext == nullptr)
                {
                    //Need to fix this later....!!!!
                    std::cerr << "Queue empty \n";
                    //return nullptr and check it in the function using dequeue
                    return NULL;
                }
                value  = pNext->m_value;
                m_phead = pNext; 
                return value;
            }
            m_dequeueLock.LeaveLock();
            
        }


    private:
        Node<Type>* m_phead;
        Node<Type>* m_ptail;
        CSLock m_enqueueLock;
        CSLock m_dequeueLock;

    };
}