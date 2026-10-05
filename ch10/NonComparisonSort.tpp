#pragma once
#include <cassert>

template <typename Data>
Data NonComparisonSort::detail::get_max(Data* arr, int len) {
    Data max = arr[0];
    for (int i = 1; i < len; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

template <typename Data>
Data NonComparisonSort::detail::get_min(Data* arr, int len) {
    Data min = arr[0];
    for (int i = 1; i < len; i++) {
        if (arr[i] < min) {
            min = arr[i];
        }
    }
    return min;
}

template <typename Data>
void NonComparisonSort::counting_sort(Data* arr, int len) {
    if (len < 2) return;
    Data max = detail::get_max(arr, len);
    // A negative key would index outside the array, so check.
    assert(detail::get_min(arr, len) >= 0 && "non-negative");

    int* count = new int[max + 1]();    // one slot per value
    Data* output = new Data[len];

    // How many of each value are there?
    for (int i = 0; i < len; i++) {
        ++count[arr[i]];
    }

    // Running totals: count[v] becomes the number of elements
    // no larger than v, one past where the last v belongs.
    for (int i = 1; i <= max; i++) {
        count[i] += count[i - 1];
    }

    // Backwards, so equal elements keep their original order.
    for (int i = len - 1; i >= 0; i--) {
        output[--count[arr[i]]] = arr[i];
    }

    for (int i = 0; i < len; i++) {
        arr[i] = output[i];
    }

    delete[] count;
    delete[] output;
}

template <typename Data>
void NonComparisonSort::detail::sort(Data* arr, int len,
                                     int exp) {
    Data* aux = new Data[len];
    int frequency[10] = {0};

    // How many elements fall in each of the ten buckets?
    for (int i = 0; i < len; i++) {
        int digit = (arr[i] / exp) % 10;
        ++frequency[digit];
    }

    // Running totals: frequency[d] is where bucket d ends.
    for (int i = 1; i < 10; i++) {
        frequency[i] += frequency[i - 1];
    }

    // Backwards, so this pass does not undo the previous one.
    for (int i = len - 1; i >= 0; i--) {
        int digit = (arr[i] / exp) % 10;
        aux[--frequency[digit]] = arr[i];
    }

    for (int i = 0; i < len; i++) {
        arr[i] = aux[i];
    }

    delete[] aux;
}

template <typename Data>
void NonComparisonSort::radix_sort(Data* arr, int len) {
    if (len < 2) return;
    Data max = detail::get_max(arr, len);
    assert(detail::get_min(arr, len) >= 0 && "non-negative");

    // One pass per digit of the largest element, units first.
    for (int exp = 1; max / exp > 0; exp *= 10) {
        detail::sort(arr, len, exp);
    }
}
