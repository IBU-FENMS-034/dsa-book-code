// One solution to the DoublyLinkedList exercise. Write your own first.
#ifndef CH03_DOUBLYLINKEDLIST_TPP
#define CH03_DOUBLYLINKEDLIST_TPP

#include <stdexcept>
#include <utility>

template <typename Data>
DoublyLinkedList<Data>::DoublyLinkedList(
        std::initializer_list<Data> values) {
    for (const Data& v : values) add_to_back(v);
}

template <typename Data>
void DoublyLinkedList<Data>::clear() {
    while (head != nullptr) {
        DoubleNode<Data>* old_head = head;
        head = head->next;
        delete old_head;
    }
    tail = nullptr;
    size = 0;
}

template <typename Data>
DoublyLinkedList<Data>::~DoublyLinkedList() { clear(); }

// add_to_back is constant time here, so the copy is linear
template <typename Data>
DoublyLinkedList<Data>::DoublyLinkedList(const DoublyLinkedList& src) {
    for (DoubleNode<Data>* n = src.head; n != nullptr; n = n->next)
        add_to_back(n->data);
}

template <typename Data>
DoublyLinkedList<Data>&
DoublyLinkedList<Data>::operator=(const DoublyLinkedList& src) {
    if (this == &src) return *this;
    clear();
    for (DoubleNode<Data>* n = src.head; n != nullptr; n = n->next)
        add_to_back(n->data);
    return *this;
}

template <typename Data>
DoublyLinkedList<Data>::DoublyLinkedList(DoublyLinkedList&& src) noexcept
    : head(src.head), tail(src.tail), size(src.size) {
    src.head = nullptr;
    src.tail = nullptr;
    src.size = 0;
}

template <typename Data>
DoublyLinkedList<Data>&
DoublyLinkedList<Data>::operator=(DoublyLinkedList&& src) noexcept {
    if (this == &src) return *this;
    clear();
    head = src.head;
    tail = src.tail;
    size = src.size;
    src.head = nullptr;
    src.tail = nullptr;
    src.size = 0;
    return *this;
}

template <typename Data>
void DoublyLinkedList<Data>::add_to_front(const Data& data) {
    DoubleNode<Data>* node = new DoubleNode<Data>(data);
    node->next = head;
    if (head != nullptr) head->prev = node;
    else tail = node;               // the list was empty
    head = node;
    ++size;
}

template <typename Data>
void DoublyLinkedList<Data>::add_to_back(const Data& data) {
    DoubleNode<Data>* node = new DoubleNode<Data>(data);
    node->prev = tail;
    if (tail != nullptr) tail->next = node;
    else head = node;               // the list was empty
    tail = node;
    ++size;
}

template <typename Data>
void DoublyLinkedList<Data>::remove_from_front() {
    if (head == nullptr)
        throw std::out_of_range("List is empty");
    DoubleNode<Data>* old_head = head;
    head = head->next;
    if (head != nullptr) head->prev = nullptr;
    else tail = nullptr;            // that was the only node
    delete old_head;
    --size;
}

// no walk: the node before the last one is tail->prev
template <typename Data>
void DoublyLinkedList<Data>::remove_from_back() {
    if (tail == nullptr)
        throw std::out_of_range("List is empty");
    DoubleNode<Data>* old_tail = tail;
    tail = tail->prev;
    if (tail != nullptr) tail->next = nullptr;
    else head = nullptr;            // that was the only node
    delete old_tail;
    --size;
}

template <typename Data>
Data& DoublyLinkedList<Data>::get(int index) {
    if (index < 0 || index >= size)
        throw std::out_of_range("Index is out of range");
    DoubleNode<Data>* current = head;
    for (int i = 0; i < index; ++i) current = current->next;
    return current->data;
}

template <typename Data>
const Data& DoublyLinkedList<Data>::get(int index) const {
    if (index < 0 || index >= size)
        throw std::out_of_range("Index is out of range");
    const DoubleNode<Data>* current = head;
    for (int i = 0; i < index; ++i) current = current->next;
    return current->data;
}

template <typename Data>
Data& DoublyLinkedList<Data>::operator[](int index) {
    return get(index);
}

template <typename Data>
const Data& DoublyLinkedList<Data>::operator[](int index) const {
    return get(index);
}

// every node swaps its two pointers, and then the ends swap
template <typename Data>
void DoublyLinkedList<Data>::reverse() {
    for (DoubleNode<Data>* n = head; n != nullptr; n = n->prev)
        std::swap(n->next, n->prev);
    std::swap(head, tail);
}

template <typename Data>
class DoublyLinkedList<Data>::Iterator {
private:
    DoubleNode<Data>* current;
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = Data;
    using difference_type   = std::ptrdiff_t;
    using pointer           = Data*;
    using reference         = Data&;

    explicit Iterator(DoubleNode<Data>* current)
        : current(current) {}

    Data& operator*() { return current->data; }

    Iterator& operator++() {
        current = current->next;
        return *this;
    }

    Iterator operator++(int) {
        Iterator temp = *this;
        current = current->next;
        return temp;
    }

    bool operator==(const Iterator& other) const {
        return current == other.current;
    }

    bool operator!=(const Iterator& other) const {
        return current != other.current;
    }
};

template <typename Data>
typename DoublyLinkedList<Data>::Iterator
DoublyLinkedList<Data>::begin() {
    return Iterator(head);
}

template <typename Data>
typename DoublyLinkedList<Data>::Iterator
DoublyLinkedList<Data>::end() {
    return Iterator(nullptr);
}

#endif
