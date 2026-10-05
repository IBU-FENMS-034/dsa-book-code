// Every listing in Chapter 4 compiles and passes these checks.
//   c++ -std=c++17 -Wall -Wextra -O2 -o tests tests.cpp && ./tests
#include "Stack.h"
#include "Queue.h"
#include "ArrayStack.h"
#include "postfix.h"

#include <cstdio>
#include <stdexcept>
#include <string>

static int failures = 0;
static void check(bool ok, const char* what) {
    if (!ok) { std::printf("  FAIL  %s\n", what); ++failures; }
}

int main() {
    // ---- the empty stack, which is where the bugs are --------------------
    {
        Stack<int> s;
        check(s.is_empty() && s.size() == 0, "a new stack is empty");
        bool threw = false;
        try { s.pop(); } catch (const std::out_of_range&) { threw = true; }
        check(threw, "pop on an empty stack throws");
        threw = false;
        try { s.peek(); } catch (const std::out_of_range&) { threw = true; }
        check(threw, "peek on an empty stack throws");
    }

    // ---- last in, first out ----------------------------------------------
    {
        Stack<int> s;
        s.push(1); s.push(2); s.push(3);
        check(s.size() == 3, "three pushes give size three");
        check(s.peek() == 3, "peek gives the last pushed");
        check(s.size() == 3, "peek does not remove");
        check(s.pop() == 3 && s.pop() == 2 && s.pop() == 1, "pops come out reversed");
        check(s.is_empty(), "the stack is empty again");
    }

    // ---- the empty queue --------------------------------------------------
    {
        Queue<int> q;
        check(q.is_empty() && q.size() == 0, "a new queue is empty");
        bool threw = false;
        try { q.dequeue(); } catch (const std::out_of_range&) { threw = true; }
        check(threw, "dequeue on an empty queue throws");
    }

    // ---- first in, first out ----------------------------------------------
    {
        Queue<int> q;
        q.enqueue(1); q.enqueue(2); q.enqueue(3);
        check(q.peek() == 1, "peek gives the first enqueued");
        check(q.dequeue() == 1 && q.dequeue() == 2 && q.dequeue() == 3,
              "dequeues come out in order");
        check(q.is_empty(), "the queue is empty again");
        // Emptying and refilling is the transition most likely to go wrong.
        // enqueue tests head, not tail, so a stale tail would be overwritten
        // here; dequeue clears it anyway so that tail is nullptr exactly when
        // the queue is empty.
        q.enqueue(9);
        check(q.size() == 1 && q.peek() == 9, "the queue is reusable after emptying");
    }

    // ---- reverse, on both --------------------------------------------------
    {
        Stack<int> s{1, 2, 3, 4, 5};           // 5 is on top
        s.reverse();
        check(s.size() == 5 && s.pop() == 1 && s.pop() == 2,
              "a reversed stack has its bottom on top");

        Queue<int> q{1, 2, 3, 4, 5};
        q.reverse();
        check(q.peek() == 5, "a reversed queue has its tail at the head");
        q.enqueue(6);   // only works if reverse moved the tail as well
        int out[6], i = 0;
        while (!q.is_empty()) out[i++] = q.dequeue();
        check(i == 6 && out[0] == 5 && out[4] == 1 && out[5] == 6,
              "and enqueue after reverse joins at the new tail");

        Queue<int> empty;
        empty.reverse();
        check(empty.is_empty(), "reversing an empty queue does not crash");
    }

    // ---- the Rule of Five, on both -----------------------------------------
    {
        Stack<int> a{1, 2, 3};                 // 3 is on top
        check(a.peek() == 3, "the initialiser list pushes in order");
        Stack<int> b = a;                      // copy
        check(b.pop() == 3 && b.pop() == 2 && a.peek() == 3,
              "a copied stack is independent and the right way up");
        Stack<int> c = std::move(a);
        check(c.size() == 3 && a.is_empty(), "a moved-from stack is empty");

        Queue<int> p{1, 2, 3};
        Queue<int> r = p;
        check(r.dequeue() == 1 && p.peek() == 1, "a copied queue is independent");
        Queue<int> t = std::move(p);
        check(t.size() == 3 && p.is_empty(), "a moved-from queue is empty");
    }

    // ---- the two stack applications the chapter walks through -------------
    check(balanced("{[()]}"),        "nested brackets balance");
    check(balanced("a(b)[c]{d}"),    "brackets among other characters balance");
    check(!balanced("([)]"),         "crossed brackets do not balance");
    check(!balanced("("),            "an unclosed bracket does not balance");
    check(!balanced(")"),            "a stray closer does not balance");
    check(balanced(""),              "the empty string balances");

    check(evaluate_postfix("3 4 +") == 7,            "3 4 + is 7");
    check(evaluate_postfix("5 1 2 + 4 * + 3 -") == 14, "the worked example gives 14");
    check(evaluate_postfix("2 3 4 * +") == 14,       "multiplication binds first");

    // ---- the array-backed stack, and the capacities it passes through ----
    {
        ArrayStack<int> s(4);
        check(s.capacity() == 4 && s.is_empty(), "it starts at the given capacity");
        for (int i = 0; i < 4; ++i) s.push(i);
        check(s.capacity() == 4, "four pushes fit in four slots");
        s.push(4);
        check(s.capacity() == 8 && s.size() == 5, "the fifth push doubles it");
        s.pop();
        check(s.size() == 4 && s.capacity() == 8, "four of eight has not shrunk");
        s.pop();
        check(s.size() == 3 && s.capacity() == 8, "three of eight has not shrunk");
        s.pop();   // now two of eight, which is exactly a quarter
        check(s.size() == 2 && s.capacity() == 4, "a quarter full halves it");
        s.pop();   // one of four, a quarter again
        check(s.size() == 1 && s.capacity() == 2, "and halves it again");
        check(s.pop() == 0 && s.is_empty(), "the elements survived every resize");
    }

    if (failures == 0) std::printf("Chapter 4 verification suite: all checks pass\n");
    return failures == 0 ? 0 : 1;
}
