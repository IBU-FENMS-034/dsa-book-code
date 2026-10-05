// Reproduces every empirical table in Chapter 8.
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <string>

// An instrumented merge that counts key comparisons. `moves` counts writes
// into the array, which is what tells us whether a "cheap" sort really is
// cheap: a merge that makes no comparisons still copies every element.
static long long comparisons = 0;
static long long moves = 0;

void merge_counted(int* a, int* aux, int low, int mid, int high) {
    for (int k = low; k <= high; k++) aux[k] = a[k];
    int i = low, j = mid + 1;
    for (int k = low; k <= high; k++) {
        moves++;
        if (i > mid)            a[k] = aux[j++];
        else if (j > high)      a[k] = aux[i++];
        else { comparisons++;
               if (aux[j] < aux[i]) a[k] = aux[j++]; else a[k] = aux[i++]; }
    }
}

// The same merges, on the bottom-up schedule: runs of 1, then 2, then 4.
void bottom_up_counted(int* a, int* aux, int len) {
    for (int size = 1; size < len; ) {
        for (int low = 0; low < len - size; ) {
            int mid = low + size - 1;
            int high = mid + std::min(size, len - mid - 1);
            merge_counted(a, aux, low, mid, high);
            low = high + 1;
        }
        if (size > len / 2) break;
        size *= 2;
    }
}

// Timsort, counted, and with the two halves of its work kept apart.
static long long ins_compares = 0, ins_moves = 0;

int run_length(int len, int threshold) {
    int length = len, remainder = 0;
    while (length >= threshold) { remainder |= (length & 1); length >>= 1; }
    return length + remainder;
}
void insertion_counted(int* a, int low, int high) {
    for (int i = low + 1; i <= high; i++) {
        int key = a[i], j = i - 1;
        while (j >= low && (++ins_compares, key < a[j])) {
            a[j + 1] = a[j]; ins_moves++; j--;
        }
        a[j + 1] = key; ins_moves++;
    }
}
void tim_counted(int* a, int* aux, int len, int threshold) {
    if (len < 2) return;
    int run = run_length(len, threshold);
    for (int low = 0; low < len; ) {
        int high = low + std::min(run, len - low) - 1;
        insertion_counted(a, low, high);
        low = high + 1;
    }
    for (int size = run; size < len; ) {
        for (int low = 0; low < len - size; ) {
            int mid = low + size - 1;
            int high = mid + std::min(size, len - mid - 1);
            merge_counted(a, aux, low, mid, high);
            low = high + 1;
        }
        if (size > len / 2) break;
        size *= 2;
    }
}
void sort_counted(int* a, int* aux, int low, int high) {
    if (high <= low) return;
    int mid = low + (high - low) / 2;
    sort_counted(a, aux, low, mid);
    sort_counted(a, aux, mid + 1, high);
    merge_counted(a, aux, low, mid, high);
}
long long count_for(std::vector<int> v) {
    comparisons = 0;
    std::vector<int> aux(v.size());
    if (!v.empty()) sort_counted(v.data(), aux.data(), 0, (int)v.size() - 1);
    return comparisons;
}

// Every counter, for whichever of the three sorts is asked for.
struct Tally { long long compares, moves, ins_compares, ins_moves; };

Tally run_sort(const char* which, std::vector<int> v, int threshold = 64) {
    comparisons = moves = ins_compares = ins_moves = 0;
    std::vector<int> aux(v.size());
    int n = (int) v.size();
    if (n > 1) {
        if (std::string(which) == "top-down")
            sort_counted(v.data(), aux.data(), 0, n - 1);
        else if (std::string(which) == "bottom-up")
            bottom_up_counted(v.data(), aux.data(), n);
        else
            tim_counted(v.data(), aux.data(), n, threshold);
    }
    for (int i = 1; i < n; i++)
        if (v[i] < v[i - 1]) { std::cout << "  !! " << which << " did not sort\n"; break; }
    return {comparisons, moves, ins_compares, ins_moves};
}

// Uninstrumented copies, for the clock. Counting inside the inner loop costs
// more than the loop does, so the timings have to run without it.
void merge_plain(int* a, int* aux, int low, int mid, int high) {
    for (int k = low; k <= high; k++) aux[k] = a[k];
    int i = low, j = mid + 1;
    for (int k = low; k <= high; k++) {
        if (i > mid)                a[k] = aux[j++];
        else if (j > high)          a[k] = aux[i++];
        else if (aux[j] < aux[i])   a[k] = aux[j++];
        else                        a[k] = aux[i++];
    }
}
void sort_plain(int* a, int* aux, int low, int high) {
    if (high <= low) return;
    int mid = low + (high - low) / 2;
    sort_plain(a, aux, low, mid);
    sort_plain(a, aux, mid + 1, high);
    merge_plain(a, aux, low, mid, high);
}
void insertion_plain(int* a, int low, int high) {
    for (int i = low + 1; i <= high; i++) {
        int key = a[i], j = i - 1;
        while (j >= low && key < a[j]) { a[j + 1] = a[j]; j--; }
        a[j + 1] = key;
    }
}
void tim_plain(int* a, int* aux, int len, int threshold) {
    if (len < 2) return;
    int run = run_length(len, threshold);
    for (int low = 0; low < len; ) {
        int high = low + std::min(run, len - low) - 1;
        insertion_plain(a, low, high);
        low = high + 1;
    }
    for (int size = run; size < len; ) {
        for (int low = 0; low < len - size; ) {
            int mid = low + size - 1;
            int high = mid + std::min(size, len - mid - 1);
            merge_plain(a, aux, low, mid, high);
            low = high + 1;
        }
        if (size > len / 2) break;
        size *= 2;
    }
}

// The fastest of as many runs as fit in a quarter of a second, as in Chapter 7.
double time_it(const char* which, const std::vector<int>& v) {
    double best = 1e30, spent = 0;
    int n = (int) v.size();
    std::vector<int> aux(n);
    for (int run = 0; run < 60 && spent < 1.5; run++) {
        std::vector<int> a = v;
        auto start = std::chrono::steady_clock::now();
        if (std::string(which) == "top-down") sort_plain(a.data(), aux.data(), 0, n - 1);
        else                                  tim_plain(a.data(), aux.data(), n, 64);
        auto stop = std::chrono::steady_clock::now();
        double t = std::chrono::duration<double>(stop - start).count();
        if (t < best) best = t;
        spent += t;
    }
    return best;
}

// Builds an input that forces the maximum number of comparisons: the inverse
// of what a full merge-sort run consumes, generated by "un-merging" a sorted array.
void build_worst(std::vector<int>& dst, std::vector<int>& src, int low, int high) {
    if (high <= low) return;
    int mid = low + (high - low) / 2;
    // Deal alternately so that every merge alternates between the two runs.
    std::vector<int> left, right;
    for (int k = low, t = 0; k <= high; k++, t++)
        (t % 2 == 0 ? left : right).push_back(dst[k]);
    for (int k = low, t = 0; t < (int)left.size(); k++, t++)  src[k] = left[t];
    for (int k = low + (int)left.size(), t = 0; t < (int)right.size(); k++, t++) src[k] = right[t];
    for (int k = low; k <= high; k++) dst[k] = src[k];
    build_worst(dst, src, low, mid);
    build_worst(dst, src, mid + 1, high);
}

int main() {
    // The sizes are Chapter 7's, so that the merge sort column can be read
    // straight against Table 7.2 rather than against a different experiment.
    std::cout << "TABLE 0  Seconds to sort a shuffled array, merge sort\n";
    std::cout << std::setw(12) << "N" << std::setw(12) << "seconds" << "\n";
    {
        std::mt19937 r0(20260914);
        for (int n : {2000, 4000, 8000, 16000, 32000, 64000,
                      100000, 1000000, 10000000}) {
            std::vector<int> v(n);
            for (int i = 0; i < n; i++) v[i] = i;
            std::shuffle(v.begin(), v.end(), r0);
            std::vector<int> aux(n);
            // At least three runs, and more while they stay cheap, because
            // the small sizes finish inside the clock's own noise.
            double best = 1e30, spent = 0;
            for (int run = 0; run < 2000 && (run < 3 || spent < 0.5); run++) {
                std::vector<int> a = v;
                auto start = std::chrono::steady_clock::now();
                sort_plain(a.data(), aux.data(), 0, n - 1);
                auto stop = std::chrono::steady_clock::now();
                double t = std::chrono::duration<double>(stop - start).count();
                if (t < best) best = t;
                spent += t;
            }
            std::cout << std::setw(12) << n << std::setw(12)
                      << std::fixed << std::setprecision(5) << best << "\n";
        }
    }

    std::cout << "\nTABLE 1  Key comparisons, top-down merge sort\n";
    std::cout << std::setw(9) << "N" << std::setw(14) << "sorted"
              << std::setw(14) << "random" << std::setw(14) << "worst"
              << std::setw(14) << "N lg N" << std::setw(12) << "worst/NlgN" << "\n";
    std::mt19937 rng(42);
    for (int n : {16, 64, 256, 1024, 4096, 16384, 65536}) {
        std::vector<int> sorted(n), random_v(n);
        for (int i = 0; i < n; i++) sorted[i] = i;
        random_v = sorted;
        std::shuffle(random_v.begin(), random_v.end(), rng);

        std::vector<int> worst(n), scratch(n);
        for (int i = 0; i < n; i++) worst[i] = i;
        build_worst(worst, scratch, 0, n - 1);

        long long cs = count_for(sorted), cr = count_for(random_v), cw = count_for(worst);
        double nlgn = n * std::log2((double)n);
        std::cout << std::setw(9) << n << std::setw(14) << cs << std::setw(14) << cr
                  << std::setw(14) << cw << std::setw(14) << (long long)nlgn
                  << std::setw(12) << std::fixed << std::setprecision(3) << (cw / nlgn) << "\n";
    }

    std::cout << "\nTABLE 2  Sorted-input comparisons as a fraction of the worst case\n";
    for (int n : {1024, 4096, 16384, 65536}) {
        std::vector<int> sorted(n);
        for (int i = 0; i < n; i++) sorted[i] = i;
        std::vector<int> worst(n), scratch(n);
        for (int i = 0; i < n; i++) worst[i] = i;
        build_worst(worst, scratch, 0, n - 1);
        double ratio = (double)count_for(sorted) / (double)count_for(worst);
        std::cout << std::setw(9) << n << "   ratio = "
                  << std::fixed << std::setprecision(3) << ratio << "\n";
    }

    // ------------------------------------------- bottom-up against top-down --
    // Each table from here on seeds its own generator. Sharing one means that
    // adding a table anywhere above silently changes every random figure below
    // it, and those figures are printed in the book.
    std::mt19937 r3(1001);
    std::cout << "\nTABLE 3  Comparisons: top-down against bottom-up\n";
    std::cout << std::setw(9) << "N" << std::setw(10) << "input"
              << std::setw(14) << "top-down" << std::setw(14) << "bottom-up"
              << std::setw(12) << "difference" << "\n";
    for (int n : {1024, 1000, 2048, 2067, 65536, 100000}) {
        std::vector<int> sorted(n);
        for (int i = 0; i < n; i++) sorted[i] = i;
        std::vector<int> rnd = sorted;
        std::shuffle(rnd.begin(), rnd.end(), r3);
        std::vector<int> rev = sorted;
        std::reverse(rev.begin(), rev.end());
        const char* names[] = {"sorted", "random", "reversed"};
        std::vector<int>* inputs[] = {&sorted, &rnd, &rev};
        for (int k = 0; k < 3; k++) {
            Tally td = run_sort("top-down", *inputs[k]);
            Tally bu = run_sort("bottom-up", *inputs[k]);
            std::cout << std::setw(9) << n << std::setw(10) << names[k]
                      << std::setw(14) << td.compares << std::setw(14) << bu.compares
                      << std::setw(12) << (bu.compares - td.compares) << "\n";
        }
    }

    // How far apart the two can get at sizes that are not powers of two.
    {
        std::mt19937 rs(2002);
        double sum = 0, biggest = 0; int cnt = 0, at = 0;
        for (int n = 1000; n <= 100000; n += 97) {
            std::vector<int> v(n);
            for (int i = 0; i < n; i++) v[i] = i;
            std::shuffle(v.begin(), v.end(), rs);
            long long a = run_sort("top-down", v).compares;
            long long b = run_sort("bottom-up", v).compares;
            double d = std::fabs((double)(b - a)) / a;
            sum += d; cnt++;
            if (d > biggest) { biggest = d; at = n; }
        }
        std::cout << "  over N = 1,000 to 100,000: bottom-up differs by "
                  << std::fixed << std::setprecision(1) << (100 * sum / cnt)
                  << "% on average, at most " << (100 * biggest)
                  << "% (at N = " << at << ")\n";
    }

    // ------------------------------------------------------ what Timsort does --
    std::mt19937 r4(3003);
    std::cout << "\nTABLE 4  Timsort's two halves, threshold 64\n";
    std::cout << std::setw(9) << "N" << std::setw(10) << "input"
              << std::setw(8) << "run" << std::setw(12) << "runs"
              << std::setw(14) << "insertion" << std::setw(14) << "merging"
              << std::setw(14) << "total cmp" << std::setw(14) << "merge sort"
              << "\n";
    for (int n : {1024, 16384, 262144, 1048576}) {
        std::vector<int> sorted(n);
        for (int i = 0; i < n; i++) sorted[i] = i;
        std::vector<int> rnd = sorted;
        std::shuffle(rnd.begin(), rnd.end(), r4);
        std::vector<int> rev = sorted;
        std::reverse(rev.begin(), rev.end());
        const char* names[] = {"sorted", "random", "reversed"};
        std::vector<int>* inputs[] = {&sorted, &rnd, &rev};
        int r = run_length(n, 64);
        for (int k = 0; k < 3; k++) {
            Tally t = run_sort("timsort", *inputs[k]);
            Tally m = run_sort("top-down", *inputs[k]);
            std::cout << std::setw(9) << n << std::setw(10) << names[k]
                      << std::setw(8) << r << std::setw(12) << (n + r - 1) / r
                      << std::setw(14) << t.ins_compares << std::setw(14) << t.compares
                      << std::setw(14) << (t.ins_compares + t.compares)
                      << std::setw(14) << m.compares << "\n";
        }
    }

    // Moves, which is the number that says whether "cheap" really is cheap.
    std::mt19937 r5(4004);
    std::cout << "\nTABLE 5  Array writes, and the seconds they take\n";
    std::cout << std::setw(9) << "N" << std::setw(10) << "input"
              << std::setw(16) << "merge writes" << std::setw(16) << "Tim writes"
              << std::setw(12) << "merge s" << std::setw(12) << "Tim s" << "\n";
    for (int n : {262144, 1048576, 4194304}) {
        std::vector<int> sorted(n);
        for (int i = 0; i < n; i++) sorted[i] = i;
        std::vector<int> rnd = sorted;
        std::shuffle(rnd.begin(), rnd.end(), r5);
        std::vector<int> rev = sorted;
        std::reverse(rev.begin(), rev.end());
        const char* names[] = {"sorted", "random", "reversed"};
        std::vector<int>* inputs[] = {&sorted, &rnd, &rev};
        for (int k = 0; k < 3; k++) {
            Tally m = run_sort("top-down", *inputs[k]);
            Tally t = run_sort("timsort", *inputs[k]);
            std::cout << std::setw(9) << n << std::setw(10) << names[k]
                      << std::setw(16) << m.moves
                      << std::setw(16) << (t.moves + t.ins_moves)
                      << std::setw(12) << std::fixed << std::setprecision(4)
                      << time_it("top-down", *inputs[k])
                      << std::setw(12) << time_it("timsort", *inputs[k]) << "\n";
        }
    }

    // How the run length behaves as N grows: it stays inside [T/2, T].
    std::cout << "\nTABLE 6  Run length against the threshold, T = 64\n";
    std::cout << std::setw(12) << "N" << std::setw(10) << "run"
              << std::setw(12) << "runs" << "\n";
    for (int n : {19, 64, 100, 1000, 10000, 100000, 1000000, 10000000}) {
        int r = run_length(n, 64);
        std::cout << std::setw(12) << n << std::setw(10) << r
                  << std::setw(12) << (n + r - 1) / r << "\n";
    }
    return 0;
}
