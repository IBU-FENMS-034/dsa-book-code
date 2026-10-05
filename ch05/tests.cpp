// Chapter 5 verification suite. Every count the chapter prints is checked
// here against the code that produces it, so a formula in the text and the
// behaviour of a loop cannot disagree.
#include <iostream>
#include <cmath>
#include <string>
#include "Analysis.h"

static int failures = 0;

static void check(bool ok, const std::string& what) {
    if (!ok) { std::cout << "  FAIL: " << what << "\n"; ++failures; }
}

// Instrumented copies of the two loops. They mirror Analysis.h exactly, with a
// counter added on the line the chapter counts.
static long long pair_steps(int len) {
    long long steps = 0;
    for (int i = 0; i < len; ++i)
        for (int j = i + 1; j < len; ++j) ++steps;
    return steps;
}

static long long triple_steps(int len) {
    long long steps = 0;
    for (int i = 0; i < len; ++i)
        for (int j = i + 1; j < len; ++j)
            for (int k = j + 1; k < len; ++k) ++steps;
    return steps;
}

// A resizing array that only records what it would cost, so the amortised
// argument can be checked without allocating anything.
static long long push_cost(int pushes, int start_cap) {
    long long work = 0;
    int cap = start_cap, count = 0;
    for (int i = 0; i < pushes; ++i) {
        if (count == cap) { work += count; cap *= 2; }
        work += 1;
        ++count;
    }
    return work;
}

int main() {
    std::cout << "Chapter 5 verification suite\n\n";

    // ---------------------------------------------------- the loop counts --
    // The double loop runs N(N-1)/2 times and the triple loop N(N-1)(N-2)/6.
    for (int n = 0; n <= 60; ++n) {
        check(pair_steps(n) == (long long) n * (n - 1) / 2,
              "double loop count at N = " + std::to_string(n));
        check(triple_steps(n) == (long long) n * (n - 1) * (n - 2) / 6,
              "triple loop count at N = " + std::to_string(n));
    }
    std::cout << "  double loop: N = 8 runs " << pair_steps(8)
              << " times, N = 1000 runs " << pair_steps(1000) << "\n";
    std::cout << "  triple loop: N = 8 runs " << triple_steps(8)
              << " times, N = 1000 runs " << triple_steps(1000) << "\n";

    // Doubling N multiplies the double loop's count by very nearly four.
    double ratio = (double) pair_steps(2000) / (double) pair_steps(1000);
    check(std::fabs(ratio - 4.0) < 0.01, "pair count roughly quadruples");
    std::cout << "  doubling N from 1000 to 2000 multiplies the count by "
              << ratio << "\n\n";

    // -------------------------------------------------------- the answers --
    // The counting itself has to be right, not only the number of steps.
    const int a[] = {-3, 1, 2, 3, -1, 0, 4};
    check(Analysis::count_pairs(a, 7) == 2, "count_pairs finds {-3,3} and {1,-1}");
    check(Analysis::count_triples(a, 7) == 4, "count_triples on the same array");
    check(Analysis::total(a, 7) == 6, "total of the same array");
    std::cout << "  count_pairs   = " << Analysis::count_pairs(a, 7) << "\n";
    std::cout << "  count_triples = " << Analysis::count_triples(a, 7) << "\n\n";

    // ------------------------------------------------------- the halvings --
    // halvings(n) is floor(lg n), which is why halving loops are logarithmic.
    for (int n = 1; n <= 5000; ++n)
        check(Analysis::halvings(n) == (int) std::floor(std::log2((double) n)),
              "halvings(" + std::to_string(n) + ") is floor(lg n)");
    std::cout << "  halvings(1000) = " << Analysis::halvings(1000)
              << ", halvings(1000000) = " << Analysis::halvings(1000000) << "\n\n";

    // ------------------------------------------------------ the amortising --
    // N pushes into a doubling array cost less than 3N in total, whatever the
    // starting capacity, so the cost per push is bounded by a constant.
    for (int start : {1, 2, 4, 16}) {
        for (int n = 1; n <= 5000; ++n) {
            long long work = push_cost(n, start);
            check(work < 3LL * n + start,
                  "N pushes cost under 3N, start_cap " + std::to_string(start));
        }
    }
    std::cout << "  1000 pushes from capacity 1 cost " << push_cost(1000, 1)
              << " units of work, " << (double) push_cost(1000, 1) / 1000
              << " per push\n";
    std::cout << "  1000 pushes from capacity 4 cost " << push_cost(1000, 4)
              << " units of work, " << (double) push_cost(1000, 4) / 1000
              << " per push\n\n";

    if (failures == 0) std::cout << "Chapter 5 verification suite: all checks pass\n";
    else std::cout << failures << " check(s) failed\n";
    return failures == 0 ? 0 : 1;
}
