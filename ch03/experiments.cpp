// Produces the tables in Chapter 3. Run it yourself:
//   c++ -std=c++17 -O2 -o experiments experiments.cpp && ./experiments
//
// The point is not the absolute times, which depend on the machine. It is the
// shape: which column grows with N and which does not.
#include "LinkedList.h"

#include <chrono>
#include <cstdio>
#include <algorithm>
#include <numeric>
#include <vector>

// The fastest of as many runs as fit in half a second, with at least three.
// A single run of the small sizes finishes inside the clock's own noise, and
// the ratios that come out of one run are not usable.
template <typename F>
static double ms(F f) {
    double best = 1e30, spent = 0;
    for (int run = 0; run < 2000 && (run < 3 || spent < 500.0); ++run) {
        const auto t0 = std::chrono::steady_clock::now();
        f();
        const auto t1 = std::chrono::steady_clock::now();
        const double t =
            std::chrono::duration<double, std::milli>(t1 - t0).count();
        if (t < best) best = t;
        spent += t;
    }
    return best;
}

static volatile long long sink = 0;

int main() {
    std::printf("TABLE 1  Inserting N items at the FRONT\n");
    std::printf("%9s %14s %14s\n", "N", "vector (ms)", "list (ms)");
    for (const int n : {1000, 2000, 4000, 8000, 16000}) {
        const double vec = ms([&] {
            std::vector<int> v;
            for (int i = 0; i < n; ++i) v.insert(v.begin(), i);   // shifts everything
        });
        const double lst = ms([&] {
            LinkedList<int> l;
            for (int i = 0; i < n; ++i) l.add_to_front(i);        // relinks one node
        });
        std::printf("%9d %14.2f %14.2f\n", n, vec, lst);
    }

    std::printf("\nTABLE 2  Reading every element by index\n");
    std::printf("%9s %14s %14s\n", "N", "vector (ms)", "list (ms)");
    for (const int n : {1000, 2000, 4000, 8000, 16000}) {
        std::vector<int> v(n);
        std::iota(v.begin(), v.end(), 0);

        const double vec = ms([&] {
            long long t = 0;
            for (int i = 0; i < n; ++i) t += v[i];
            sink = t;
        });

        // A list's walk depends on where the allocator happened to put the
        // nodes, and that is fixed once the list is built, so timing one list
        // many times measures one layout. Build it afresh each time and report
        // the median, which is the figure that survives a second run.
        std::vector<double> walks;
        for (int rebuild = 0; rebuild < 9; ++rebuild) {
            LinkedList<int> l;
            for (int i = n - 1; i >= 0; --i) l.add_to_front(i);
            walks.push_back(ms([&] {
                long long t = 0;
                for (int i = 0; i < n; ++i) t += l.get(i);        // walks from the head
                sink = t;
            }));
        }
        std::sort(walks.begin(), walks.end());
        std::printf("%9d %14.3f %14.3f\n", n, vec, walks[walks.size() / 2]);
    }

    std::printf("\nTABLE 3  What one int costs, in bytes\n");
    std::printf("  vector<int>        %2zu  (the int, and nothing else)\n", sizeof(int));
    std::printf("  LinkedList<int>    %2zu  (an int, a pointer, and the padding between)\n",
                sizeof(Node<int>));
    return 0;
}
