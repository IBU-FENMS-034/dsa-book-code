#ifndef MERGESORTOPTIMISED_H
#define MERGESORTOPTIMISED_H

// The practical improvements from the merge sort chapter: cut off to
// insertion sort on small subarrays, and skip merges that change nothing.
namespace MergeSortOptimised {
    static const int CUTOFF = 7;

    template<typename Data>
    void merge_sort(Data* arr, int len);

    template<typename Data>
    static void merge_sort(Data* arr, Data* aux, int low, int high);

    template<typename Data>
    static void insertion_sort(Data* arr, int low, int high);
}

#include "MergeSortOptimised.tpp"

#endif //MERGESORTOPTIMISED_H
