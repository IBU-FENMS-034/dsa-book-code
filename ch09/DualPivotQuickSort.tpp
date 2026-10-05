#pragma once

template <typename Data>
std::pair<int, int> DualPivotQuickSort::detail::partition(
        Data* arr, int low, int high) {
    using QuickSort::detail::swap;
    // The pivots are the two ends, smaller one first.
    if (arr[high] < arr[low]) swap(arr, low, high);
    Data left = arr[low], right = arr[high];

    int lt = low + 1;       // below lt: smaller than left
    int gt = high - 1;      // above gt: larger than right
    int k = lt;             // the element being placed

    while (k <= gt) {
        if (arr[k] < left) {
            swap(arr, k, lt);
            ++lt;
            ++k;
        } else if (right < arr[k]) {
            swap(arr, k, gt);
            // The new arr[k] has not been looked at yet.
            --gt;
        } else {
            // Between the pivots, so leave it alone.
            ++k;
        }
    }

    // Bring the pivots in to meet their own parts.
    --lt;
    ++gt;
    swap(arr, low, lt);
    swap(arr, high, gt);
    return {lt, gt};
}

template <typename Data>
void DualPivotQuickSort::detail::sort(
        Data* arr, int low, int high) {
    if (high <= low) return;
    auto [lt, gt] = partition(arr, low, high);
    sort(arr, low, lt - 1);      // below the left pivot
    sort(arr, lt + 1, gt - 1);   // between the pivots
    sort(arr, gt + 1, high);     // above the right pivot
}

template <typename Data>
void DualPivotQuickSort::quick_sort(Data* arr, int len) {
    QuickSort::detail::shuffle(arr, len);
    detail::sort(arr, 0, len - 1);
}
