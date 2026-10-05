// Reproduces every measured table in Chapter 6, and prints the data the
// break-even figure is drawn from.
//
// Times are from one machine and will differ on yours. The comparison counts
// and the crossover point are exact and will not.
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>
#include "Search.h"

using Clock = std::chrono::steady_clock;

// A search is far too fast to time once, so each measurement runs a whole
// batch of them and the batch is timed. The keys are drawn in advance so that
// generating them is not part of what the clock sees.
// Binary search is so much faster that one batch of it finishes inside the
// clock's own noise, so the cheap search runs its batch many times over and
// the result is divided back down. Repeating changes neither the ratio nor the
// time per query, which is what the table is about.
template <typename Fn>
static double time_batch(Fn fn, const std::vector<int>& keys, int repeats,
                         long long& found) {
    auto start = Clock::now();
    for (int r = 0; r < repeats; ++r)
        for (int key : keys) if (fn(key) >= 0) ++found;
    auto stop = Clock::now();
    return std::chrono::duration<double>(stop - start).count() / repeats;
}

int main() {
    const int QUERIES = 50000;

    std::cout << "TABLE 1  Doubling test, one search of a sorted array\n";
    std::cout << std::setw(10) << "N" << std::setw(14) << "linear (s)"
              << std::setw(9) << "ratio" << std::setw(14) << "binary (s)"
              << std::setw(9) << "ratio" << std::setw(12) << "linear/bin" << "\n";
    double prev_lin = 0, prev_bin = 0;
    long long checksum = 0;
    for (int n = 2000; n <= 64000; n *= 2) {
        std::vector<int> v(n);
        for (int i = 0; i < n; ++i) v[i] = 2 * i;
        std::mt19937 rng(99);
        std::uniform_int_distribution<int> pick(0, 2 * n);
        std::vector<int> keys(QUERIES);
        for (int& k : keys) k = pick(rng);

        long long hits = 0;
        double lin = time_batch([&](int k) {
            return Search::linear_search(v.data(), n, k); }, keys, 1, hits);
        double bin = time_batch([&](int k) {
            return Search::binary_search(v.data(), n, k); }, keys, 40, hits);

        std::cout << std::setw(10) << n
                  << std::setw(14) << std::fixed << std::setprecision(6) << lin;
        if (prev_lin > 0) std::cout << std::setw(9) << std::setprecision(2) << lin / prev_lin;
        else              std::cout << std::setw(9) << "-";
        std::cout << std::setw(14) << std::setprecision(6) << bin;
        if (prev_bin > 0) std::cout << std::setw(9) << std::setprecision(2) << bin / prev_bin;
        else              std::cout << std::setw(9) << "-";
        std::cout << std::setw(12) << std::setprecision(0) << lin / bin << "\n";
        checksum += hits;
        prev_lin = lin;
        prev_bin = bin;
    }

    std::cout << "  (" << QUERIES << " queries per size, about half of them hits; "
              << checksum << " found in all, printed so that the compiler\n"
              << "   cannot decide the searches are dead code)\n";

    // The counts, which are the same on every machine.
    std::cout << "\nTABLE 2  Comparisons in the worst case\n";
    std::cout << std::setw(14) << "N" << std::setw(12) << "linear"
              << std::setw(12) << "binary" << std::setw(16) << "linear/binary" << "\n";
    for (long long n : {100LL, 1000LL, 1000000LL, 1000000000LL}) {
        long long bin = (long long) std::floor(std::log2((double) n)) + 1;
        std::cout << std::setw(14) << n << std::setw(12) << n
                  << std::setw(12) << bin
                  << std::setw(16) << n / bin << "\n";
    }

    // When does sorting pay for itself? Count comparisons rather than seconds,
    // because the sort's count is the honest thing to weigh against the
    // searches it saves. std::sort does the sorting and a counting comparator
    // does the counting.
    std::cout << "\nTABLE 3  Sorting first, against searching again and again\n";
    std::cout << std::setw(10) << "N" << std::setw(14) << "sort cmps"
              << std::setw(12) << "N lg N" << std::setw(14) << "break-even k"
              << std::setw(10) << "lg N" << "\n";
    for (int n : {1000, 10000, 100000, 1000000}) {
        std::mt19937 rng(5);
        std::vector<int> v(n);
        for (int i = 0; i < n; ++i) v[i] = i;
        std::shuffle(v.begin(), v.end(), rng);
        long long sort_cmps = 0;
        std::sort(v.begin(), v.end(), [&](int a, int b) { ++sort_cmps; return a < b; });

        double lg = std::log2((double) n);
        // k linear searches cost about kN/2 comparisons on a hit. Sorting and
        // then searching costs the sort plus about k lg N. Find the smallest k
        // for which the second is cheaper.
        long long k = 1;
        while (0.5 * k * n < sort_cmps + k * lg && k < 1000000000LL) ++k;
        std::cout << std::setw(10) << n << std::setw(14) << sort_cmps
                  << std::setw(12) << (long long) (n * lg)
                  << std::setw(14) << k
                  << std::setw(10) << std::fixed << std::setprecision(1) << lg << "\n";
    }
    return 0;
}
