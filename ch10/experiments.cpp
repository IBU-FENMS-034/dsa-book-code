// Reproduces every measured table in Chapter 10.
//
// Times are from one machine and will differ on yours. The pass counts and the
// memory figures are exact and will not.
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <string>
#include "NonComparisonSort.h"
#include "../ch08/MergeSort.h"
#include "../ch09/QuickSort.h"

using Clock = std::chrono::steady_clock;

static std::vector<int> random_values(int len, int range, unsigned seed) {
    std::vector<int> v(len);
    std::mt19937 rng(seed);
    for (int i = 0; i < len; i++) v[i] = (int) (rng() % (unsigned) range);
    return v;
}

// Radix sort with the base left open, so the chapter can measure what the
// choice of base is worth. The printed version fixes it at ten.
static void radix_base_pass(std::vector<int>& a, long long exp, int base) {
    int len = (int) a.size();
    std::vector<int> aux(len);
    std::vector<int> frequency(base, 0);
    for (int i = 0; i < len; i++) ++frequency[(a[i] / exp) % base];
    for (int i = 1; i < base; i++) frequency[i] += frequency[i - 1];
    for (int i = len - 1; i >= 0; i--)
        aux[--frequency[(a[i] / exp) % base]] = a[i];
    a.swap(aux);
}
static int radix_base(std::vector<int>& a, int base) {
    if (a.size() < 2) return 0;
    int max = *std::max_element(a.begin(), a.end());
    int passes = 0;
    for (long long exp = 1; max / exp > 0; exp *= base) {
        radix_base_pass(a, exp, base);
        passes++;
    }
    return passes;
}

// The fastest of as many runs as fit in a quarter of a second, which is what
// Chapter 7 settled on: everything that varies between runs adds time.
template <typename Fn>
static double time_it(const std::vector<int>& v, Fn run, long long& checksum) {
    double best = 1e30, spent = 0;
    for (int i = 0; i < 40 && spent < 1.5; i++) {
        std::vector<int> a = v;
        auto start = Clock::now();
        run(a);
        auto stop = Clock::now();
        double t = std::chrono::duration<double>(stop - start).count();
        if (t < best) best = t;
        spent += t;
        for (size_t k = 0; k < a.size(); k += 997) checksum += a[k];
    }
    return best;
}

static int digits_of(long long max, int base) {
    int d = 0;
    for (long long exp = 1; max / exp > 0; exp *= base) d++;
    return d ? d : 1;
}

int main() {
    long long checksum = 0;
    QuickSort::seed(31337);

    // ---------------------------------------------------------- TABLE 1 ----
    // The value range stays at a million at every size, so that the number of
    // digits and the size of the count array do not move while N does.
    std::cout << "TABLE 1  Seconds to sort N values drawn from 0 to 1,000,000"
              << "\n";
    std::cout << std::setw(10) << "N";
    for (const char* n : {"merge", "quick", "radix", "counting", "std::sort"})
        std::cout << std::setw(12) << n;
    std::cout << "\n";
    for (int n = 100000; n <= 3200000; n *= 2) {
        std::vector<int> v = random_values(n, 1000000, 90210u);
        std::cout << std::setw(10) << n << std::fixed << std::setprecision(4);
        std::cout << std::setw(12) << time_it(v, [](std::vector<int>& a) {
            MergeSort::merge_sort(a.data(), (int) a.size()); }, checksum);
        std::cout << std::setw(12) << time_it(v, [](std::vector<int>& a) {
            QuickSort::quick_sort(a.data(), (int) a.size()); }, checksum);
        std::cout << std::setw(12) << time_it(v, [](std::vector<int>& a) {
            NonComparisonSort::radix_sort(a.data(), (int) a.size()); },
            checksum);
        std::cout << std::setw(12) << time_it(v, [](std::vector<int>& a) {
            NonComparisonSort::counting_sort(a.data(), (int) a.size()); },
            checksum);
        std::cout << std::setw(12) << time_it(v, [](std::vector<int>& a) {
            std::sort(a.begin(), a.end()); }, checksum);
        std::cout << "\n";
    }

    // ---------------------------------------------------------- TABLE 2 ----
    // What the range costs counting sort. N stays at a million; only the
    // largest value moves.
    std::cout << "\nTABLE 2  Counting sort on a million values, as the range"
              << " grows\n";
    std::cout << std::setw(14) << "largest value" << std::setw(12) << "count KB"
              << std::setw(12) << "counting" << std::setw(12) << "radix"
              << std::setw(12) << "std::sort" << "\n";
    for (long long k = 1000; k <= 10000000LL; k *= 10) {
        std::vector<int> v = random_values(1000000, (int) k, 4242u);
        double kb = (double) (k + 1) * sizeof(int) / 1024;
        std::cout << std::setw(14) << k
                  << std::setw(12) << std::fixed << std::setprecision(0) << kb
                  << std::setprecision(4)
                  << std::setw(12) << time_it(v, [](std::vector<int>& a) {
                        NonComparisonSort::counting_sort(a.data(),
                            (int) a.size()); }, checksum)
                  << std::setw(12) << time_it(v, [](std::vector<int>& a) {
                        NonComparisonSort::radix_sort(a.data(),
                            (int) a.size()); }, checksum)
                  << std::setw(12) << time_it(v, [](std::vector<int>& a) {
                        std::sort(a.begin(), a.end()); }, checksum)
                  << "\n";
    }
    std::cout << "  not run: a largest value of 10^9 needs a count array of "
              << (double) 1000000001LL * sizeof(int) / (1024 * 1024 * 1024)
              << " GB\n";

    // ---------------------------------------------------------- TABLE 3 ----
    // What the number of digits costs radix sort. Same N, wider values.
    std::cout << "\nTABLE 3  Radix sort on a million values, as the values"
              << " get longer\n";
    std::cout << std::setw(14) << "largest value" << std::setw(10) << "digits"
              << std::setw(12) << "radix" << std::setw(12) << "std::sort"
              << std::setw(16) << "s per digit" << "\n";
    for (long long k = 10; k <= 1000000000LL; k *= 10) {
        std::vector<int> v = random_values(1000000, (int) k, 777u);
        int d = digits_of(k - 1, 10);
        double t = time_it(v, [](std::vector<int>& a) {
            NonComparisonSort::radix_sort(a.data(), (int) a.size()); },
            checksum);
        double s = time_it(v, [](std::vector<int>& a) {
            std::sort(a.begin(), a.end()); }, checksum);
        std::cout << std::setw(14) << k << std::setw(10) << d
                  << std::setw(12) << std::fixed << std::setprecision(4) << t
                  << std::setw(12) << s
                  << std::setw(16) << std::setprecision(5) << t / d << "\n";
    }

    // ---------------------------------------------------------- TABLE 4 ----
    // The base is a free parameter, and it trades passes against buckets.
    std::cout << "\nTABLE 4  The same million values, sorted in four bases\n";
    std::cout << std::setw(10) << "base" << std::setw(10) << "buckets"
              << std::setw(10) << "passes" << std::setw(12) << "seconds"
              << "\n";
    {
        std::vector<int> v = random_values(1000000, 1000000000, 55555u);
        for (int base : {10, 16, 256, 65536}) {
            std::vector<int> probe = v;
            int passes = radix_base(probe, base);
            double t = time_it(v, [base](std::vector<int>& a) {
                radix_base(a, base); }, checksum);
            std::cout << std::setw(10) << base << std::setw(10) << base
                      << std::setw(10) << passes
                      << std::setw(12) << std::fixed << std::setprecision(4)
                      << t << "\n";
        }
    }

    // ---------------------------------------------------------- TABLE 5 ----
    // Distinct keys need enough digits to tell them apart, so d cannot stay
    // still while N grows. Section 10.6 is about what that means.
    std::cout << "\nTABLE 5  The fewest digits N distinct keys can have\n";
    std::cout << std::setw(14) << "N" << std::setw(12) << "base 10"
              << std::setw(12) << "base 256" << std::setw(12)
              << "lg N" << "\n";
    for (long long n = 1000; n <= 1000000000LL; n *= 10) {
        int d10 = (int) std::ceil(std::log((double) n) / std::log(10.0));
        int d256 = (int) std::ceil(std::log((double) n) / std::log(256.0));
        std::cout << std::setw(14) << n << std::setw(12) << d10
                  << std::setw(12) << d256 << std::setw(12)
                  << std::fixed << std::setprecision(1)
                  << std::log2((double) n) << "\n";
    }

    std::cout << "\n(checksum " << checksum
              << ", printed so the compiler cannot discard the sorting)\n";
    return 0;
}
