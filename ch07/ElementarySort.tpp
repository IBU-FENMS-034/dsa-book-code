#pragma once

// These definitions leave off the top-level const on the
// by-value parameter. It changes nothing about the function
// and it keeps the printed signatures inside the page.

template <typename Data>
void ElementarySort::swap(Data* arr, int a, int b) {
    Data temp = arr[a];
    arr[a] = arr[b];
    arr[b] = temp;
}

template <typename Data>
void ElementarySort::bubble_sort(Data* arr, int len) {
    for (int i = 0; i < len; ++i) {
        bool swapped = false;
        // The last i are home, so the pass stops short.
        for (int j = 1; j < len - i; ++j) {
            // "<", not "<=", so equals never swap.
            if (arr[j] < arr[j - 1]) {
                swap(arr, j, j - 1);
                swapped = true;
            }
        }
        if (!swapped) break;  // no swap: done
    }
}

template <typename Data>
void ElementarySort::selection_sort(Data* arr, int len) {
    for (int i = 0; i < len; ++i) {
        int min = i;            // smallest seen so far
        for (int j = i + 1; j < len; ++j) {
            if (arr[j] < arr[min]) min = j;
        }
        swap(arr, min, i);    // one exchange per i
    }
}

template <typename Data>
void ElementarySort::insertion_sort(Data* arr, int len) {
    for (int i = 1; i < len; ++i) {
        for (int j = i; j > 0; --j) {
            if (arr[j] < arr[j - 1]) swap(arr, j, j - 1);
            else break;       // the rest are smaller
        }
    }
}

template <typename Data>
void ElementarySort::shell_sort(Data* arr, int len) {
    // Climb 1, 4, 13, 40, 121, ... past len / 3.
    int h = 1;
    while (h < len / 3) h = 3 * h + 1;

    while (h >= 1) {
        // Insertion sort again, h apart instead of 1.
        for (int i = h; i < len; ++i) {
            for (int j = i; j >= h; j -= h) {
                if (arr[j] < arr[j - h])
                    swap(arr, j, j - h);
                else break;
            }
        }
        h = h / 3;
    }
}
