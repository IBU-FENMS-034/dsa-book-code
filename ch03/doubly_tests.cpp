// Checks for the DoublyLinkedList exercise in Chapter 3. Put your own
// DoublyLinkedList.tpp beside DoublyLinkedList.h, then:
//   c++ -std=c++17 -Wall -Wextra -O2 -o doubly_tests doubly_tests.cpp && ./doubly_tests
#include "DoublyLinkedList.h"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <utility>

static int failures = 0;
static void check(bool ok, const char* what) {
    if (!ok) { std::printf("  FAIL  %s\n", what); ++failures; }
}

template <typename T>
static bool holds(const DoublyLinkedList<T>& list, std::initializer_list<T> expected) {
    if (list.count() != static_cast<int>(expected.size())) return false;
    int i = 0;
    for (const T& v : expected) if (!(list[i++] == v)) return false;
    return true;
}

// Walks backwards from the tail, so a missed prev pointer shows up here even
// when the list reads correctly forwards.
template <typename T>
static bool holds_backwards(const DoublyLinkedList<T>& list,
                            std::initializer_list<T> expected) {
    const DoubleNode<T>* n = list.get_back();
    const T* e = expected.end();
    while (e != expected.begin()) {
        if (n == nullptr || !(n->data == *--e)) return false;
        n = n->prev;
    }
    return n == nullptr;
}

int main() {
    // ---- adding, and where head and tail end up ---------------------------
    {
        DoublyLinkedList<int> list;
        check(list.count() == 0, "a new list is empty");
        check(list.get_front() == nullptr && list.get_back() == nullptr,
              "and both ends are nullptr");

        list.add_to_front(10);
        check(list.count() == 1 && list[0] == 10, "one element");
        check(list.get_front() != nullptr && list.get_front() == list.get_back(),
              "with one node, head and tail are the same node");

        list.add_to_front(20);
        check(holds(list, {20, 10}), "add_to_front puts it first");
        check(list.get_front()->data == 20 && list.get_back()->data == 10,
              "head moved, tail did not");

        list.add_to_back(30);
        check(holds(list, {20, 10, 30}), "add_to_back puts it last");
        check(list.get_front()->data == 20 && list.get_back()->data == 30,
              "tail moved, head did not");
        check(holds_backwards(list, {20, 10, 30}), "prev pointers are right");
    }

    // ---- removing from both ends ------------------------------------------
    {
        DoublyLinkedList<int> list{1, 2, 3, 4};
        check(list.get_front()->data == 1 && list.get_back()->data == 4,
              "the initialiser list sets both ends");

        list.remove_from_front();
        check(list.count() == 3 && list.get_front()->data == 2,
              "remove_from_front moves head");
        list.remove_from_back();
        check(list.count() == 2 && list.get_back()->data == 3,
              "remove_from_back moves tail");
        check(holds_backwards(list, {2, 3}), "and the prev pointers follow");

        list.remove_from_front();
        list.remove_from_back();
        check(list.count() == 0 && list.get_front() == nullptr &&
              list.get_back() == nullptr, "emptied, both ends are nullptr");

        bool threw = false;
        try { list.remove_from_back(); } catch (const std::out_of_range&) { threw = true; }
        check(threw, "remove_from_back on an empty list throws");
        threw = false;
        try { list.remove_from_front(); } catch (const std::out_of_range&) { threw = true; }
        check(threw, "remove_from_front on an empty list throws");
    }

    // ---- the Rule of Five -------------------------------------------------
    {
        DoublyLinkedList<int> original{100, 200, 300};

        DoublyLinkedList<int> copy(original);
        copy.add_to_back(400);
        check(holds(original, {100, 200, 300}), "a copy is independent");
        check(holds_backwards(copy, {100, 200, 300, 400}), "and fully linked");

        DoublyLinkedList<int> assigned{9};
        assigned = original;
        check(holds(assigned, {100, 200, 300}) &&
              assigned.get_back()->data == 300, "copy assignment sets tail");

        DoublyLinkedList<int> moved(std::move(copy));
        check(moved.count() == 4 && moved.get_back()->data == 400,
              "the move constructor takes both ends");
        check(copy.count() == 0 && copy.get_front() == nullptr &&
              copy.get_back() == nullptr, "and leaves the source empty");

        DoublyLinkedList<int> target{4, 5, 6};
        target = std::move(moved);
        check(holds(target, {100, 200, 300, 400}), "move assignment");
        check(moved.count() == 0 && moved.get_back() == nullptr,
              "leaves the source empty too");
    }

    // ---- reverse, get, iterator -------------------------------------------
    {
        DoublyLinkedList<int> list{1, 2, 3, 4, 5};
        list.reverse();
        check(holds(list, {5, 4, 3, 2, 1}), "reverse");
        check(list.get_front()->data == 5 && list.get_back()->data == 1,
              "reverse swaps head and tail");
        check(holds_backwards(list, {5, 4, 3, 2, 1}), "and every prev");

        check(list.get(0) == 5 && list.get(4) == 1, "get");
        list[2] = 33;
        check(list[2] == 33, "[] can write");

        int sum = 0;
        for (auto it = list.begin(); it != list.end(); ++it) sum += *it;
        check(sum == 5 + 4 + 33 + 2 + 1, "the iterator visits every node");
    }

    std::printf(failures ? "%d check(s) failed\n" : "all checks pass\n", failures);
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
