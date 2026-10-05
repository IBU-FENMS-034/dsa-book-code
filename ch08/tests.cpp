// Verification suite for the Chapter 8 listings.
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <cassert>
#include "MergeSort.h"
#include "TimSort.h"
#include "MergeSortOptimised.h"

static int failures = 0;
static int checks   = 0;

void check(bool ok, const std::string& what) {
    checks++;
    if (!ok) { failures++; std::cout << "  FAIL: " << what << "\n"; }
}

// A record whose ordering ignores the tag, so we can observe stability.
struct Record {
    int key;
    int tag;
    bool operator<(const Record& o) const { return key < o.key; }
};

template<typename F>
void test_sorts_correctly(const std::string& name, F sorter) {
    std::mt19937 rng(12345);
    for (int n : {0, 1, 2, 3, 5, 7, 8, 16, 17, 64, 100, 511, 1000}) {
        std::vector<int> v(n);
        std::uniform_int_distribution<int> d(-1000, 1000);
        for (int& x : v) x = d(rng);
        std::vector<int> expected = v;
        std::sort(expected.begin(), expected.end());
        sorter(v.data(), n);
        check(v == expected, name + " random n=" + std::to_string(n));
    }
    // Adversarial shapes.
    for (int n : {1, 2, 9, 64, 257}) {
        std::vector<int> asc(n), desc(n), same(n, 42);
        for (int i = 0; i < n; i++) { asc[i] = i; desc[i] = n - i; }
        std::vector<int> a = asc, b = desc, c = same;
        sorter(a.data(), n); sorter(b.data(), n); sorter(c.data(), n);
        check(std::is_sorted(a.begin(), a.end()), name + " ascending n=" + std::to_string(n));
        check(std::is_sorted(b.begin(), b.end()), name + " descending n=" + std::to_string(n));
        check(std::is_sorted(c.begin(), c.end()), name + " all-equal n=" + std::to_string(n));
    }
}

template<typename F>
void test_is_stable(const std::string& name, F sorter) {
    // 400 records over only 20 distinct keys: ties are guaranteed.
    const int n = 400;
    std::vector<Record> v(n);
    for (int i = 0; i < n; i++) v[i] = Record{i % 20, i};
    sorter(v.data(), n);
    bool stable = true;
    for (int i = 1; i < n; i++)
        if (v[i-1].key == v[i].key && v[i-1].tag > v[i].tag) stable = false;
    check(stable, name + " is stable");
    check(std::is_sorted(v.begin(), v.end(),
          [](const Record& a, const Record& b){ return a.key < b.key; }),
          name + " sorted the records");
}

int main() {
    std::cout << "Chapter 8 verification suite\n";

    test_sorts_correctly("top-down  ", [](int* a, int n){ MergeSort::merge_sort(a, n); });
    test_sorts_correctly("bottom-up ", [](int* a, int n){ MergeSort::bottom_up_merge_sort(a, n); });
    test_sorts_correctly("timsort   ", [](int* a, int n){ TimSort::sort(a, n, 64); });
    test_sorts_correctly("optimised ", [](int* a, int n){ MergeSortOptimised::merge_sort(a, n); });

    test_is_stable("top-down  ", [](Record* a, int n){ MergeSort::merge_sort(a, n); });
    test_is_stable("bottom-up ", [](Record* a, int n){ MergeSort::bottom_up_merge_sort(a, n); });
    test_is_stable("timsort   ", [](Record* a, int n){ TimSort::sort(a, n, 64); });
    test_is_stable("optimised ", [](Record* a, int n){ MergeSortOptimised::merge_sort(a, n); });

    // The worked example from the lecture: n = 19, threshold = 8 -> run = 5.
    check(TimSort::calculate_run_length(19, 8) == 5, "calculate_run_length(19, 8) == 5");
    check(TimSort::calculate_run_length(64, 64) == 32, "calculate_run_length(64, 64) == 32");
    check(TimSort::calculate_run_length(7, 8) == 7,  "calculate_run_length(7, 8) == 7 (single run)");
    check(TimSort::calculate_run_length(1000, 64) == 63, "calculate_run_length(1000, 64) == 63");

    // The merge example used throughout the chapter.
    {
        int arr[] = {27, 38, 3, 43};
        int aux[4];
        MergeSort::merge(arr, aux, 0, 1, 3);
        int want[] = {3, 27, 38, 43};
        check(std::equal(arr, arr + 4, want), "merge [27,38] with [3,43] -> [3,27,38,43]");
    }

    // The demo array from the lab.
    {
        int arr[] = {5, -1, 23, 8, 17, 4, 2, 6};
        MergeSort::merge_sort(arr, 8);
        int want[] = {-1, 2, 4, 5, 6, 8, 17, 23};
        check(std::equal(arr, arr + 8, want), "lab demo array sorts correctly");
    }

    std::cout << "\n" << (checks - failures) << "/" << checks << " checks passed\n";
    return failures == 0 ? 0 : 1;
}
