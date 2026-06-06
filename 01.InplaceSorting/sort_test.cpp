#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <random>
#include "sorting.h"


template <typename T>
void verify_sorting(std::vector<T> data) {
    std::vector<T> expected = data;
    std::sort(expected.begin(), expected.end());

    std::vector<T> bubble_data = data;
    bubble_sort(bubble_data.begin(), bubble_data.end());
    EXPECT_EQ(bubble_data, expected) << "Bubble sort failed";

    std::vector<T> quick_data = data;
    quick_sort(quick_data.begin(), quick_data.end());
    EXPECT_EQ(quick_data, expected) << "Quick sort failed";
}

// Пустой вектор
TEST(SortingTests, EmptyVector) {
    std::vector<int> data = {};
    verify_sorting(data);
}

// Один элемент
TEST(SortingTests, SingleElement) {
    std::vector<int> data = { 42 };
    verify_sorting(data);
}

// Отсортированные данные
TEST(SortingTests, AlreadySorted) {
    std::vector<int> data = { 1, 2, 3, 4, 5, 10, 20 };
    verify_sorting(data);
}

// Обратный порядок
TEST(SortingTests, ReverseSorted) {
    std::vector<int> data = { 10, 8, 6, 4, 2, 0, -5 };
    verify_sorting(data);
}

// Потворяющиеся элементы
TEST(SortingTests, Duplicates) {
    std::vector<int> data = { 3, 1, 2, 3, 1, 3, 2 };
    verify_sorting(data);
}

// Слцчайные данные
TEST(SortingTests, RandomLarge) {
    const size_t size = 500;
    std::vector<int> data(size);

    std::mt19937 gen(42); 
    std::uniform_int_distribution<> dis(-1000, 1000);

    for (auto& val : data) val = dis(gen);

    verify_sorting(data);
}

// Отрицательные числа
TEST(SortingTests, NegativeNumbers) {
    std::vector<int> data = { -1, -5, -2, 0, -10, 3 };
    verify_sorting(data);
}