#include <gtest/gtest.h>
#include <thread>
#include <algorithm>
#include <chrono>

extern "C" {
    #include "ctools/stack.h"
}

void speed_difference_test(const int producer_delay_ms, const int consumer_delay_ms) {
    stack* s = stack_create();

    const int NUMBER_COUNT = 300;
    int producer_numbers[NUMBER_COUNT];
    int consumed_numbers[NUMBER_COUNT];

    for (int i = 0; i < NUMBER_COUNT; i++)
        producer_numbers[i] = i;
    
    std::thread producer([&s, &producer_numbers, &producer_delay_ms](){
        for (int i = 0; i < NUMBER_COUNT; i++) {
            stack_push(s, &producer_numbers[i]);
            std::this_thread::sleep_for(std::chrono::milliseconds(producer_delay_ms));
        }
    });

    std::thread consumer([&s, &consumed_numbers, &consumer_delay_ms](){
        for (int i = 0; i < NUMBER_COUNT; i++) {
            int* num = NULL;

            while (num == NULL) {
                num = (int*)stack_pop(s);
                std::this_thread::sleep_for(std::chrono::milliseconds(consumer_delay_ms));
            }

            consumed_numbers[i] = *num;
        }
    });

    producer.join();
    consumer.join();

    std::sort(consumed_numbers, &consumed_numbers[NUMBER_COUNT]);

    for (int i = 0; i < NUMBER_COUNT; i++)
        EXPECT_EQ(consumed_numbers[i], i);
    
    // Cleanup
    stack_destroy(s);
}

TEST(stack_concurrency, faster_producer) {
    speed_difference_test(1, 2);
}

TEST(stack_concurrency, faster_consumer) {
    speed_difference_test(2, 1);
}



int comp(const void* lhs_p, const void* rhs_p) {
    int lhs = *((int*)lhs_p);
    int rhs = *((int*)rhs_p);
    if (lhs > rhs) return  1;
    if (lhs < rhs) return -1;
    return 0;
}

void speed_difference_test_push_sort(const int producer_delay_ms, const int consumer_delay_ms) {
    stack* s = stack_create();

    const int NUMBER_COUNT = 300;
    int producer_numbers[NUMBER_COUNT];
    int consumed_numbers[NUMBER_COUNT];

    for (int i = 0; i < NUMBER_COUNT; i++)
        producer_numbers[i] = i;
    
    std::thread producer([&s, &producer_numbers, &producer_delay_ms](){
        for (int i = 0; i < NUMBER_COUNT; i++) {
            stack_push_sort(s, &producer_numbers[i], comp);
            std::this_thread::sleep_for(std::chrono::milliseconds(producer_delay_ms));
        }
    });

    std::thread consumer([&s, &consumed_numbers, &consumer_delay_ms](){
        for (int i = 0; i < NUMBER_COUNT; i++) {
            int* num = NULL;

            while (num == NULL) {
                num = (int*)stack_pop(s);
                std::this_thread::sleep_for(std::chrono::milliseconds(consumer_delay_ms));
            }

            consumed_numbers[i] = *num;
        }
    });

    producer.join();
    consumer.join();

    std::sort(consumed_numbers, &consumed_numbers[NUMBER_COUNT]);

    for (int i = 0; i < NUMBER_COUNT; i++)
        EXPECT_EQ(consumed_numbers[i], i);
    
    // Cleanup
    stack_destroy(s);
}

TEST(stack_concurrency, faster_producer_push_sort) {
    speed_difference_test_push_sort(1, 2);
}

TEST(stack_concurrency, faster_consumer_push_sort) {
    speed_difference_test_push_sort(2, 1);
}
