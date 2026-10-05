// The same function, two ways. Chapter 1 uses the gap between them to make
// the point that the algorithm matters more than the machine.
#ifndef CH01_FIBONACCI_H
#define CH01_FIBONACCI_H

#include <vector>

// type alias, to avoid copy-pasting unsigned long long int every time
using longest = unsigned long long int;

namespace Fibonacci {
    // Straight from the mathematical definition, and catastrophically slow:
    // it recomputes the same subproblems over and over.
    inline longest recursive(longest n) {
        if (n <= 1) {
            return n;
        }
        return recursive(n - 1) + recursive(n - 2);
    }

    // The same recurrence, computed once each, bottom up.
    inline longest iterative(longest n) {
        std::vector<longest> fib(n + 1);

        fib[0] = 0;
        if (n > 0) {
            fib[1] = 1;
            for (longest i = 2; i <= n; i++) {
                fib[i] = fib[i - 1] + fib[i - 2];
            }
        }
        return fib[n];
    }

    // How many calls recursive() makes. Counted rather than timed, so the
    // number is the same on every machine.
    inline longest recursive_calls(longest n) {
        if (n <= 1) return 1;
        return 1 + recursive_calls(n - 1) + recursive_calls(n - 2);
    }
}

#endif
