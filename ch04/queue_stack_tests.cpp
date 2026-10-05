// Checks for the QueueBasedStack exercise in Chapter 4. Put your own
// QueueBasedStack.tpp beside QueueBasedStack.h, then:
//   c++ -std=c++17 -Wall -Wextra -O2 -o queue_stack_tests queue_stack_tests.cpp && ./queue_stack_tests
#include "QueueBasedStack.h"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>

static int failures = 0;
static void check(bool ok, const char* what) {
    if (!ok) { std::printf("  FAIL  %s\n", what); ++failures; }
}

int main() {
    {
        QueueBasedStack<int> s;
        check(s.is_empty() && s.size() == 0, "a new stack is empty");
        bool threw = false;
        try { s.pop(); } catch (const std::out_of_range&) { threw = true; }
        check(threw, "pop on an empty stack throws");
        threw = false;
        try { (void) s.peek(); } catch (const std::out_of_range&) { threw = true; }
        check(threw, "peek on an empty stack throws");
    }
    {
        QueueBasedStack<int> s;
        s.push(10); s.push(20); s.push(30);
        check(s.size() == 3 && !s.is_empty(), "three pushes give size three");
        check(s.peek() == 30, "peek gives the last pushed");
        check(s.pop() == 30 && s.pop() == 20, "pops come out reversed");
        s.push(40);
        check(s.peek() == 40 && s.size() == 2, "a push after pops goes on top");
        check(s.pop() == 40 && s.pop() == 10, "and the bottom is still there");
        check(s.is_empty(), "the stack is empty again");
    }
    {
        QueueBasedStack<std::string> s;
        s.push("A"); s.push("B"); s.push("C");
        check(s.pop() == "C" && s.pop() == "B" && s.pop() == "A",
              "the same stack holds strings");
    }
    std::printf(failures ? "%d check(s) failed\n" : "all checks pass\n", failures);
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
