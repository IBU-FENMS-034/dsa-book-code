// Chapter 9 verification suite. Both partitions are checked against the
// property they promise, both sorts against std::sort, and every count the
// chapter prints is checked against the code that produces it.
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include "QuickSort.h"
#include "DualPivotQuickSort.h"

static int failures = 0;

static void check(bool ok, const std::string& what) {
    if (!ok) { std::cout << "  FAIL: " << what << "\n"; ++failures; }
}

struct Tagged {
    int key, tag;
    bool operator<(const Tagged& o) const { return key < o.key; }
};

// Instrumented copies of the two partitions and the two sorts. They mirror the
// headers with counters added on the comparison and the exchange.
struct Count { long long compares = 0, swaps = 0; };

static void sw(std::vector<int>& a, int i, int j, Count& c) {
    std::swap(a[i], a[j]);
    ++c.swaps;
}

static int partition_counted(std::vector<int>& a, int low, int high, Count& c) {
    int i = low, j = high + 1;
    while (true) {
        while (++c.compares, a[++i] < a[low]) if (i == high) break;
        while (++c.compares, a[low] < a[--j]) if (j == low) break;
        if (i >= j) break;
        sw(a, i, j, c);
    }
    sw(a, low, j, c);
    return j;
}

static void sort_counted(std::vector<int>& a, int low, int high, Count& c) {
    if (high <= low) return;
    int j = partition_counted(a, low, high, c);
    sort_counted(a, low, j - 1, c);
    sort_counted(a, j + 1, high, c);
}

static std::pair<int, int> dual_partition_counted(std::vector<int>& a, int low,
                                                  int high, Count& c) {
    ++c.compares;
    if (a[high] < a[low]) sw(a, low, high, c);
    int left = a[low], right = a[high];
    int lt = low + 1, gt = high - 1, k = lt;
    while (k <= gt) {
        ++c.compares;
        if (a[k] < left) { sw(a, k, lt, c); ++lt; ++k; }
        else {
            ++c.compares;
            if (right < a[k]) { sw(a, k, gt, c); --gt; }
            else ++k;
        }
    }
    --lt; ++gt;
    sw(a, low, lt, c);
    sw(a, high, gt, c);
    return {lt, gt};
}

static void dual_sort_counted(std::vector<int>& a, int low, int high, Count& c) {
    if (high <= low) return;
    auto [lt, gt] = dual_partition_counted(a, low, high, c);
    dual_sort_counted(a, low, lt - 1, c);
    dual_sort_counted(a, lt + 1, gt - 1, c);
    dual_sort_counted(a, gt + 1, high, c);
}

// How deep the recursion goes, which is the space the sort uses.
static int depth_counted(std::vector<int>& a, int low, int high) {
    if (high <= low) return 0;
    Count ignore;
    int j = partition_counted(a, low, high, ignore);
    return 1 + std::max(depth_counted(a, low, j - 1),
                        depth_counted(a, j + 1, high));
}

int main() {
    std::cout << "Chapter 9 verification suite\n\n";
    std::mt19937 rng(31);

    // ------------------------------------------------ partition promises ---
    // After partition, the pivot is home: nothing to its left is larger and
    // nothing to its right is smaller. That is Proposition 9.1.
    for (int len = 1; len <= 120; ++len) {
        for (int kind = 0; kind < 4; ++kind) {
            std::vector<int> a(len);
            for (int i = 0; i < len; ++i) {
                if (kind == 0) a[i] = (int) (rng() % 1000);
                else if (kind == 1) a[i] = i;
                else if (kind == 2) a[i] = len - i;
                else a[i] = (int) (rng() % 3);
            }
            std::vector<int> b = a;
            Count c;
            int j = partition_counted(a, 0, len - 1, c);
            check(0 <= j && j < len, "partition returns an index in range");
            for (int i = 0; i < j; ++i)
                check(!(a[j] < a[i]), "nothing left of the pivot is larger");
            for (int i = j + 1; i < len; ++i)
                check(!(a[i] < a[j]), "nothing right of the pivot is smaller");
            std::sort(a.begin(), a.end());
            std::sort(b.begin(), b.end());
            check(a == b, "partition rearranges, and loses nothing");

            if (len >= 2) {
                std::vector<int> d = b;
                std::shuffle(d.begin(), d.end(), rng);
                std::vector<int> keep = d;
                Count dc;
                auto [lt, gt] = dual_partition_counted(d, 0, len - 1, dc);
                check(lt <= gt, "the left pivot lands at or before the right");
                for (int i = 0; i < lt; ++i)
                    check(d[i] < d[lt], "part one is below the left pivot");
                for (int i = lt + 1; i < gt; ++i)
                    check(!(d[i] < d[lt]) && !(d[gt] < d[i]),
                          "part two is between the pivots");
                for (int i = gt + 1; i < len; ++i)
                    check(d[gt] < d[i], "part three is above the right pivot");
                std::sort(d.begin(), d.end());
                std::sort(keep.begin(), keep.end());
                check(d == keep, "dual-pivot partition loses nothing");
            }
        }
    }

    // ------------------------------------------------------ both do sort ---
    QuickSort::seed(2026);
    for (int len = 0; len <= 200; ++len) {
        for (int kind = 0; kind < 4; ++kind) {
            std::vector<int> base(len);
            for (int i = 0; i < len; ++i) {
                if (kind == 0) base[i] = (int) (rng() % 1000);
                else if (kind == 1) base[i] = i;
                else if (kind == 2) base[i] = len - i;
                else base[i] = (int) (rng() % 3);
            }
            std::vector<int> want = base;
            std::sort(want.begin(), want.end());
            std::vector<int> v = base;
            QuickSort::quick_sort(v.data(), len);
            check(v == want, "quick_sort at length " + std::to_string(len));
            v = base;
            DualPivotQuickSort::quick_sort(v.data(), len);
            check(v == want, "dual-pivot at length " + std::to_string(len));
        }
    }
    for (int n : {1000, 5000, 20001}) {
        std::vector<int> base(n);
        for (int& x : base) x = (int) (rng() % 100000);
        std::vector<int> want = base;
        std::sort(want.begin(), want.end());
        std::vector<int> v = base;
        QuickSort::quick_sort(v.data(), n);
        check(v == want, "quick_sort at " + std::to_string(n));
        v = base;
        DualPivotQuickSort::quick_sort(v.data(), n);
        check(v == want, "dual-pivot at " + std::to_string(n));
    }

    // -------------------------------------------------------- the shuffle --
    // Every position should receive every element about equally often. With
    // 6 elements and 60,000 shuffles each cell expects 10,000.
    QuickSort::seed(7);
    const int M = 6, TRIALS = 60000;
    std::vector<std::vector<int>> tally(M, std::vector<int>(M, 0));
    for (int t = 0; t < TRIALS; ++t) {
        std::vector<int> a(M);
        for (int i = 0; i < M; ++i) a[i] = i;
        QuickSort::detail::shuffle(a.data(), M);
        for (int pos = 0; pos < M; ++pos) tally[pos][a[pos]]++;
    }
    int worst = 0;
    for (int p = 0; p < M; ++p)
        for (int e = 0; e < M; ++e)
            worst = std::max(worst, std::abs(tally[p][e] - TRIALS / M));
    check(worst < 0.06 * (double) TRIALS / M,
          "the shuffle is uniform to within six per cent");
    std::cout << "  shuffle: worst cell is " << worst << " away from "
              << TRIALS / M << ", over " << TRIALS << " shuffles\n";

    // ------------------------------------------ the worst case, unshuffled --
    // Sorted input with a first-element pivot gives one partition per element.
    for (int n : {50, 200, 500}) {
        std::vector<int> a(n);
        for (int i = 0; i < n; ++i) a[i] = i;
        Count c;
        std::vector<int> b = a;
        sort_counted(b, 0, n - 1, c);
        check(c.compares >= (long long) n * (n - 1) / 2,
              "sorted input costs at least N(N-1)/2 comparisons");
        std::vector<int> d = a;
        check(depth_counted(d, 0, n - 1) == n - 1,
              "sorted input recurses N-1 deep");
    }
    std::vector<int> thousand(1000);
    for (int i = 0; i < 1000; ++i) thousand[i] = i;
    Count worst_c;
    std::vector<int> copy = thousand;
    sort_counted(copy, 0, 999, worst_c);
    std::cout << "  1000 sorted elements, no shuffle: " << worst_c.compares
              << " comparisons and a recursion 999 deep\n";

    // ------------------------------------------------ the average, counted --
    // Sedgewick's Proposition K: about 2N ln N comparisons, which is about
    // 1.39 N lg N. Averaged over a hundred shuffled arrays.
    for (int n : {1000, 10000}) {
        long long total = 0, swaps = 0, dual_total = 0, dual_swaps = 0;
        int deepest = 0;
        for (int t = 0; t < 100; ++t) {
            std::vector<int> a(n);
            for (int i = 0; i < n; ++i) a[i] = i;
            std::shuffle(a.begin(), a.end(), rng);
            std::vector<int> b = a;
            Count c;
            sort_counted(b, 0, n - 1, c);
            total += c.compares;
            swaps += c.swaps;
            std::vector<int> e = a;
            deepest = std::max(deepest, depth_counted(e, 0, n - 1));
            std::vector<int> d = a;
            Count dc;
            dual_sort_counted(d, 0, n - 1, dc);
            dual_total += dc.compares;
            dual_swaps += dc.swaps;
        }
        double avg = (double) total / 100;
        // Sedgewick's Proposition K, exactly: 2(N+1)(H(N+1) - 3/2), where H is
        // the harmonic number. The familiar 2N ln N is that with the smaller
        // terms dropped, and it overstates the count by about 10% at this size.
        double harmonic = 0;
        for (int i = 1; i <= n + 1; ++i) harmonic += 1.0 / i;
        double model = 2.0 * (n + 1) * (harmonic - 1.5);
        double loose = 2.0 * n * std::log((double) n);
        check(std::fabs(avg - model) / model < 0.05,
              "average comparisons are within 5% of Sedgewick's formula");
        if (n == 10000)
            std::cout << "  10,000 shuffled elements, averaged over 100 runs:\n"
                      << "    two-way  " << (long long) avg << " comparisons, "
                      << swaps / 100 << " exchanges\n"
                      << "    predicted " << (long long) model
                      << " by 2(N+1)(H-3/2), and " << (long long) loose
                      << " by the looser 2N ln N\n"
                      << "    dual     " << dual_total / 100 << " comparisons, "
                      << dual_swaps / 100 << " exchanges\n"
                      << "    deepest recursion seen: " << deepest
                      << ", and lg N is " << (int) std::log2((double) n) << "\n";
    }

    // ---------------------------------------------------- not stable -------
    // The pivot swap jumps over elements it never compared itself with.
    std::vector<Tagged> ties(400);
    for (int i = 0; i < 400; ++i) ties[i] = {(int) (rng() % 8), i};
    QuickSort::seed(11);
    std::vector<Tagged> v = ties;
    QuickSort::quick_sort(v.data(), 400);
    bool stable = true;
    for (int i = 1; i < 400; ++i)
        if (v[i - 1].key == v[i].key && v[i - 1].tag > v[i].tag) stable = false;
    check(!stable, "quicksort is not stable on this input");

    std::cout << "\n";
    if (failures == 0) std::cout << "Chapter 9 verification suite: all checks pass\n";
    else std::cout << failures << " check(s) failed\n";
    return failures == 0 ? 0 : 1;
}
