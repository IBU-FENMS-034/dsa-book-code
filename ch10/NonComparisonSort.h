#ifndef CH10_NON_COMPARISON_SORT_H
#define CH10_NON_COMPARISON_SORT_H

// Two sorts that never compare one key with another. Both use a key as an
// array index instead, which is what restricts them to non-negative
// integers. Section 10.5 says what that restriction costs.
namespace NonComparisonSort {

    // Sort by counting how many of each value there are. Allocates an array
    // as long as the largest element, so it wants a small range of values.
    template <typename Data>
    void counting_sort(Data* arr, int len);

    // Sort by counting sort on one digit at a time, least significant first.
    // The memory depends on the number of digits rather than on the largest
    // value, which is what makes it usable on a wide range.
    template <typename Data>
    void radix_sort(Data* arr, int len);

    namespace detail {
        // One stable counting sort of arr[0..len-1] on the digit that exp
        // selects: exp = 1 is the units, 10 the tens, and so on.
        template <typename Data>
        void sort(Data* arr, int len, int exp);

        template <typename Data>
        Data get_max(Data* arr, int len);

        // Only ever used to check the precondition, so it disappears from a
        // release build along with the assertion that calls it.
        template <typename Data>
        Data get_min(Data* arr, int len);
    }

}

#include "NonComparisonSort.tpp"

#endif
