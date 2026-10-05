// One solution to the QueueBasedStack exercise. Write your own first.
#ifndef CH04_QUEUEBASEDSTACK_TPP
#define CH04_QUEUEBASEDSTACK_TPP

#include <stdexcept>
#include <utility>

// The new element goes into the empty queue first, then everything else
// follows it, so it ends up at the head, which is the top of the stack.
// That costs a pass over the whole stack: push is linear, pop is constant.
template <typename Data>
void QueueBasedStack<Data>::push(const Data& data) {
    q2.enqueue(data);
    while (!q1.is_empty()) q2.enqueue(q1.dequeue());
    std::swap(q1, q2);
}

template <typename Data>
Data QueueBasedStack<Data>::pop() {
    if (q1.is_empty()) throw std::out_of_range("Stack is empty");
    return q1.dequeue();
}

template <typename Data>
Data QueueBasedStack<Data>::peek() const {
    if (q1.is_empty()) throw std::out_of_range("Stack is empty");
    return q1.peek();
}

template <typename Data>
int QueueBasedStack<Data>::size() const { return q1.size(); }

template <typename Data>
bool QueueBasedStack<Data>::is_empty() const { return q1.is_empty(); }

#endif
