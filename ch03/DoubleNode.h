// One link in a doubly linked list: Node from Node.h, plus a way back.
#ifndef CH03_DOUBLENODE_H
#define CH03_DOUBLENODE_H

template <typename Data>
struct DoubleNode {
    Data        data{};
    DoubleNode* next{nullptr};
    DoubleNode* prev{nullptr};

    DoubleNode() = default;
    explicit DoubleNode(const Data& data) : data(data) {}
};

#endif
