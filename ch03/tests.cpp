// Every listing in Chapter 3 compiles and passes these checks.
//   c++ -std=c++17 -Wall -Wextra -O2 -o tests tests.cpp && ./tests
#include "LinkedList.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

static int failures = 0;
static void check(bool ok, const char* what) {
    if (!ok) { std::printf("  FAIL  %s\n", what); ++failures; }
}

template <typename T>
static bool holds(const LinkedList<T>& list, std::initializer_list<T> expected) {
    if (list.count() != static_cast<int>(expected.size())) return false;
    int i = 0;
    for (const T& v : expected) if (!(list.get(i++) == v)) return false;
    return true;
}

static bool throws_out_of_range(void (*f)(LinkedList<int>&), LinkedList<int>& l) {
    try { f(l); } catch (const std::out_of_range&) { return true; } catch (...) { }
    return false;
}

int main() {
    // ---- the empty list, which is where most list bugs live ---------------
    {
        LinkedList<int> empty;
        check(empty.count() == 0, "a new list is empty");
        check(empty.is_empty(),   "and says so");
        check(throws_out_of_range([](LinkedList<int>& l){ l.remove_from_front(); }, empty),
              "remove_from_front on an empty list throws");
        check(throws_out_of_range([](LinkedList<int>& l){ l.remove_from_back(); }, empty),
              "remove_from_back on an empty list throws");
        check(throws_out_of_range([](LinkedList<int>& l){ (void) l.get(0); }, empty),
              "get(0) on an empty list throws");
    }

    // ---- adding at each end ----------------------------------------------
    {
        LinkedList<int> l;
        l.add_to_front(2); l.add_to_front(1);      // 1 -> 2
        l.add_to_back(3);  l.add_to_back(4);       // 1 -> 2 -> 3 -> 4
        check(holds(l, {1, 2, 3, 4}), "front and back insertion interleave correctly");
        check(l.count() == 4, "count tracks the insertions");
    }

    // ---- the one-element list, the other place bugs live ------------------
    {
        LinkedList<int> l;
        l.add_to_front(7);
        check(holds(l, {7}), "one element");
        l.remove_from_back();                      // must not walk off the end
        check(l.count() == 0 && l.is_empty(), "removing the only node empties the list");
        l.add_to_back(9);
        l.remove_from_front();
        check(l.is_empty(), "and so does removing it from the front");
    }

    // ---- removal from both ends ------------------------------------------
    {
        LinkedList<int> l{1, 2, 3, 4, 5};
        check(holds(l, {1, 2, 3, 4, 5}), "the initialiser list builds in order");
        l.remove_from_front();
        check(holds(l, {2, 3, 4, 5}), "remove_from_front takes the first");
        l.remove_from_back();
        check(holds(l, {2, 3, 4}), "remove_from_back takes the last");
    }

    // ---- reverse ----------------------------------------------------------
    {
        LinkedList<int> l{1, 2, 3, 4, 5};
        l.reverse();
        check(holds(l, {5, 4, 3, 2, 1}), "reverse turns the list around");
        l.reverse();
        check(holds(l, {1, 2, 3, 4, 5}), "and twice is the identity");

        LinkedList<int> one{42};
        one.reverse();
        check(holds(one, {42}), "reversing one element changes nothing");

        LinkedList<int> none;
        none.reverse();
        check(none.is_empty(), "reversing an empty list does not crash");
    }

    // ---- the Rule of Five: a copy must be independent ---------------------
    {
        LinkedList<int> original{1, 2, 3};
        LinkedList<int> copy = original;                 // copy constructor
        copy.add_to_back(4);
        check(holds(original, {1, 2, 3}),    "the original is untouched by the copy");
        check(holds(copy, {1, 2, 3, 4}),     "and the copy has its own nodes");

        LinkedList<int> assigned{9, 9};
        assigned = original;                             // copy assignment
        check(holds(assigned, {1, 2, 3}),    "copy assignment replaces the contents");
        LinkedList<int>& alias = assigned;
        assigned = alias;                                // self-assignment
        check(holds(assigned, {1, 2, 3}),    "self-assignment is harmless");

        LinkedList<int> moved = std::move(copy);         // move constructor
        check(holds(moved, {1, 2, 3, 4}),    "the moved-to list has the nodes");
        check(moved.count() == 4,            "and the size came with them");
        check(copy.is_empty(),               "the moved-from list gave them up");
    }

    // ---- operator[], both versions ---------------------------------------
    {
        LinkedList<int> l{5, 1, 2, 3};
        check(l[0] == 5 && l[3] == 3, "[] reads like get()");
        l[1] = 42;
        check(holds(l, {5, 42, 2, 3}), "[] on a list we own can write");
        const LinkedList<int>& view = l;
        check(view[1] == 42, "[] on a const list reads through the const get()");
        check(throws_out_of_range([](LinkedList<int>& m){ (void) m[4]; }, l),
              "[] past the end throws, as get() does");
    }

    // ---- the iterator ------------------------------------------------------
    {
        LinkedList<int> l{100, 200, 300};
        int expected[] = {100, 200, 300};
        int idx = 0;
        for (auto it = l.begin(); it != l.end(); ++it)
            check(*it == expected[idx++], "the iterator visits in order");
        check(idx == 3, "and stops at end()");

        int sum = 0;
        for (const int v : l) sum += v;
        check(sum == 600, "range-based for walks the whole list");

        for (int& v : l) v += 1;
        check(holds(l, {101, 201, 301}), "*it is a reference, so it can write");

        auto it = l.begin();
        auto old = it++;
        check(*old == 101 && *it == 201, "it++ returns where it was");

        check(std::accumulate(l.begin(), l.end(), 0) == 603,
              "standard algorithms accept it");
        check(std::find(l.begin(), l.end(), 301) != l.end() &&
              std::find(l.begin(), l.end(), 7) == l.end(),
              "std::find finds what is there and only that");
        check(std::distance(l.begin(), l.end()) == 3,
              "std::distance counts the steps, using the five member types");
        std::vector<int> copied(l.begin(), l.end());
        check(copied == std::vector<int>{101, 201, 301},
              "a vector can be built from the list's range");

        LinkedList<int> none;
        check(none.begin() == none.end(), "an empty list has begin() == end()");
    }

    // ---- it really is generic --------------------------------------------
    {
        LinkedList<std::string> words;
        words.add_to_back("hello");
        words.add_to_front("well,");
        check(words.get(0) == "well," && words.get(1) == "hello",
              "the same list holds strings");
    }

    std::printf(failures ? "%d check(s) failed\n" : "all checks pass\n", failures);
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
