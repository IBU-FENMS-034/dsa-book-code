#pragma once
#include <algorithm>
#include "MergeSort.h"

inline int TimSort::calculate_run_length(
        const int initial_length, const int threshold) {
    int length = initial_length;
    int remainder = 0;

    // Halve the length until it drops below the threshold. If any halving
    // throws away a remainder, remember it and add it back at the end.
    while (length >= threshold) {
        remainder |= (length & 1);
        length >>= 1;
    }

    return length + remainder;
}

template<typename Data>
void TimSort::insertion_sort(Data *arr, int low, int high) {
    for (int i = low + 1; i <= high; i++) {
        Data key = arr[i];
        int j = i - 1;
        while (j >= low && key < arr[j]) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

template<typename Data>
void TimSort::merge(Data *arr, Data *aux,
                    int low, int mid, int high) {
    MergeSort::merge(arr, aux, low, mid, high);
}

template<typename Data>
void TimSort::sort(Data *arr, int len, int threshold) {
    if (len < 2) {
        return;
    }

    const int run = calculate_run_length(len, threshold);

    // Step 1: sort every run with insertion sort.
    for (int low = 0; low < len; ) {
        const int high = low + std::min(run, len - low) - 1;
        insertion_sort(arr, low, high);
        low = high + 1;
    }

    // Step 2: merge adjacent runs, doubling size each pass.
    Data* aux = new Data[len];
    for (int size = run; size < len; ) {
        for (int low = 0; low < len - size; ) {
            const int mid = low + size - 1;
            const int rest = len - mid - 1;
            const int high = mid + std::min(size, rest);
            merge(arr, aux, low, mid, high);
            low = high + 1;
        }
        if (size > len / 2) break;
        size *= 2;
    }
    delete[] aux;
}
