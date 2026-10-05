// A singly linked list, with the same interface as the course repository's.
#ifndef CH03_LINKEDLIST_H
#define CH03_LINKEDLIST_H

#include "Node.h"

#include <cstddef>
#include <initializer_list>
#include <iterator>

template <typename Data>
class LinkedList {
public:
    LinkedList() = default;
    LinkedList(std::initializer_list<Data> values);

    // The Rule of Five. This class owns every node it allocated, so all five
    // have to exist: see Chapter 2.
    ~LinkedList();
    LinkedList(const LinkedList& src);
    LinkedList& operator=(const LinkedList& src);
    LinkedList(LinkedList&& src) noexcept;
    LinkedList& operator=(LinkedList&& src) noexcept;

    void add_to_front(const Data& data);
    void add_to_back(const Data& data);
    void remove_from_front();
    void remove_from_back();

    Data& get(int index);
    const Data& get(int index) const;
    int  count() const { return size; }
    bool is_empty() const { return head == nullptr; }
    void reverse();

    // array-style access, through get()
    Data& operator[](int index);
    const Data& operator[](int index) const;

    // walking the list without touching next
    class Iterator;
    Iterator begin();
    Iterator end();

private:
    Node<Data>* head{nullptr};
    int         size{0};

    void copy_from(const Node<Data>* src);   // shared by the two copy operations
    void clear();                            // shared by the destructor and both assignments
};

#include "LinkedList.tpp"

#endif
