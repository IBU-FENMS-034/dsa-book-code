// The array facts quoted in Chapter 3, section "Arrays, and How They Work".
// Run it yourself and compare the addresses it prints with Figure 3.1:
//   c++ -std=c++17 -Wall -Wextra -O2 -o arrays arrays.cpp && ./arrays
#include <cstdio>
#include <cstdint>
#include <vector>

// An address is only a number, and std::uintptr_t is an
// integer type guaranteed to be wide enough to hold one.
static std::uintptr_t address_of(const int* p) {
    return reinterpret_cast<std::uintptr_t>(p);
}

int main() {
    int scores[5] = {90, 72, 85, 61, 78};

    std::printf("sizeof(int) = %zu bytes, scores holds %zu\n",
                sizeof(int), sizeof(scores) / sizeof(scores[0]));

    const auto start = address_of(scores);

    for (int i = 0; i < 5; ++i) {
        const auto here = address_of(scores + i);
        std::printf(
            "scores[%d] = %2d  at %p  (+%2d bytes)\n",
            i, scores[i], static_cast<void*>(scores + i),
            static_cast<int>(here - start));
    }

    // scores + i and &scores[i] name the same address, which is the
    // whole point: indexing is arithmetic, not a search.
    for (int i = 0; i < 5; ++i)
        if (scores + i != &scores[i]) { std::printf("FAIL  addresses\n"); return 1; }

    // A vector is an array that owns its block and replaces it with a
    // larger one when it fills up. The elements are still contiguous.
    std::vector<int> grown;
    for (int i = 0; i < 5; ++i) grown.push_back(scores[i]);
    for (int i = 0; i < 5; ++i)
        if (&grown[i] != grown.data() + i) { std::printf("FAIL  vector\n"); return 1; }

    std::printf("all checks passed\n");
    return 0;
}
