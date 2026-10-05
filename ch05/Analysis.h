#ifndef CH05_ANALYSIS_H
#define CH05_ANALYSIS_H

// The small programs Chapter 5 analyses. None of them is interesting in
// itself; each is the shortest piece of code with a particular shape, so that
// the cost of the shape is the only thing being counted.
namespace Analysis {

    // How many pairs of distinct positions hold values that sum to zero.
    // The inner loop starts at i + 1, so each pair is looked at once.
    inline int count_pairs(const int* arr, int len) {
        int found = 0;
        for (int i = 0; i < len; ++i)
            for (int j = i + 1; j < len; ++j)
                if (arr[i] + arr[j] == 0) ++found;
        return found;
    }

    // The same question for triples. One more loop, one more factor of N.
    inline int count_triples(const int* arr, int len) {
        int found = 0;
        for (int i = 0; i < len; ++i)
            for (int j = i + 1; j < len; ++j)
                for (int k = j + 1; k < len; ++k)
                    if (arr[i] + arr[j] + arr[k] == 0) ++found;
        return found;
    }

    // One pass, so the work is proportional to the length.
    inline long long total(const int* arr, int len) {
        long long sum = 0;
        for (int i = 0; i < len; ++i) sum += arr[i];
        return sum;
    }

    // How many times n can be halved before it reaches 1. This is the shape
    // every halving algorithm has, and the answer is the base-2 logarithm.
    inline int halvings(int n) {
        int steps = 0;
        while (n > 1) {
            n /= 2;
            ++steps;
        }
        return steps;
    }

}

#endif
