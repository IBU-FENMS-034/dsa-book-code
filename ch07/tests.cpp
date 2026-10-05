// Chapter 7 verification suite. Every count, every property and every worked
// example the chapter prints is checked here against the code that produces it.
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include "ElementarySort.h"

static int failures = 0;

static void check(bool ok, const std::string& what) {
    if (!ok) { std::cout << "  FAIL: " << what << "\n"; ++failures; }
}

// A record whose sort key is one field, so that ties can be told apart. The
// tag records where the element started, which is what stability is about.
struct Tagged {
    int key;
    int tag;
    bool operator<(const Tagged& other) const { return key < other.key; }
};

static bool is_stable(const std::vector<Tagged>& sorted) {
    for (size_t i = 1; i < sorted.size(); ++i) {
        if (sorted[i - 1].key == sorted[i].key && sorted[i - 1].tag > sorted[i].tag)
            return false;
    }
    return true;
}

// Instrumented copies of the four sorts. Each mirrors ElementarySort.tpp with
// two counters added, on the comparison and on the exchange.
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
    for (int i = 1; i < len; ++i) {
        for (int j = i; j > 0; --j) {
            ++c.compares;
            if (a[j] < a[j - 1]) { std::swap(a[j], a[j - 1]); ++c.swaps; }
            else break;
        }
    }
}

// The number of pairs that are in the wrong order, counted the slow and
// obvious way. Insertion sort should perform exactly this many exchanges.
static long long inversions(const std::vector<int>& a) {
    long long n = 0;
    for (size_t i = 0; i < a.size(); ++i)
        for (size_t j = i + 1; j < a.size(); ++j)
            if (a[j] < a[i]) ++n;
    return n;
}

int main() {
    std::cout << "Chapter 7 verification suite\n\n";

    // ----------------------------------------------------- they all sort ----
    // Against std::sort, on random, sorted, reversed and heavily repeated
    // input, at every length up to 60 and at a few larger ones.
    std::mt19937 rng(11);
    std::vector<int> lengths;
    for (int n = 0; n <= 60; ++n) lengths.push_back(n);
    for (int n : {100, 257, 1000}) lengths.push_back(n);
    for (int len : lengths) {
        for (int kind = 0; kind < 4; ++kind) {
            std::vector<int> base(len);
            for (int i = 0; i < len; ++i) {
                if (kind == 0) base[i] = (int) (rng() % 1000);
                else if (kind == 1) base[i] = i;
                else if (kind == 2) base[i] = len - i;
                else base[i] = (int) (rng() % 3);
            }
            std::vector<int> want = base;
            std::sort(want.begin(), want.end());
            for (int alg = 0; alg < 4; ++alg) {
                std::vector<int> v = base;
                if (alg == 0) ElementarySort::bubble_sort(v.data(), len);
                if (alg == 1) ElementarySort::selection_sort(v.data(), len);
                if (alg == 2) ElementarySort::insertion_sort(v.data(), len);
                if (alg == 3) ElementarySort::shell_sort(v.data(), len);
                check(v == want, "sort " + std::to_string(alg) + " at length "
                                 + std::to_string(len) + " kind " + std::to_string(kind));
            }
        }
    }

    // -------------------------------------------------------- stability ----
    // Four hundred records sharing twenty keys. Bubble and insertion sort must
    // keep ties in their original order; selection and Shell sort must not be
    // relied on to, and on this input they do not.
    std::vector<Tagged> records(400);
    for (int i = 0; i < 400; ++i) records[i] = {(int) (rng() % 20), i};
    std::vector<Tagged> v;
    v = records; ElementarySort::bubble_sort(v.data(), 400);
    check(is_stable(v), "bubble sort is stable");
    v = records; ElementarySort::insertion_sort(v.data(), 400);
    check(is_stable(v), "insertion sort is stable");
    v = records; ElementarySort::selection_sort(v.data(), 400);
    check(!is_stable(v), "selection sort is not stable on this input");
    v = records; ElementarySort::shell_sort(v.data(), 400);
    check(!is_stable(v), "Shell sort is not stable on this input");

    // The chapter's small worked examples, which a reader can follow by hand.
    std::vector<Tagged> four = {{3, 0}, {8, 1}, {3, 2}, {1, 3}};
    v = four; ElementarySort::selection_sort(v.data(), 4);
    check(v[1].tag == 2 && v[2].tag == 0,
          "selection sort reverses the two 3s of [3, 8, 3, 1]");
    std::cout << "  [3, 8, 3, 1] under selection sort leaves the 3s as "
              << v[1].tag << " then " << v[2].tag << ", swapped from 0 then 2\n";
    // The smallest input on which shell_sort's own increments reorder equal
    // keys: the 4-sort carries the fifth element past three equal ones it
    // never compares it with.
    std::vector<Tagged> shellcase = {{1, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}};
    v = shellcase; ElementarySort::shell_sort(v.data(), 6);
    check(v[0].tag == 4, "Shell sort moves the 0 from position 4 to the front");
    std::cout << "  [1, 0, 0, 0, 0, 0] under Shell sort leaves the zeros in the order ";
    for (int i = 0; i < 5; ++i) std::cout << v[i].tag << (i < 4 ? ", " : "\n");

    // ------------------------------------------------------- the counts ----
    // Selection sort makes exactly N(N-1)/2 comparisons and N exchanges,
    // whatever the input.
    for (int len : {8, 16, 64, 200}) {
        for (int kind = 0; kind < 3; ++kind) {
            std::vector<int> a(len);
            for (int i = 0; i < len; ++i)
                a[i] = kind == 0 ? (int) (rng() % 1000) : (kind == 1 ? i : len - i);
            Count c;
            selection_counted(a, c);
            check(c.compares == (long long) len * (len - 1) / 2,
                  "selection sort comparisons at " + std::to_string(len));
            check(c.swaps == len, "selection sort exchanges at " + std::to_string(len));
        }
    }
    std::vector<int> two_hundred(200);
    for (int i = 0; i < 200; ++i) two_hundred[i] = 200 - i;
    Count sel;
    selection_counted(two_hundred, sel);
    std::cout << "  selection sort on 200 elements: " << sel.compares
              << " comparisons and " << sel.swaps
              << " exchanges, on every input\n";

    // Insertion sort performs exactly as many exchanges as the input has
    // inversions, and its comparisons sit between that and that plus N - 1.
    for (int trial = 0; trial < 60; ++trial) {
        int len = 5 + (int) (rng() % 40);
        std::vector<int> a(len);
        for (int& x : a) x = (int) (rng() % 50);
        long long inv = inversions(a);
        std::vector<int> copy = a;
        Count c;
        insertion_counted(a, c);
        check(c.swaps == inv, "insertion sort exchanges equal the inversions");
        // Bubble sort moves elements one place at a time as well, so it makes
        // exactly the same exchanges, and only the comparisons differ.
        Count bc;
        bubble_counted(copy, bc);
        check(bc.swaps == inv, "bubble sort exchanges equal the inversions too");
        check(c.compares >= inv && c.compares <= inv + len - 1,
              "insertion sort comparisons are inversions plus at most N - 1");
    }

    // The two extremes for bubble and insertion sort.
    for (int len : {50, 500}) {
        std::vector<int> sorted(len), reversed(len);
        for (int i = 0; i < len; ++i) { sorted[i] = i; reversed[i] = len - i; }
        Count a, b, c, d;
        std::vector<int> t;
        t = sorted;   bubble_counted(t, a);
        t = reversed; bubble_counted(t, b);
        t = sorted;   insertion_counted(t, c);
        t = reversed; insertion_counted(t, d);
        check(a.compares == len - 1 && a.swaps == 0,
              "bubble sort on sorted input is one clean pass");
        check(b.swaps == (long long) len * (len - 1) / 2,
              "bubble sort on reversed input swaps every pair");
        check(c.compares == len - 1 && c.swaps == 0,
              "insertion sort on sorted input makes N - 1 comparisons");
        check(d.swaps == (long long) len * (len - 1) / 2,
              "insertion sort on reversed input swaps every pair");
        if (len == 500)
            std::cout << "  500 elements, already sorted: bubble " << a.compares
                      << " comparisons, insertion " << c.compares
                      << "; reversed: " << b.swaps << " exchanges each\n";
    }

    // ------------------------------------------------- the h-sequence ------
    // The increments the chapter names, and the fact that an h-sorted array is
    // still h-sorted after the next, smaller pass has run.
    // Built exactly the way shell_sort builds it, so the chapter can quote it.
    const int BIG = 100000;
    int start = 1;
    while (start < BIG / 3) start = 3 * start + 1;
    std::vector<int> hs;
    for (int h = start; h >= 1; h /= 3) hs.push_back(h);
    check((hs == std::vector<int>{88573, 29524, 9841, 3280, 1093, 364, 121,
                                  40, 13, 4, 1}),
          "the increments shell_sort uses on 100,000 elements");
    std::cout << "  " << hs.size() << " passes over 100,000 elements, with h = ";
    for (size_t i = 0; i < hs.size(); ++i)
        std::cout << hs[i] << (i + 1 == hs.size() ? "\n" : ", ");

    std::cout << "\n";
    if (failures == 0) std::cout << "Chapter 7 verification suite: all checks pass\n";
    else std::cout << failures << " check(s) failed\n";
    return failures == 0 ? 0 : 1;
}
