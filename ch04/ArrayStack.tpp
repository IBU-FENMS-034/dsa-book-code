#ifndef CH04_ARRAY_STACK_TPP
#define CH04_ARRAY_STACK_TPP

// Allocate a new block, copy across, free the old one. This is the whole of
// the resizing array technique.
template <typename Data>
void ArrayStack<Data>::resize(int new_cap) {
    Data* bigger = new Data[new_cap];
    for (int i = 0; i < count; ++i) bigger[i] = items[i];
    delete[] items;
    items = bigger;
    cap = new_cap;
}

template <typename Data>
void ArrayStack<Data>::push(const Data& value) {
    if (count == cap) resize(2 * cap);      // full: double it
    items[count++] = value;                 // then store, and move top up
}

template <typename Data>
Data ArrayStack<Data>::pop() {
    if (count == 0) throw std::out_of_range("pop on an empty stack");
    Data value = items[--count];            // move top down, then read
    // A quarter rather than a half: halving at half means a program that
    // pushes and pops across the boundary resizes on every operation.
    if (count > 0 && count == cap / 4) resize(cap / 2);
    return value;
}

#endif
