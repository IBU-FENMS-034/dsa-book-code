#ifndef CH04_QUEUE_TPP
#define CH04_QUEUE_TPP

// ------------------------------------------------------- the two operations --
template <typename Data>
void Queue<Data>::enqueue(const Data& data) {
    Node<Data>* node = new Node<Data>(data);
    if (head == nullptr) {
        // first element: it is both ends at once
        head = tail = node;
    } else {
        // no walk: the tail pointer is already there
        tail->next = node;
        tail = node;
    }
    ++length;
}

template <typename Data>
Data Queue<Data>::dequeue() {
    if (head == nullptr)
        throw std::out_of_range("Queue is empty");
    Node<Data>* old_head = head;
    Data data = old_head->data;
    head = head->next;
    if (head == nullptr)    // the queue is now empty
        tail = nullptr;
    delete old_head;
    --length;
    return data;
}

template <typename Data>
const Data& Queue<Data>::peek() const {
    if (head == nullptr)
        throw std::out_of_range("Queue is empty");
    return head->data;
}

// Chapter 3's reverse, plus one line: the old head is the new tail.
template <typename Data>
void Queue<Data>::reverse() {
    Node<Data>* previous = nullptr;
    Node<Data>* current  = head;
    tail = head;
    while (current != nullptr) {
        Node<Data>* next = current->next;
        current->next = previous;
        previous = current;
        current  = next;
    }
    head = previous;
}

// ----------------------------------------------------------- housekeeping ---
template <typename Data>
void Queue<Data>::copy_from(const Node<Data>* src) {
    for (; src != nullptr; src = src->next) enqueue(src->data);
}

template <typename Data>
Queue<Data>::Queue(std::initializer_list<Data> values) {
    for (const Data& v : values) enqueue(v);
}

template <typename Data>
Queue<Data>::~Queue() {
    while (head != nullptr) {
        Node<Data>* old_head = head;
        head = head->next;
        delete old_head;
    }
    tail = nullptr;
}

template <typename Data>
Queue<Data>::Queue(const Queue& src) { copy_from(src.head); }

template <typename Data>
Queue<Data>& Queue<Data>::operator=(const Queue& src) {
    if (this != &src) {
        while (head != nullptr) dequeue();
        copy_from(src.head);
    }
    return *this;
}

template <typename Data>
Queue<Data>::Queue(Queue&& src) noexcept
    : head(src.head), tail(src.tail), length(src.length) {
    src.head = src.tail = nullptr;
    src.length = 0;
}

template <typename Data>
Queue<Data>& Queue<Data>::operator=(Queue&& src) noexcept {
    if (this != &src) {
        while (head != nullptr) dequeue();
        head = src.head;
        tail = src.tail;
        length = src.length;
        src.head = src.tail = nullptr;
        src.length = 0;
    }
    return *this;
}

#endif
