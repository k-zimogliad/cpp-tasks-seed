#ifndef SORTING_H
#define SORTING_H

#include "collvalue.h"
#include <iterator>
#include <algorithm>

// BubbleSort: O(N^2)) ---
template <typename Iterator>
void bubble_sort(Iterator begin, Iterator end) {
    if (begin == end) return;
    bool swapped = true;
    while (swapped) {
        swapped = false;
        for (Iterator i = begin; std::next(i) != end; ++i) {
            if (*std::next(i) < *i) {
                std::swap(*i, *std::next(i));
                swapped = true;
            }
        }
        --end;
    }
}

// QuickSort: O(N log N)
template <typename Iterator>
void quick_sort(Iterator begin, Iterator end) {
    auto const range = std::distance(begin, end);
    if (range <= 1) return;

    auto pivot = *std::next(begin, range / 2);

    Iterator i = begin;
    Iterator j = std::prev(end);

    while (std::distance(i, j) >= 0) {
        while (*i < pivot) ++i;
        while (pivot < *j) --j;

        if (std::distance(i, j) >= 0) {
            std::swap(*i, *j);
            ++i;
            if (j != begin) --j;
            else break;
        }
    }

    if (std::distance(begin, j) > 0) quick_sort(begin, std::next(j));
    if (std::distance(i, end) > 0) quick_sort(i, end);
}

#endif 