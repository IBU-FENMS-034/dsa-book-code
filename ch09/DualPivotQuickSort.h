#ifndef CH09_DUAL_PIVOT_QUICK_SORT_H
#define CH09_DUAL_PIVOT_QUICK_SORT_H

#include <utility>
#include "QuickSort.h"

// Quicksort with two pivots and three parts, which is Yaroslavskiy's scheme
// and the algorithm Java uses on arrays of primitives.
namespace DualPivotQuickSort {

    template <typename Data>
    void quick_sort(Data* arr, int len);

    namespace detail {
        template <typename Data>
        void sort(Data* arr, int low, int high);

        // Rearrange arr[low..high] around two pivots and return where they
        // ended up, smaller first.
        template <typename Data>
        std::pair<int, int> partition(Data* arr, int low, int high);
    }

}

#include "DualPivotQuickSort.tpp"

#endif
