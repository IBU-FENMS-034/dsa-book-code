#ifndef CH04_STACK_TPP
#define CH04_STACK_TPP

// ------------------------------------------------------- the two operations --
template <typename Data>
void Stack<Data>::push(const Data& data) {
    Node<Data>* node = new Node<Data>(data);
    node->next = top;     // point it at the old top
    top = node;           // then make it the top
    ++length;
}

template <typename Data>
Data Stack<Data>::pop() {
    if (top == nullptr)
        throw std::out_of_range("Stack is empty");
    Node<Data>* old_top = top;
    // copy it out before the node is gone
    Data data = old_top->data;
    top = top->next;
    delete old_top;
    --length;
    return data;
}

template <typename Data>
const Data& Stack<Data>::peek() const {
    if (top == nullptr)
        throw std::out_of_range("Stack is empty");
    return top->data;
}

// ----------------------------------------------------------- housekeeping ---
template <typename Data>
void Stack<Data>::reverse() {
    Node<Data>* previous = nullptr;
    Node<Data>* current  = top;
    while (current != nullptr) {
        Node<Data>* next = current->next;
        current->next = previous;
        previous = current;
        current  = next;
    }
    top = previous;
}

template <typename Data>
void Stack<Data>::copy_from(const Node<Data>* src) {
    // Pushing walks the source top-downwards, which builds the copy upside
    // down, so we turn it the right way up once at the end.
    for (; src != nullptr; src = src->next) push(src->data);
    reverse();
}

template <typename Data>
Stack<Data>::Stack(std::initializer_list<Data> values) {
    for (const Data& v : values) push(v);
}

template <typename Data>
Stack<Data>::~Stack() {
    while (top != nullptr) {
        Node<Data>* old_top = top;
        top = top->next;
        delete old_top;
    }
}

template <typename Data>
Stack<Data>::Stack(const Stack& src) { copy_from(src.top); }

template <typename Data>
Stack<Data>& Stack<Data>::operator=(const Stack& src) {
    if (this != &src) {
        while (top != nullptr) pop();
        copy_from(src.top);
    }
    return *this;
}

template <typename Data>
Stack<Data>::Stack(Stack&& src) noexcept
    : top(src.top), length(src.length) {
    src.top = nullptr;
    src.length = 0;
}

template <typename Data>
Stack<Data>& Stack<Data>::operator=(Stack&& src) noexcept {
    if (this != &src) {
        while (top != nullptr) pop();
        top = src.top;
        length = src.length;
        src.top = nullptr;
        src.length = 0;
    }
    return *this;
}

#endif
