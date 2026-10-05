// Reproduces every measured table in Chapter 7, and prints the data the
// comparison figure is drawn from.
//
// Times are from one machine and will differ on yours. The comparison and
// exchange counts are exact and will not.
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <cstdio>
#include "ElementarySort.h"

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
    {"bubble",    [](int* a, int n) { ElementarySort::bubble_sort(a, n); }},
    {"selection", [](int* a, int n) { ElementarySort::selection_sort(a, n); }},
    {"insertion", [](int* a, int n) { ElementarySort::insertion_sort(a, n); }},
    {"Shell",     [](int* a, int n) { ElementarySort::shell_sort(a, n); }},
};

// A single sort of a small array finishes inside the clock's own noise, so the
// small cases are run several times and the fastest run is reported. The
// minimum is the right summary of a set of timings: everything that varies
// between runs adds time, so the fastest run is the one with least
// interference in it. Copying the array is outside the timed region.
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

// Instrumented copies, so the counts are the ones the printed code performs.
struct Count { long long compares = 0, swaps = 0; };

static void bubble_counted(std::vector<int>& a, Count& c) {
    int len = (int) a.size();
    for (int i = 0; i < len; ++i) {
        bool swapped = false;
        for (int j = 1; j < len - i; ++j) {
            ++c.compares;
            if (a[j] < a[j - 1]) { std::swap(a[j], a[j - 1]); ++c.swaps; swapped = true; }
        }
        if (!swapped) break;
    }
}
static void selection_counted(std::vector<int>& a, Count& c) {
    int len = (int) a.size();
    for (int i = 0; i < len; ++i) {
        int min = i;
        for (int j = i + 1; j < len; ++j) { ++c.compares; if (a[j] < a[min]) min = j; }
        std::swap(a[min], a[i]);
        ++c.swaps;
    }
}
static void insertion_counted(std::vector<int>& a, Count& c) {
    int len = (int) a.size();
    for (int i = 1; i < len; ++i)
        for (int j = i; j > 0; --j) {
            ++c.compares;
            if (a[j] < a[j - 1]) { std::swap(a[j], a[j - 1]); ++c.swaps; }
            else break;
        }
}
static void shell_counted(std::vector<int>& a, Count& c) {
    int len = (int) a.size(), h = 1;
    while (h < len / 3) h = 3 * h + 1;
    for (; h >= 1; h /= 3)
        for (int i = h; i < len; ++i)
            for (int j = i; j >= h; j -= h) {
                ++c.compares;
                if (a[j] < a[j - h]) { std::swap(a[j], a[j - h]); ++c.swaps; }
                else break;
            }
}

int main() {
    long long checksum = 0;

    std::cout << "TABLE 1  Seconds to sort a shuffled array\n";
    std::cout << std::setw(9) << "N";
    for (const Alg& a : ALGS) std::cout << std::setw(13) << a.name;
    std::cout << "\n";
    std::vector<std::vector<double>> times(4);
    std::vector<int> sizes;
    for (int n = 2000; n <= 64000; n *= 2) {
        sizes.push_back(n);
        std::vector<int> v = shuffled(n, 4242u);
        std::cout << std::setw(9) << n;
        for (int i = 0; i < 4; ++i) {
            double t = time_sort(ALGS[i], v, checksum);
            times[i].push_back(t);
            std::cout << std::setw(13) << std::fixed << std::setprecision(5) << t;
        }
        std::cout << "\n";
    }

    std::cout << "\nTABLE 2  The same times as doubling ratios\n";
    std::cout << std::setw(9) << "N";
    for (const Alg& a : ALGS) std::cout << std::setw(13) << a.name;
    std::cout << "\n";
    for (size_t r = 1; r < sizes.size(); ++r) {
        std::cout << std::setw(9) << sizes[r];
        for (int i = 0; i < 4; ++i)
            std::cout << std::setw(13) << std::setprecision(2)
                      << times[i][r] / times[i][r - 1];
        std::cout << "\n";
    }

    // Beyond about a hundred thousand elements the quadratic sorts stop being
    // things you can wait for, which is the point of the table. Anything whose
    // projected time is over the budget is projected rather than run, by
    // squaring the ratio of the sizes: the doubling ratios above are what
    // justify assuming the square.
    const double BUDGET = 20.0;          // seconds we are willing to spend
    std::cout << "\nTABLE 2b  Larger inputs. A time in brackets was projected,\n"
              << "          not measured, by assuming quadratic growth.\n";
    std::cout << std::setw(11) << "N";
    for (const Alg& a : ALGS) std::cout << std::setw(16) << a.name;
    std::cout << "\n";
    std::vector<double> anchor_t(4), anchor_n(4);
    for (int i = 0; i < 4; ++i) { anchor_t[i] = times[i].back(); anchor_n[i] = sizes.back(); }
    for (long long n : {100000LL, 1000000LL, 10000000LL}) {
        std::cout << std::setw(11) << n;
        for (int i = 0; i < 4; ++i) {
            double ratio = (double) n / anchor_n[i];
            double projected = anchor_t[i] * ratio * ratio;
            if (projected <= BUDGET) {
                std::vector<int> v = shuffled((int) n, 4242u);
                double t = time_sort(ALGS[i], v, checksum);
                std::cout << std::setw(16) << std::fixed << std::setprecision(5) << t;
                anchor_t[i] = t;
                anchor_n[i] = (double) n;
            } else {
                char buf[64];
                if (projected < 1000)      snprintf(buf, sizeof buf, "(%.0f s)", projected);
                else if (projected < 86400) snprintf(buf, sizeof buf, "(%.1f h)", projected / 3600);
                else                        snprintf(buf, sizeof buf, "(%.0f days)", projected / 86400);
                std::cout << std::setw(16) << buf;
            }
        }
        std::cout << "\n";
    }

    std::cout << "\nTABLE 3  Comparisons and exchanges on a shuffled array\n";
    std::cout << std::setw(8) << "N" << std::setw(16) << "algorithm"
              << std::setw(16) << "compares" << std::setw(16) << "exchanges" << "\n";
    for (int n : {1000, 4000, 16000}) {
        std::vector<int> base = shuffled(n, 4242u);
        const char* names[] = {"bubble", "selection", "insertion", "Shell"};
        for (int i = 0; i < 4; ++i) {
            std::vector<int> a = base;
            Count c;
            if (i == 0) bubble_counted(a, c);
            if (i == 1) selection_counted(a, c);
            if (i == 2) insertion_counted(a, c);
            if (i == 3) shell_counted(a, c);
            std::cout << std::setw(8) << n << std::setw(16) << names[i]
                      << std::setw(16) << c.compares
                      << std::setw(16) << c.swaps << "\n";
        }
    }

    std::cout << "\nTABLE 4  Seconds on an array that is already sorted\n";
    std::cout << std::setw(9) << "N";
    for (const Alg& a : ALGS) std::cout << std::setw(13) << a.name;
    std::cout << "\n";
    for (int n : {16000, 64000}) {
        std::vector<int> v(n);
        for (int i = 0; i < n; ++i) v[i] = i;
        std::cout << std::setw(9) << n;
        for (const Alg& alg : ALGS)
            std::cout << std::setw(13) << std::fixed << std::setprecision(5)
                      << time_sort(alg, v, checksum);
        std::cout << "\n";
    }

    std::cout << "\n(checksum " << checksum
              << ", printed so the compiler cannot discard the sorting)\n";
    return 0;
}
