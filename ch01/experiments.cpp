// Produces the table in Chapter 1. Run it yourself:
//   c++ -std=c++17 -O2 -o experiments experiments.cpp && ./experiments
#include "Fibonacci.h"

#include <chrono>
#include <cstdio>

template <typename F>
static double seconds(F f) {
    const auto t0 = std::chrono::steady_clock::now();
    f();
    const auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(t1 - t0).count();
}

int main() {
    std::printf("%4s %18s %14s %14s\n", "N", "calls (recursive)", "recursive", "iterative");
    for (const longest n : {10, 20, 30, 35, 40, 45}) {
        const double rec = seconds([&] { Fibonacci::recursive(n); });
        // One iterative call is far too fast to time, so time a million.
        // sink is volatile so the optimiser cannot delete the loop it feeds.
        static volatile longest sink = 0;
        const double it  = seconds([&] {
            for (int i = 0; i < 1000000; ++i) sink = sink + Fibonacci::iterative(n);
        }) / 1000000.0;
        std::printf("%4llu %18llu %11.3f s %11.6f ms\n", n,
                    Fibonacci::recursive_calls(n),
                    rec, it * 1000.0);
    }
    return 0;
}
