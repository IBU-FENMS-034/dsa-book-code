#ifndef CH09_QUICK_SORT_H
#define CH09_QUICK_SORT_H

#include <random>

// Quicksort with Sedgewick's two-way partition, and the shuffle that turns its
// worst case from a property of the caller's data into a matter of luck.
namespace QuickSort {

    // Shuffle, then sort the whole array. This is the only entry point.
    template <typename Data>
    void quick_sort(Data* arr, int len);

    // The generator the shuffle draws from, seeded once. seed() exists so a
    // test can ask for the same shuffle twice; nothing else should call it.
    std::mt19937& generator();
    void seed(unsigned value);

    namespace detail {
        template <typename Data>
        void shuffle(Data* arr, int len);

        // Sort arr[low..high], inclusive at both ends.
        template <typename Data>
        void sort(Data* arr, int low, int high);

        // Rearrange arr[low..high] around arr[low] and return where the
        // pivot ended up, with nothing larger to its left and nothing
        // smaller to its right.
        template <typename Data>
        int partition(Data* arr, int low, int high);

        template <typename Data>
        void swap(Data* arr, int i, int j);
    }

}

#include "QuickSort.tpp"

#endif
