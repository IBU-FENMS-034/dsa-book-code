// Chapter 6 verification suite. Every claim the chapter makes about how many
// comparisons a search performs is checked here against the code that performs
// them.
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "Search.h"
#include "ranges.h"

static int failures = 0;

static void check(bool ok, const std::string& what) {
    if (!ok) { std::cout << "  FAIL: " << what << "\n"; ++failures; }
}

// Instrumented copies of the two searches. They mirror Search.tpp exactly,
// with a counter on the line that compares the key against an element.
static long long linear_probes(const int* arr, int len, int key) {
    long long probes = 0;
    for (int i = 0; i < len; ++i) { ++probes; if (arr[i] == key) return probes; }
    return probes;
}

static long long binary_probes(const int* arr, int len, int key) {
    long long probes = 0;
    int low = 0, high = len - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        ++probes;
        if (key == arr[mid]) return probes;
        else if (key < arr[mid]) high = mid - 1;
        else low = mid + 1;
    }
    return probes;
}

int main() {
    std::cout << "Chapter 6 verification suite\n\n";

    // ------------------------------------------------- the worked examples --
    // The two arrays the chapter traces by hand.
    const int unsorted[] = {5, -1, 23, 8, 17, 4, 2, 6};
    check(Search::linear_search(unsorted, 8, 17) == 4, "linear_search finds 17 at 4");
    check(Search::linear_search(unsorted, 8, 34) == -1, "linear_search misses 34");
    check(linear_probes(unsorted, 8, 17) == 5, "17 costs 5 comparisons");
    check(linear_probes(unsorted, 8, 34) == 8, "a miss costs all 8");

    const int sorted[] = {-5, -1, 8, 23, 37, 42, 202, 634};
    check(Search::binary_search(sorted, 8, 37) == 4, "binary_search finds 37 at 4");
    check(Search::binary_search(sorted, 8, 120) == -1, "binary_search misses 120");
    check(binary_probes(sorted, 8, 37) == 3, "37 costs 3 probes");
    check(binary_probes(sorted, 8, 120) == 3, "the miss costs 3 probes");
    std::cout << "  8 elements: 37 found in " << binary_probes(sorted, 8, 37)
              << " probes, 120 ruled out in " << binary_probes(sorted, 8, 120) << "\n";

    // ------------------------------------------------------ the agreement --
    // Binary search must give the same answer as linear search, on every key
    // that is present and on a spread of keys that are not.
    for (int len = 0; len <= 200; ++len) {
        std::vector<int> v(len);
        for (int i = 0; i < len; ++i) v[i] = 3 * i;         // sorted, gaps of 3
        for (int key = -2; key <= 3 * len + 2; ++key) {
            int want = Search::linear_search(v.data(), len, key);
            check(Search::binary_search(v.data(), len, key) == want,
                  "binary_search agrees at len " + std::to_string(len));
            check(Search::binary_search_recursive(v.data(), len, key) == want,
                  "the recursion agrees at len " + std::to_string(len));
        }
    }

    // ------------------------------------------------------- the bounds ----
    // No search of an array of length N ever costs more than floor(lg N) + 1
    // probes, and some search of it costs exactly that.
    for (int len = 1; len <= 1024; ++len) {
        std::vector<int> v(len);
        for (int i = 0; i < len; ++i) v[i] = 2 * i;
        long long worst = 0;
        for (int key = -1; key <= 2 * len; ++key)
            worst = std::max(worst, binary_probes(v.data(), len, key));
        long long bound = (long long) std::floor(std::log2((double) len)) + 1;
        check(worst == bound, "worst case at len " + std::to_string(len)
                              + " is floor(lg N) + 1");
    }
    std::vector<int> thousand(1000);
    for (int i = 0; i < 1000; ++i) thousand[i] = 2 * i;
    long long worst_1000 = 0;
    for (int key = -1; key <= 2000; ++key)
        worst_1000 = std::max(worst_1000, binary_probes(thousand.data(), 1000, key));
    std::cout << "  worst case at N = 1000 is " << worst_1000
              << " probes, and linear search costs up to 1000\n";

    // The average over a successful search of a full array of 2^k - 1
    // elements is ((k-1)2^k + 1) / (2^k - 1), which is about lg N minus one.
    for (int k = 1; k <= 14; ++k) {
        int len = (1 << k) - 1;
        std::vector<int> v(len);
        for (int i = 0; i < len; ++i) v[i] = i;
        long long total = 0;
        for (int i = 0; i < len; ++i) total += binary_probes(v.data(), len, i);
        long long formula = (long long) (k - 1) * (1LL << k) + 1;
        check(total == formula, "successful-search total at k = " + std::to_string(k));
        if (k == 10)
            std::cout << "  N = " << len << ": a successful search averages "
                      << (double) total / len << " probes, and lg N is "
                      << std::log2((double) len) << "\n";
    }

    // ------------------------------------------------------ floor_index ----
    // Checked against a scan, which is slow and obviously correct.
    for (int len = 0; len <= 120; ++len) {
        std::vector<int> v(len);
        for (int i = 0; i < len; ++i) v[i] = 5 * i;
        for (int key = -3; key <= 5 * len + 3; ++key) {
            int want = -1;
            for (int i = 0; i < len; ++i) if (v[i] <= key) want = i;
            check(Search::floor_index(v.data(), len, key) == want,
                  "floor_index at len " + std::to_string(len));
        }
    }

    // ------------------------------------------------------- the ranges ----
    check(Ranges::to_number("0.0.0.0") == 0, "0.0.0.0 is 0");
    check(Ranges::to_number("1.0.0.0") == 16777216, "1.0.0.0 is 16777216");
    check(Ranges::to_number("202.186.13.4") == 3401190660L, "the lab's example");
    check(Ranges::to_number("255.255.255.255") == 4294967295L, "the last address");
    std::cout << "  202.186.13.4 is address number "
              << Ranges::to_number("202.186.13.4") << "\n";

    // Four blocks taken from the shape of the real file, with a gap after the
    // second one so that a key can fall between two ranges.
    const long starts[] = {16777216, 16777472, 16778240, 16778496};
    const long ends[]   = {16777471, 16778239, 16778495, 16778751};
    check(Ranges::find(starts, ends, 4, 16777300) == 0, "inside the first block");
    check(Ranges::find(starts, ends, 4, 16777471) == 0, "on the first block's last address");
    check(Ranges::find(starts, ends, 4, 16778751) == 3, "on the last address of all");
    check(Ranges::find(starts, ends, 4, 16777100) == -1, "before every block");
    check(Ranges::find(starts, ends, 4, 16779000) == -1, "after every block");
    // A key in the hole between two ranges is found by neither.
    const long gapped_s[] = {10, 30};
    const long gapped_e[] = {19, 39};
    check(Ranges::find(gapped_s, gapped_e, 2, 25) == -1, "in the gap between blocks");
    check(Ranges::find(gapped_s, gapped_e, 2, 19) == 0, "on the edge before the gap");
    check(Ranges::find(gapped_s, gapped_e, 2, 30) == 1, "on the edge after the gap");

    std::cout << "\n";
    if (failures == 0) std::cout << "Chapter 6 verification suite: all checks pass\n";
    else std::cout << failures << " check(s) failed\n";
    return failures == 0 ? 0 : 1;
}
