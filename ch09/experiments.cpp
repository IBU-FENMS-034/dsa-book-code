// Reproduces every measured table in Chapter 9, and prints the data the
// comparison figure is drawn from.
//
// Times are from one machine and will differ on yours. The comparison and
// exchange counts are exact and will not.
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include <algorithm>
#include "QuickSort.h"
#include "DualPivotQuickSort.h"
#include "../ch08/MergeSort.h"

using Clock = std::chrono::steady_clock;

static std::vector<int> shuffled(int len, unsigned seed) {
    std::vector<int> v(len);
    for (int i = 0; i < len; ++i) v[i] = i;
    std::mt19937 rng(seed);
    std::shuffle(v.begin(), v.end(), rng);
    return v;
}

struct Alg {
    const char* name;
    void (*run)(int*, int);
};

static const Alg ALGS[] = {
    {"merge",     [](int* a, int n) { MergeSort::merge_sort(a, n); }},
    {"quick",     [](int* a, int n) { QuickSort::quick_sort(a, n); }},
    {"dual",      [](int* a, int n) { DualPivotQuickSort::quick_sort(a, n); }},
    {"std::sort", [](int* a, int n) { std::sort(a, a + n); }},
};

// The fastest of as many runs as fit in a quarter of a second, which is what
// Chapter 7 settled on: everything that varies between runs adds time.
static double time_sort(const Alg& alg, const std::vector<int>& v,
                        long long& checksum) {
    double best = 1e30, spent = 0;
    for (int run = 0; run < 25 && spent < 0.25; ++run) {
        std::vector<int> a = v;
        auto start = Clock::now();
        alg.run(a.data(), (int) a.size());
        auto stop = Clock::now();
        double t = std::chrono::duration<double>(stop - start).count();
        if (t < best) best = t;
        spent += t;
        for (size_t i = 0; i < a.size(); i += 997) checksum += a[i];
    }
    return best;
}

// Instrumented copies, mirroring the headers with counters added.
struct Count { long long compares = 0, swaps = 0; };

static void sw(std::vector<int>& a, int i, int j, Count& c) {
    std::swap(a[i], a[j]);
    ++c.swaps;
}
static int part_counted(std::vector<int>& a, int low, int high, Count& c) {
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
static void quick_counted(std::vector<int>& a, int low, int high, Count& c) {
    if (high <= low) return;
    int j = part_counted(a, low, high, c);
    quick_counted(a, low, j - 1, c);
    quick_counted(a, j + 1, high, c);
}
static std::pair<int, int> dual_part_counted(std::vector<int>& a, int low,
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
static void dual_counted(std::vector<int>& a, int low, int high, Count& c) {
    if (high <= low) return;
    auto [lt, gt] = dual_part_counted(a, low, high, c);
    dual_counted(a, low, lt - 1, c);
    dual_counted(a, lt + 1, gt - 1, c);
    dual_counted(a, gt + 1, high, c);
}
static int depth_of(std::vector<int>& a, int low, int high) {
    if (high <= low) return 0;
    Count ignore;
    int j = part_counted(a, low, high, ignore);
    return 1 + std::max(depth_of(a, low, j - 1), depth_of(a, j + 1, high));
}

int main() {
    long long checksum = 0;
    QuickSort::seed(90210);

    std::cout << "TABLE 1  Seconds to sort a shuffled array\n";
    std::cout << std::setw(10) << "N";
    for (const Alg& a : ALGS) std::cout << std::setw(13) << a.name;
    std::cout << "\n";
    for (int n = 100000; n <= 3200000; n *= 2) {
        std::vector<int> v = shuffled(n, 4242u);
        std::cout << std::setw(10) << n;
        for (const Alg& alg : ALGS)
            std::cout << std::setw(13) << std::fixed << std::setprecision(4)
                      << time_sort(alg, v, checksum);
        std::cout << "\n";
    }

    std::cout << "\nTABLE 2  Comparisons and exchanges, averaged over 100"
              << " shuffled arrays\n";
    std::cout << std::setw(9) << "N" << std::setw(14) << "partition"
              << std::setw(16) << "compares" << std::setw(16) << "exchanges"
              << std::setw(14) << "cmp / N lg N" << "\n";
    std::mt19937 rng(5150);
    for (int n : {1000, 10000, 100000}) {
        long long c1 = 0, s1 = 0, c2 = 0, s2 = 0;
        for (int t = 0; t < 100; ++t) {
            std::vector<int> a(n);
            for (int i = 0; i < n; ++i) a[i] = i;
            std::shuffle(a.begin(), a.end(), rng);
            std::vector<int> b = a;
            Count x;
            quick_counted(b, 0, n - 1, x);
            c1 += x.compares;
            s1 += x.swaps;
            std::vector<int> d = a;
            Count y;
            dual_counted(d, 0, n - 1, y);
            c2 += y.compares;
            s2 += y.swaps;
        }
        double nlgn = n * std::log2((double) n);
        std::cout << std::setw(9) << n << std::setw(14) << "two-way"
                  << std::setw(16) << c1 / 100 << std::setw(16) << s1 / 100
                  << std::setw(14) << std::setprecision(2) << (c1 / 100.0) / nlgn << "\n";
        std::cout << std::setw(9) << n << std::setw(14) << "dual-pivot"
                  << std::setw(16) << c2 / 100 << std::setw(16) << s2 / 100
                  << std::setw(14) << std::setprecision(2) << (c2 / 100.0) / nlgn << "\n";
    }

    std::cout << "\nTABLE 3  Comparisons on an already sorted array\n";
    std::cout << std::setw(9) << "N" << std::setw(18) << "no shuffle"
              << std::setw(16) << "shuffled" << std::setw(14) << "ratio" << "\n";
    for (int n : {1000, 10000, 100000}) {
        std::vector<int> sorted(n);
        for (int i = 0; i < n; ++i) sorted[i] = i;
        // The unshuffled run cannot be timed: its recursion is N deep and the
        // real stack gives out long before 32,000. Counting it costs nothing
        // and is what the table is about anyway.
        Count bad;
        std::vector<int> b = sorted;
        quick_counted(b, 0, n - 1, bad);
        Count good;
        std::vector<int> g = sorted;
        std::shuffle(g.begin(), g.end(), rng);
        quick_counted(g, 0, n - 1, good);
        std::cout << std::setw(9) << n << std::setw(18) << bad.compares
                  << std::setw(16) << good.compares
                  << std::setw(13) << std::fixed << std::setprecision(0)
                  << (double) bad.compares / good.compares << "x\n";
    }

    std::cout << "\nTABLE 4  How deep the recursion goes, worst of 100 shuffles\n";
    std::cout << std::setw(10) << "N" << std::setw(14) << "deepest"
              << std::setw(12) << "lg N" << std::setw(14) << "2 ln N" << "\n";
    for (int n : {1000, 10000, 100000}) {
        int deepest = 0;
        for (int t = 0; t < 100; ++t) {
            std::vector<int> a(n);
            for (int i = 0; i < n; ++i) a[i] = i;
            std::shuffle(a.begin(), a.end(), rng);
            deepest = std::max(deepest, depth_of(a, 0, n - 1));
        }
        std::cout << std::setw(10) << n << std::setw(14) << deepest
                  << std::setw(12) << std::setprecision(1)
                  << std::log2((double) n)
                  << std::setw(14) << 2 * std::log((double) n) << "\n";
    }

    std::cout << "\n(checksum " << checksum
              << ", printed so the compiler cannot discard the sorting)\n";
    return 0;
}
