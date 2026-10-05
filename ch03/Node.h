// One link in the chain. A struct rather than a class because it has no
// behaviour to protect: it is two fields the list owns and manipulates.
#ifndef CH03_NODE_H
#define CH03_NODE_H

template <typename Data>
struct Node {
    Data  data{};
    // nullptr marks the end of the list
    Node* next{nullptr};

    Node() = default;
    explicit Node(const Data& data) : data(data) {}
};

#endif
