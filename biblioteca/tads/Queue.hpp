
#ifndef _TQUEUE_TAD_
#define _TQUEUE_TAD_

#include <iostream>
#include "../funciones/lists.hpp"

using namespace std;

template <typename T>
struct Queue // 1.7.6.1
{
   Node<T>* first;
   Node<T>* last;
   int size;
};

template <typename T>
Queue<T> queue() // 1.7.6.2
{
   Queue<T> q;
   q.first = NULL;
   q.last = NULL;
   q.size = 0;
   return q;
}

template <typename T>
T* queueEnqueue(Queue<T>& q, T e) // 1.7.6.3
{
   Node<T>* node = enqueue<T>(q.first, q.last, e);
   q.size++;
   return &(node->data);
}

template <typename T>
T queueDequeue(Queue<T>& q) // 1.7.6.4
{
   T t = dequeue<T>(q.first, q.last);
   q.size--;
   return t;
}

template <typename T>
bool queueIsEmpty(Queue<T> q) // 1.7.6.5
{
   return q.size == 0;
}

template <typename T>
int queueSize(Queue<T> q) // 1.7.6.6
{
   return q.size;
}

#endif
