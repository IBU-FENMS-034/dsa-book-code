#pragma once
#include <algorithm>
#include "MergeSort.h"

template<typename Data>
void MergeSortOptimised::insertion_sort(Data *arr, int low, int high) {
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
void MergeSortOptimised::merge_sort(Data *arr, Data *aux,
                                    int low, int high) {
    // Improvement 1: small subarrays go to insertion sort.
    if (high <= low + CUTOFF - 1) {
        insertion_sort(arr, low, high);
        return;
    }

    int mid = low + (high - low) / 2;
    merge_sort(arr, aux, low, mid);
    merge_sort(arr, aux, mid + 1, high);

    // Improvement 2: halves already in order means nothing to do.
    if (!(arr[mid + 1] < arr[mid])) {
        return;
    }

    MergeSort::merge(arr, aux, low, mid, high);
}

template<typename Data>
void MergeSortOptimised::merge_sort(Data *arr, int len) {
    if (len < 2) {
        return;
    }
    Data* aux = new Data[len];
    merge_sort(arr, aux, 0, len - 1);
    delete[] aux;
}
