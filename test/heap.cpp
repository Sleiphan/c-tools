#include <gtest/gtest.h>

extern "C" {
    #include "ctools/heap.h"
}

int compare_ints(const void* lhs, const void* rhs) {
    return (*(int*)rhs - *(int*)lhs);
}

TEST(heap_verify, base_case) {
    const int heap_array[] = { 9, 8, 7, 6, 5, 4, 3, 2, 1 };

    EXPECT_EQ(heap_verify(heap_array, 9, sizeof(int), compare_ints), 0);
}

TEST(heap_verify, even_array_size) {
    const int heap_array[] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

    EXPECT_EQ(heap_verify(heap_array, 10, sizeof(int), compare_ints), 0);
}

TEST(heap_verify, swapped_largest_child) {
    const int heap_array[] = { 9, 7, 8, 5, 6, 3, 4, 1, 2 };

    EXPECT_EQ(heap_verify(heap_array, 9, sizeof(int), compare_ints), 0);
}

TEST(heap_verify, voilation_counts) {
    const int heap_array_1_violations[] = { 8, 9, 7, 6, 5, 4, 3, 2, 1 };
    const int heap_array_2_violations[] = { 8, 9, 4, 6, 5, 7, 3, 2, 1 };
    const int heap_array_3_violations[] = { 8, 9, 4, 2, 5, 7, 3, 6, 1 };

    EXPECT_EQ(heap_verify(heap_array_1_violations, 9, sizeof(int), compare_ints), 1);
    EXPECT_EQ(heap_verify(heap_array_2_violations, 9, sizeof(int), compare_ints), 2);
    EXPECT_EQ(heap_verify(heap_array_3_violations, 9, sizeof(int), compare_ints), 3);
}

void print_array(int* array, size_t array_size) {
    for (int* it = array; it < array + array_size; it++)
        printf("%d ", *it);
    
    printf("\n");
}

TEST(heap_build, base_case) {
    int heap_array[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    heap_build(heap_array, 9, sizeof(int), compare_ints);

    print_array(heap_array, 9);

    EXPECT_EQ(heap_verify(heap_array, 9, sizeof(int), compare_ints), 0);
}

TEST(heap_build, even_array_size) {
    int heap_array[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    heap_build(heap_array, 10, sizeof(int), compare_ints);

    print_array(heap_array, 10);

    EXPECT_EQ(heap_verify(heap_array, 9, sizeof(int), compare_ints), 0);
}