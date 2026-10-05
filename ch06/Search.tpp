#pragma once

template <typename Data>
int Search::linear_search(const Data* arr, const int len, const Data& key) {
    for (int i = 0; i < len; ++i) {
        if (arr[i] == key) return i;      // found it, and i is the answer
    }
    return -1;                            // fell off the end, so it is absent
}

template <typename Data>
int Search::binary_search(const Data* arr, const int len, const Data& key) {
    int low = 0, high = len - 1;
    while (low <= high) {                 // while the interval is not empty
        int mid = low + (high - low) / 2; // never low + high: see Section 6.3.3
        if (key == arr[mid])      return mid;
        else if (key < arr[mid])  high = mid - 1;   // it can only be lower
        else                      low  = mid + 1;   // it can only be higher
    }
    return -1;
}

template <typename Data>
int Search::detail::bsearch(const Data* arr, const int low, const int high,
                            const Data& key) {
    if (low > high) return -1;            // the interval is empty
    int mid = low + (high - low) / 2;
    if (key == arr[mid])     return mid;
    else if (key < arr[mid]) return bsearch(arr, low, mid - 1, key);
    else                     return bsearch(arr, mid + 1, high, key);
}

template <typename Data>
int Search::binary_search_recursive(const Data* arr, const int len,
                                    const Data& key) {
    return detail::bsearch(arr, 0, len - 1, key);
}

template <typename Data>
int Search::floor_index(const Data* arr, const int len, const Data& key) {
    int low = 0, high = len - 1, best = -1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid] <= key) {
            best = mid;                   // a candidate, but look for a later one
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return best;
}
