// Chapter 10 verification suite. Both sorts are checked against std::sort on
// every shape of input the chapter mentions, the digit pass is checked for the
// stability radix sort depends on, and the claim that dropping that stability
// breaks radix sort is checked by dropping it.
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include "NonComparisonSort.h"

static int failures = 0;

static void check(bool ok, const std::string& what) {
    if (!ok) { std::cout << "  FAIL: " << what << "\n"; ++failures; }
}

// Four shapes of input, so a bug that only shows on one of them cannot hide.
static std::vector<int> make(int len, int kind, int range, std::mt19937& rng) {
    std::vector<int> v(len);
    for (int i = 0; i < len; i++) {
        if (kind == 0)      v[i] = (int) (rng() % (unsigned) range);
        else if (kind == 1) v[i] = (i * range) / (len ? len : 1);
        else if (kind == 2) v[i] = ((len - i) * range) / (len ? len : 1);
        else                v[i] = (int) (rng() % 3);
    }
    return v;
}

// The digit pass with the final loop run forwards instead of backwards, which
// reverses each bucket and so is not stable. Section 10.4.2 claims radix sort
// breaks without that stability; this is how we know.
static void unstable_pass(std::vector<int>& a, int exp) {
    int len = (int) a.size();
    std::vector<int> aux(len);
    int frequency[10] = {0};
    for (int i = 0; i < len; i++) ++frequency[(a[i] / exp) % 10];
    for (int i = 1; i < 10; i++) frequency[i] += frequency[i - 1];
    for (int i = 0; i < len; i++) aux[--frequency[(a[i] / exp) % 10]] = a[i];
    a = aux;
}
static void unstable_radix(std::vector<int>& a) {
    if (a.size() < 2) return;
    int max = *std::max_element(a.begin(), a.end());
    for (int exp = 1; max / exp > 0; exp *= 10) unstable_pass(a, exp);
}

int main() {
    std::cout << "Chapter 10 verification suite\n\n";
    std::mt19937 rng(808);

    // ------------------------------------------------- both sorts sort -----
    for (int len = 0; len <= 200; len++) {
        for (int kind = 0; kind < 4; kind++) {
            for (int range : {10, 1000, 100000}) {
                std::vector<int> base = make(len, kind, range, rng);
                std::vector<int> want = base;
                std::sort(want.begin(), want.end());

                std::vector<int> v = base;
                NonComparisonSort::counting_sort(v.data(), len);
                check(v == want, "counting_sort at length "
                                 + std::to_string(len));
                v = base;
                NonComparisonSort::radix_sort(v.data(), len);
                check(v == want, "radix_sort at length "
                                 + std::to_string(len));
            }
        }
    }
    for (int n : {1000, 50000, 200001}) {
        std::vector<int> base(n);
        for (int& x : base) x = (int) (rng() % 1000000);
        std::vector<int> want = base;
        std::sort(want.begin(), want.end());
        std::vector<int> v = base;
        NonComparisonSort::radix_sort(v.data(), n);
        check(v == want, "radix_sort at " + std::to_string(n));
        v = base;
        NonComparisonSort::counting_sort(v.data(), n);
        check(v == want, "counting_sort at " + std::to_string(n));
    }

    // ------------------------------------------------- the awkward cases ---
    {
        std::vector<int> zeros(50, 0);
        std::vector<int> v = zeros;
        NonComparisonSort::radix_sort(v.data(), 50);
        check(v == zeros, "radix_sort on fifty zeros");
        v = zeros;
        NonComparisonSort::counting_sort(v.data(), 50);
        check(v == zeros, "counting_sort on fifty zeros");

        std::vector<int> one = {7};
        NonComparisonSort::radix_sort(one.data(), 1);
        check(one == std::vector<int>{7}, "radix_sort on one element");
        NonComparisonSort::counting_sort(one.data(), 0);
        check(one == std::vector<int>{7}, "counting_sort on nothing");

        // Values either side of a power of ten, where the pass count changes.
        std::vector<int> edge = {1000, 999, 1001, 100, 99, 0, 10000};
        std::vector<int> want = edge;
        std::sort(want.begin(), want.end());
        std::vector<int> e = edge;
        NonComparisonSort::radix_sort(e.data(), (int) e.size());
        check(e == want, "radix_sort across powers of ten");
    }

    // ------------------------------------------- the digit pass is stable --
    // Elements sharing a digit must come out in the order they went in.
    {
        std::vector<int> a = {21, 31, 41, 13, 23, 11};
        std::vector<int> want = {21, 31, 41, 11};   // the ones, in order
        NonComparisonSort::detail::sort(a.data(), (int) a.size(), 1);
        std::vector<int> ones;
        for (int x : a) if (x % 10 == 1) ones.push_back(x);
        check(ones == want, "the units pass keeps equal digits in order");
        std::vector<int> threes;
        for (int x : a) if (x % 10 == 3) threes.push_back(x);
        check((threes == std::vector<int>{13, 23}),
              "and so does the second bucket");
    }

    // ------------------------------- and radix sort depends on that ------
    // The same algorithm with an unstable pass gets the wrong answer.
    {
        int wrong = 0;
        for (int t = 0; t < 200; t++) {
            std::vector<int> base(40);
            for (int& x : base) x = (int) (rng() % 1000);
            std::vector<int> want = base;
            std::sort(want.begin(), want.end());
            std::vector<int> v = base;
            unstable_radix(v);
            if (v != want) wrong++;
        }
        check(wrong > 150, "an unstable pass breaks radix sort");
        std::cout << "  radix sort with an unstable digit pass: wrong on "
                  << wrong << " of 200 random arrays\n";
    }

    // ---------------------------------------------- the number of passes --
    // One pass per digit of the largest element, and nothing else.
    for (int digits = 1; digits <= 7; digits++) {
        int max = (int) std::pow(10, digits) - 1;
        int passes = 0;
        for (int exp = 1; max / exp > 0; exp *= 10) passes++;
        check(passes == digits,
              "a " + std::to_string(digits) + "-digit maximum takes "
              + std::to_string(digits) + " passes");
    }

    // ------------------------------------------- what the range costs ------
    // Counting sort's memory follows the largest value, not the length.
    {
        std::vector<int> small = {1, 0, 1, 0, 1};
        std::vector<int> wide  = {1000000, 0, 1000000, 0, 1};
        std::vector<int> a = small, b = wide;
        NonComparisonSort::counting_sort(a.data(), 5);
        NonComparisonSort::counting_sort(b.data(), 5);
        check((a == std::vector<int>{0, 0, 1, 1, 1}), "five small values");
        check((b == std::vector<int>{0, 0, 1, 1000000, 1000000}),
              "five values spanning a million");
        std::cout << "  five elements, largest 1,000,000: the count array "
                  << "holds 1,000,001 ints (" << (1000001 * sizeof(int)) / 1024
                  << " KB)\n";
    }

    std::cout << "\n";
    if (failures == 0)
        std::cout << "Chapter 10 verification suite: all checks pass\n";
    else
        std::cout << failures << " check(s) failed\n";
    return failures == 0 ? 0 : 1;
}
