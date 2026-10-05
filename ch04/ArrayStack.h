// A stack over an array rather than a chain of nodes, with the resizing that
// a fixed array forces on you. The interface is the same as Stack; only the
// storage differs, which is the point Chapter 4 is making with it.
#ifndef CH04_ARRAY_STACK_H
#define CH04_ARRAY_STACK_H

#include <stdexcept>

template <typename Data>
class ArrayStack {
public:
    explicit ArrayStack(int initial = 4)
        : items(new Data[initial]), cap(initial) {}
    ~ArrayStack() { delete[] items; }

    ArrayStack(const ArrayStack&) = delete;             // not the point here
    ArrayStack& operator=(const ArrayStack&) = delete;

    void push(const Data& value);
    Data pop();

    int  size() const { return count; }
    int  capacity() const { return cap; }
    bool is_empty() const { return count == 0; }

private:
    void resize(int new_cap);

    Data* items;
    int   count{0};
    int   cap;
};

#include "ArrayStack.tpp"

#endif
