// A stack: last in, first out. Both operations happen at one end, which is
// why every one of them is O(1) and why the structure is worth having.
//
// The implementation is a singly linked chain whose head is the top of the
// stack, so push is add-to-front and pop is remove-from-front, the two cheap
// operations of Chapter 3's LinkedList.
#ifndef CH04_STACK_H
#define CH04_STACK_H

#include "Node.h"

#include <initializer_list>
#include <stdexcept>

template <typename Data>
class Stack {
public:
    Stack() = default;
    Stack(std::initializer_list<Data> values);

    Stack(const Stack& src);                        // the Rule of Five, again
    Stack& operator=(const Stack& src);
    Stack(Stack&& src) noexcept;
    Stack& operator=(Stack&& src) noexcept;
    ~Stack();

    void push(const Data& data);
    Data pop();
    const Data& peek() const;
    void reverse();

    int  size() const { return length; }
    bool is_empty() const { return top == nullptr; }

private:
    void copy_from(const Node<Data>* src);

    Node<Data>* top{nullptr};
    int         length{0};
};

#include "Stack.tpp"

#endif
