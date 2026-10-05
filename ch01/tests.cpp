// Every listing in this chapter compiles and passes these checks.
//   c++ -std=c++17 -Wall -Wextra -O2 -o tests tests.cpp && ./tests
#include "Fibonacci.h"

#include <cstdio>
#include <cstdlib>

static int failures = 0;

static void check(bool ok, const char* what) {
    if (!ok) { std::printf("  FAIL  %s\n", what); ++failures; }
}

int main() {
    // The two implementations must agree wherever both can be run.
    for (longest n = 0; n <= 28; ++n) {
        check(Fibonacci::recursive(n) == Fibonacci::iterative(n),
              "recursive and iterative agree");
    }

    // Known values, including the boundaries the loop is easiest to get wrong.
    check(Fibonacci::iterative(0)  == 0ULL,          "F(0) = 0");
    check(Fibonacci::iterative(1)  == 1ULL,          "F(1) = 1");
    check(Fibonacci::iterative(2)  == 1ULL,          "F(2) = 1");
    check(Fibonacci::iterative(10) == 55ULL,         "F(10) = 55");
    check(Fibonacci::iterative(50) == 12586269025ULL,"F(50) = 12586269025");
    check(Fibonacci::iterative(93) == 12200160415121876738ULL,
          "F(93), the largest that fits in 64 bits");

    // The call count is the point of the chapter, so pin it down.
    check(Fibonacci::recursive_calls(10) == 177ULL, "F(10) costs 177 calls");
    check(Fibonacci::recursive_calls(20) == 21891ULL, "F(20) costs 21891 calls");

    std::printf(failures ? "%d check(s) failed\n" : "all checks pass\n", failures);
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
