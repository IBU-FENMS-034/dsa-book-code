// The stack-from-two-queues exercise at the end of Chapter 4. The interface
// is the stack's; the storage is two of Queue.h's queues and nothing else.
#ifndef CH04_QUEUEBASEDSTACK_H
#define CH04_QUEUEBASEDSTACK_H

#include "Queue.h"

template <typename Data>
class QueueBasedStack {
public:
    QueueBasedStack() = default;

    void push(const Data& data);
    Data pop();
    Data peek() const;
    int  size() const;
    bool is_empty() const;

private:
    Queue<Data> q1;     // the elements, top of the stack at the head
    Queue<Data> q2;     // empty between operations
};

#include "QueueBasedStack.tpp"

#endif
