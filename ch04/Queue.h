// A queue: first in, first out. Elements join at the tail and leave at the
// head, so the two ends do different jobs and the structure keeps a pointer
// to each. Follows Code Repos/FENMS_034_2024-25/Week_03/include/Queue.h.
#ifndef CH04_QUEUE_H
#define CH04_QUEUE_H

#include "Node.h"

#include <initializer_list>
#include <stdexcept>

template <typename Data>
class Queue {
public:
    Queue() = default;
    Queue(std::initializer_list<Data> values);

    Queue(const Queue& src);
    Queue& operator=(const Queue& src);
    Queue(Queue&& src) noexcept;
    Queue& operator=(Queue&& src) noexcept;
    ~Queue();

    void enqueue(const Data& data);
    Data dequeue();
    const Data& peek() const;
    void reverse();

    int  size() const { return length; }
    bool is_empty() const { return head == nullptr; }

private:
    void copy_from(const Node<Data>* src);

    Node<Data>* head{nullptr};    // where elements leave
    Node<Data>* tail{nullptr};    // where they join
    int         length{0};
};

#include "Queue.tpp"

#endif
