#ifndef CH03_LINKEDLIST_TPP
#define CH03_LINKEDLIST_TPP

#include <stdexcept>
#include <utility>

// --------------------------------------------------------- construction ----
template <typename Data>
LinkedList<Data>::LinkedList(std::initializer_list<Data> values) {
    for (const Data& v : values) add_to_back(v);
}

template <typename Data>
void LinkedList<Data>::clear() {
    while (head != nullptr) {
        Node<Data>* old_head = head;
        head = head->next;    // step off it before freeing it
        delete old_head;
    }
    size = 0;
}

template <typename Data>
void LinkedList<Data>::copy_from(const Node<Data>* src) {
    for (; src != nullptr; src = src->next)
        add_to_back(src->data);
}

template <typename Data>
LinkedList<Data>::~LinkedList() { clear(); }

template <typename Data>
LinkedList<Data>::LinkedList(const LinkedList& src) { copy_from(src.head); }

template <typename Data>
LinkedList<Data>& LinkedList<Data>::operator=(const LinkedList& src) {
    if (this == &src) return *this;
    clear();
    copy_from(src.head);
    return *this;
}

template <typename Data>
LinkedList<Data>::LinkedList(LinkedList&& src) noexcept
    : head(src.head), size(src.size) {
    src.head = nullptr;             // the source must still be safe to destroy
    src.size = 0;
}

template <typename Data>
LinkedList<Data>& LinkedList<Data>::operator=(LinkedList&& src) noexcept {
    if (this == &src) return *this;
    clear();
    head = src.head;
    size = src.size;
    src.head = nullptr;
    src.size = 0;
    return *this;
}

// ------------------------------------------------------- the front, O(1) ---
template <typename Data>
void LinkedList<Data>::add_to_front(const Data& data) {
    Node<Data>* node = new Node<Data>(data);
    // point the new node at the old first one
    node->next = head;
    head = node;          // then make it the first one
    ++size;
}

template <typename Data>
void LinkedList<Data>::remove_from_front() {
    if (head == nullptr)
        throw std::out_of_range("List is empty");
    Node<Data>* old_head = head;
    head = head->next;
    delete old_head;
    --size;
}

// -------------------------------------------------------- the back, O(N) ---
template <typename Data>
void LinkedList<Data>::add_to_back(const Data& data) {
    Node<Data>* node = new Node<Data>(data);
    if (head == nullptr) {
        head = node;
    } else {
        // find the last node
        Node<Data>* current = head;
        while (current->next != nullptr)
            current = current->next;
        current->next = node;
    }
    ++size;
}

template <typename Data>
void LinkedList<Data>::remove_from_back() {
    if (head == nullptr)
        throw std::out_of_range("List is empty");
    if (head->next == nullptr) {    // exactly one node
        delete head;
        head = nullptr;
    } else {
        // stop at the node before the last one
        Node<Data>* current = head;
        while (current->next->next != nullptr)
            current = current->next;
        delete current->next;
        current->next = nullptr;
    }
    --size;
}

// ------------------------------------------------------------ access -------
template <typename Data>
Data& LinkedList<Data>::get(int index) {
    if (index < 0 || index >= size)
        throw std::out_of_range("Index is out of range");
    Node<Data>* current = head;
    for (int i = 0; i < index; ++i) current = current->next;
    return current->data;
}

// the same walk, for a list we may only read
template <typename Data>
const Data& LinkedList<Data>::get(int index) const {
    if (index < 0 || index >= size)
        throw std::out_of_range("Index is out of range");
    const Node<Data>* current = head;
    for (int i = 0; i < index; ++i) current = current->next;
    return current->data;
}

template <typename Data>
Data& LinkedList<Data>::operator[](int index) {
    return get(index);
}

template <typename Data>
const Data& LinkedList<Data>::operator[](int index) const {
    return get(index);
}

// ---------------------------------------------------------- iterator -------
template <typename Data>
class LinkedList<Data>::Iterator {
private:
    Node<Data>* current;
public:
    // what the standard library asks of an iterator
    using iterator_category = std::forward_iterator_tag;
    using value_type        = Data;
    using difference_type   = std::ptrdiff_t;
    using pointer           = Data*;
    using reference         = Data&;

    explicit Iterator(Node<Data>* current)
        : current(current) {}

    Data& operator*() {
        return current->data;
    }

    Iterator& operator++() {        // ++it
        current = current->next;
        return *this;
    }

    Iterator operator++(int) {      // it++
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
typename LinkedList<Data>::Iterator
LinkedList<Data>::begin() {
    return Iterator(head);
}

template <typename Data>
typename LinkedList<Data>::Iterator
LinkedList<Data>::end() {
    return Iterator(nullptr);
}

// ----------------------------------------------------------- reverse -------
template <typename Data>
void LinkedList<Data>::reverse() {
    Node<Data>* previous = nullptr;
    Node<Data>* current  = head;
    while (current != nullptr) {
        // remember it before overwriting
        Node<Data>* next = current->next;
        current->next = previous;    // turn the link around
        // and step both cursors forward
        previous = current;
        current  = next;
    }
    head = previous;
}

#endif
