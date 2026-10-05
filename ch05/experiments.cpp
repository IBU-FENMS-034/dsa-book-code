// Reproduces every measured table in Chapter 5, and prints the data the
// doubling-test figure is drawn from.
//
// The measurement is the doubling test: run the same program on N, then on
// 2N, and divide. If the running time grows like a power of N, the ratio
// settles on 2 raised to that power, so the exponent is the base-2 logarithm
// of the ratio. Times are from one machine and will differ on yours; the
// ratios are the part that should not.
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include "Analysis.h"

using Clock = std::chrono::steady_clock;

static std::vector<int> random_array(int len, unsigned seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> pick(-len * 5, len * 5);
    std::vector<int> v(len);
    for (int& x : v) x = pick(rng);
    return v;
}

// Runs one function on an array of the given length and returns the seconds
// it took. The result is printed by the caller so the optimiser cannot decide
// the whole computation is dead code.
template <typename Fn>
static double time_it(Fn fn, const std::vector<int>& v, int repeats,
                      long long& answer) {
    auto start = Clock::now();
    for (int r = 0; r < repeats; ++r) answer = fn(v.data(), (int) v.size());
    auto stop = Clock::now();
    return std::chrono::duration<double>(stop - start).count();
}

// A single pass over even a large array finishes faster than the clock can
// resolve, so the cheap functions are run several times and the whole batch is
// timed. Repeating does not touch the ratio, which is what the table is for.
template <typename Fn>
static void doubling_test(const char* title, Fn fn, int from, int steps,
                          int repeats = 1) {
    std::cout << title << "\n";
    std::cout << std::setw(10) << "N" << std::setw(14) << "seconds"
              << std::setw(10) << "ratio" << std::setw(12) << "lg ratio"
              << std::setw(14) << "answer" << "\n";
    double previous = 0;
    for (int i = 0, n = from; i < steps; ++i, n *= 2) {
        std::vector<int> v = random_array(n, 1234u + i);
        long long answer = 0;
        double seconds = time_it(fn, v, repeats, answer);
        std::cout << std::setw(10) << n
                  << std::setw(14) << std::fixed << std::setprecision(4) << seconds;
        if (previous > 0)
            std::cout << std::setw(10) << std::setprecision(2) << seconds / previous
                      << std::setw(12) << std::setprecision(2)
                      << std::log2(seconds / previous);
        else
            std::cout << std::setw(10) << "-" << std::setw(12) << "-";
        std::cout << std::setw(14) << answer << "\n";
        previous = seconds;
    }
    std::cout << "\n";
}

int main() {
    std::cout << "TABLE 1  Doubling test, one pass over the array\n";
    doubling_test("(Analysis::total, expected ratio 2)",
                  [](const int* a, int n) { return Analysis::total(a, n); },
                  10000, 6, 50000);

    std::cout << "TABLE 2  Doubling test, the double loop\n";
    doubling_test("(Analysis::count_pairs, expected ratio 4)",
                  [](const int* a, int n) {
                      return (long long) Analysis::count_pairs(a, n);
                  },
                  2000, 6);

    std::cout << "TABLE 3  Doubling test, the triple loop\n";
    doubling_test("(Analysis::count_triples, expected ratio 8)",
                  [](const int* a, int n) {
                      return (long long) Analysis::count_triples(a, n);
                  },
                  250, 6);

    // The exact counts, which are the same on every machine.
    std::cout << "TABLE 4  Steps taken, against the formulas\n";
    std::cout << std::setw(8) << "N" << std::setw(16) << "double loop"
              << std::setw(16) << "N(N-1)/2" << std::setw(18) << "triple loop"
              << std::setw(18) << "N(N-1)(N-2)/6" << "\n";
    for (int n : {8, 16, 32, 64, 128, 256}) {
        long long d = 0, t = 0;
        for (int i = 0; i < n; ++i)
            for (int j = i + 1; j < n; ++j) {
                ++d;
                for (int k = j + 1; k < n; ++k) ++t;
            }
        std::cout << std::setw(8) << n << std::setw(16) << d
                  << std::setw(16) << (long long) n * (n - 1) / 2
                  << std::setw(18) << t
                  << std::setw(18) << (long long) n * (n - 1) * (n - 2) / 6 << "\n";
    }

    // The sizes each class can finish inside a fixed budget of steps. The
    // budget is an assumption, not a measurement: a billion basic steps a
    // second is the right order for one core of a current laptop, and the
    // table in the chapter says so.
    std::cout << "\nTABLE 5  Largest N finishing inside a step budget\n";
    std::cout << std::setw(16) << "order of growth" << std::setw(20) << "one second"
              << std::setw(20) << "one hour" << "\n";
    const double budgets[2] = {1e9, 3.6e12};
    struct Class { const char* name; double (*steps)(double); };
    const Class classes[] = {
        {"N",       [](double n) { return n; }},
        {"N lg N",  [](double n) { return n * std::log2(n); }},
        {"N^2",     [](double n) { return n * n; }},
        {"N^3",     [](double n) { return n * n * n; }},
        {"2^N",     [](double n) { return std::pow(2.0, n); }},
        {"N!",      [](double n) { double f = 1; for (int i = 2; i <= (int) n; ++i) f *= i; return f; }},
    };
    for (const Class& cl : classes) {
        std::cout << std::setw(16) << cl.name;
        for (double budget : budgets) {
            // Largest whole N whose cost still fits. Walk upwards by doubling
            // and then bisect, so no class needs its own inverse function.
            double lo = 1, hi = 2;
            while (cl.steps(hi) <= budget && hi < 1e18) hi *= 2;
            for (int it = 0; it < 200; ++it) {
                double mid = (lo + hi) / 2;
                if (cl.steps(std::floor(mid)) <= budget) lo = mid; else hi = mid;
            }
            std::cout << std::setw(20) << std::setprecision(0) << std::floor(lo);
        }
        std::cout << "\n";
    }
    return 0;
}
