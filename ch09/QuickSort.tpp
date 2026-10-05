#pragma once

inline std::mt19937& QuickSort::generator() {
    static std::mt19937 gen(std::random_device{}());
    return gen;
}

inline void QuickSort::seed(unsigned value) {
    generator().seed(value);
}

template <typename Data>
void QuickSort::detail::swap(Data* arr, int i, int j) {
    Data temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

// Fisher and Yates, as Knuth gives it: position i gets one of the elements
// from i onwards, chosen uniformly, so every ordering is equally likely.
template <typename Data>
void QuickSort::detail::shuffle(Data* arr, int len) {
    for (int i = 0; i < len; ++i) {
        std::uniform_int_distribution<int> pick(i, len - 1);
        swap(arr, i, pick(generator()));
    }
}

template <typename Data>
int QuickSort::detail::partition(
        Data* arr, int low, int high) {
    // Start one before the range and one after it.
    int i = low, j = high + 1;
    while (true) {
        // Walk i right past everything below the pivot.
        while (arr[++i] < arr[low])
            if (i == high) break;
        // Walk j left past everything larger than it.
        while (arr[low] < arr[--j])
            if (j == low) break;    // cannot fire: see below
        if (i >= j) break;          // the two have met
        // Both are on the wrong side, so exchange them.
        swap(arr, i, j);
    }
    // Put the pivot between the two halves.
    swap(arr, low, j);
    return j;
}

template <typename Data>
void QuickSort::detail::sort(Data* arr, int low, int high) {
    if (high <= low) return;        // nothing, or one element
    int j = partition(arr, low, high);
    sort(arr, low, j - 1);          // everything smaller
    sort(arr, j + 1, high);         // everything larger
}

template <typename Data>
void QuickSort::quick_sort(Data* arr, int len) {
    detail::shuffle(arr, len);
    detail::sort(arr, 0, len - 1);
}
