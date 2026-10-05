#ifndef CH07_ELEMENTARY_SORT_H
#define CH07_ELEMENTARY_SORT_H

// The four elementary sorts. All of them sort in place, all of them return
// nothing, and all of them take the array as a pointer plus its length,
// because a pointer carries no length of its own.
namespace ElementarySort {

    // Every one of the four moves elements through this one helper, so the
    // exchange count is a fair comparison between them. It is static, so it
    // stays inside this translation unit.
    template <typename Data>
    static void swap(Data* arr, int a, int b);

    // Repeatedly carry the largest remaining element to the end, one
    // neighbouring pair at a time. Stops early if a pass finds nothing to do.
    template <typename Data>
    void bubble_sort(Data* arr, int len);

    // Repeatedly find the smallest remaining element and put it in place.
    template <typename Data>
    void selection_sort(Data* arr, int len);

    // Grow a sorted prefix by pushing each new element left past everything
    // larger than it.
    template <typename Data>
    void insertion_sort(Data* arr, int len);

    // Insertion sort over elements h apart, for a decreasing sequence of h
    // that ends at 1.
    template <typename Data>
    void shell_sort(Data* arr, int len);

}

#include "ElementarySort.tpp"

#endif
