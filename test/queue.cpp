#include <gtest/gtest.h>

extern "C" {
    #include "ctools/queue.h"
}

TEST(queue, base_case) {
    int input_numbers[] = {0, 1, 2, 3, 4, 5};
    const int BUFFER_SIZE = sizeof(input_numbers) / sizeof(input_numbers[0]);
    int output_numbers[BUFFER_SIZE];

    memset(output_numbers, 0, sizeof(output_numbers));

    queue* q = queue_create();

    for (int i = 0; i < BUFFER_SIZE; i++)
        queue_push(q, (void*)&input_numbers[i]);
    
    for (int i = 0; i < BUFFER_SIZE; i++)
        output_numbers[i] = *((int*)queue_pop(q));
    
    queue_destroy(q);

    for (int i = 0; i < BUFFER_SIZE; i++)
        EXPECT_EQ(input_numbers[i], output_numbers[i]);
}

TEST(queue, emptying_and_reusing) {
    const int test_value_1 = 42;
    const int test_value_2 = 420;
    
    queue* q = queue_create();

    for (int i = 0; i < 10; i++) {
        queue_push(q, (void*)&test_value_1);
        queue_push(q, (void*)&test_value_2);
        EXPECT_EQ(*((int*)queue_pop(q)), test_value_1);
        EXPECT_EQ(*((int*)queue_pop(q)), test_value_2);
    }
    
    queue_destroy(q);
}