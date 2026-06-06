#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <iomanip>
#include <numeric>

#include "collvalue.h"
#include "sorting.h"

using CollInt = CollectingValue<int>;

std::vector<CollInt> generate_data(size_t n) {
    std::vector<CollInt> data(n);
    std::iota(data.begin(), data.end(), CollInt(0));
    return data;
}

void shuffle_data(std::vector<CollInt>& data) {
    std::shuffle(data.begin(), data.end(), std::mt19937{ std::random_device{}() });
}

int main() {
    std::vector<size_t> sizes = { 100, 500, 1000, 2000 };

    std::cout << "N\tAlgo\t\tComps\tSwaps\tMoves\n";
    std::cout << "--------------------------------------------------\n";

    for (size_t n : sizes) {
        auto number_data = generate_data(n);

        // --- Std Sort ---
        shuffle_data(number_data);
        CollInt::reset_stats();
        std::sort(number_data.begin(), number_data.end());
        std::cout << n << "\tstd::sort\t" << CollInt::comps << "\t" << CollInt::swaps << "\t" << CollInt::moves << "\n";

        // --- Bubble Sort ---
        shuffle_data(number_data);
        CollInt::reset_stats();
        bubble_sort(number_data.begin(), number_data.end());
        std::cout << n << "\tbubble_sort\t" << CollInt::comps << "\t" << CollInt::swaps << "\t" << CollInt::moves << "\n";

        // --- Quick Sort ---
        shuffle_data(number_data);
        CollInt::reset_stats();
        quick_sort(number_data.begin(), number_data.end());
        std::cout << n << "\tquick_sort\t" << CollInt::comps << "\t" << CollInt::swaps << "\t" << CollInt::moves << "\n";

        std::cout << "--------------------------------------------------\n";
    }

    return 0;
}