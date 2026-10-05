// One link in the chain. A struct rather than a class because it has no
// behaviour to protect: it is two fields the structure owns and manipulates.
#ifndef CH04_NODE_H
#define CH04_NODE_H

template <typename Data>
struct Node {
    Data  data{};
    Node* next{nullptr};      // nullptr marks the end of the chain

    Node() = default;
    explicit Node(const Data& data) : data(data) {}
};

#endif
