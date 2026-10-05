// The doubly linked list from the exercises at the end of Chapter 3. The
// interface is LinkedList's, plus a tail pointer and two accessors the tests
// use to check that head and tail are kept right.
#ifndef CH03_DOUBLYLINKEDLIST_H
#define CH03_DOUBLYLINKEDLIST_H

#include "DoubleNode.h"

#include <cstddef>
#include <initializer_list>
#include <iterator>

template <typename Data>
class DoublyLinkedList {
public:
    DoublyLinkedList() = default;
    DoublyLinkedList(std::initializer_list<Data> values);

    ~DoublyLinkedList();
    DoublyLinkedList(const DoublyLinkedList& src);
    DoublyLinkedList& operator=(const DoublyLinkedList& src);
    DoublyLinkedList(DoublyLinkedList&& src) noexcept;
    DoublyLinkedList& operator=(DoublyLinkedList&& src) noexcept;

    void add_to_front(const Data& data);
    void add_to_back(const Data& data);
    void remove_from_front();
    void remove_from_back();

    Data& get(int index);
    const Data& get(int index) const;
    int  count() const { return size; }
    void reverse();

    Data& operator[](int index);
    const Data& operator[](int index) const;

    class Iterator;
    Iterator begin();
    Iterator end();

    // used by the tests to check the two ends
    DoubleNode<Data>* get_front() const { return head; }
    DoubleNode<Data>* get_back() const { return tail; }

private:
    DoubleNode<Data>* head{nullptr};
    DoubleNode<Data>* tail{nullptr};
    int               size{0};

    void clear();
};

#include "DoublyLinkedList.tpp"

#endif
