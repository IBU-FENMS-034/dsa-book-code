#ifndef CH06_SEARCH_H
#define CH06_SEARCH_H

// The searching algorithms of Chapter 6. Every one of them answers the same
// question, "where is this key", and returns -1 when the answer is nowhere.
namespace Search {

    // Works on any array. Looks at every element until it finds the key.
    template <typename Data>
    int linear_search(const Data* arr, int len, const Data& key);

    // Needs the array to be sorted. Halves the interval at every step.
    template <typename Data>
    int binary_search(const Data* arr, int len, const Data& key);

    // The same algorithm written as a recursion, for comparison.
    template <typename Data>
    int binary_search_recursive(const Data* arr, int len, const Data& key);

    // The index of the largest element that does not exceed the key, or -1 if
    // every element is larger. An exact match is the special case where the
    // element found happens to equal the key.
    template <typename Data>
    int floor_index(const Data* arr, int len, const Data& key);

    namespace detail {
        template <typename Data>
        int bsearch(const Data* arr, int low, int high, const Data& key);
    }

}

#include "Search.tpp"

#endif
