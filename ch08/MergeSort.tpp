#pragma once
#include <algorithm>

template<typename Data>
void MergeSort::merge_sort(Data *arr, int len) {
    Data* aux = new Data[len];
    merge_sort(arr, aux, 0, len - 1);
    delete[] aux;
}

template<typename Data>
void MergeSort::merge_sort(Data *arr, Data *aux,
                           int low, int high) {
    if (high <= low) {
        return;
    }

    int mid = low + (high - low) / 2;
    merge_sort(arr, aux, low, mid);
    merge_sort(arr, aux, mid + 1, high);
    merge(arr, aux, low, mid, high);
}

template<typename Data>
void MergeSort::bottom_up_merge_sort(Data *arr, int len) {
    Data* aux = new Data[len];
    for (int size = 1; size < len; ) {
        for (int low = 0; low < len - size; ) {
            int mid = low + size - 1;
            int high = mid + std::min(size, len - mid - 1);
            merge(arr, aux, low, mid, high);
            low = high + 1;
        }
        if (size > len / 2) break;
        size *= 2;
    }
    delete[] aux;
}

template<typename Data>
void MergeSort::merge(Data *arr, Data *aux,
                      int low, int mid, int high) {
    for (int k = low; k <= high; k++) {
        aux[k] = arr[k];
    }

    int i = low;
    int j = mid + 1;
    for (int k = low; k <= high; k++) {
        if (i > mid) {
            arr[k] = aux[j++];
        } else if (j > high) {
            arr[k] = aux[i++];
        } else if (aux[j] < aux[i]) {
            arr[k] = aux[j++];
        } else {
            arr[k] = aux[i++];
        }
    }
}
