#include <gtest/gtest.h>


#define QUEUE_NAME second_queue_type
#define QUEUE_TYPE int
#define QUEUE_INDEX unsigned int
// This #include section is only to verify that the header
// supports multiple inclusions in a compilation unit
extern "C" {
    #include "ctools/queue2.h"
}

#undef QUEUE_NAME
#undef QUEUE_INDEX
#define QUEUE_NAME queue
#define QUEUE_INDEX unsigned char
extern "C" {
    #include "ctools/queue2.h"
    #include <errno.h>
}

// TEST(queue, base_case) {
//     int input_numbers[] = {0, 1, 2, 3, 4, 5};
//     const int BUFFER_SIZE = sizeof(input_numbers) / sizeof(input_numbers[0]);
//     int output_numbers[BUFFER_SIZE];

//     memset(output_numbers, 0, sizeof(output_numbers));

//     queue* q = queue_create();

//     for (int i = 0; i < BUFFER_SIZE; i++)
//         queue_push(q, (void*)&input_numbers[i]);
    
//     for (int i = 0; i < BUFFER_SIZE; i++)
//         output_numbers[i] = *((int*)queue_pop(q));
    
//     queue_destroy(q);

//     for (int i = 0; i < BUFFER_SIZE; i++)
//         EXPECT_EQ(input_numbers[i], output_numbers[i]);
// }

// TEST(queue, emptying_and_reusing) {
//     const int test_value_1 = 42;
//     const int test_value_2 = 420;
    
//     queue* q = queue_create();

//     for (int i = 0; i < 10; i++) {
//         queue_push(q, (void*)&test_value_1);
//         queue_push(q, (void*)&test_value_2);
//         EXPECT_EQ(*((int*)queue_pop(q)), test_value_1);
//         EXPECT_EQ(*((int*)queue_pop(q)), test_value_2);
//     }
    
//     queue_destroy(q);
// }

TEST(queue, mask_is_created_correctly) {
    queue* q = queue_create(7);
    EXPECT_EQ(q->capacity_mask, 7);
    queue_destroy(q);

    q = queue_create(8);
    EXPECT_EQ(q->capacity_mask, 15);
    queue_destroy(q);

    q = queue_create(9);
    EXPECT_EQ(q->capacity_mask, 15);
    queue_destroy(q);
}



TEST(queue, not_lying_about_minimum_capacity) {
    const int queue_min_size = 8;

    queue* q = queue_create(queue_min_size);

    int numbers_pushed = 0;

    while ((numbers_pushed < queue_min_size) & !queue_push(q, 0))
        numbers_pushed++;
    
    EXPECT_EQ(numbers_pushed, queue_min_size);

    queue_destroy(q);
}



TEST(queue, rejects_capacities_over_supported_values) {
    queue* q = queue_create(queue_max_size);

    EXPECT_EQ(q, NULL);

    if (q)
        queue_destroy(q);
}



TEST(queue, base_case) {
    constexpr QUEUE_INDEX capacity = (1 << 3) - 1;
    constexpr QUEUE_INDEX half_capacity = capacity / 2;

    queue* q = queue_create(capacity);

    int error = 0;

    // Push the queue halfway through its capacity
    for (QUEUE_TYPE i = 0; i < half_capacity; i++)
        error |= queue_push(q, i);

    // Pop the queue halfway through its capacity
    for (QUEUE_TYPE i = 0; i < half_capacity; i++) {
        QUEUE_TYPE sink;
        error |= queue_pop(q, &sink);
        EXPECT_EQ(sink, i);
    }

    for (QUEUE_TYPE i = 0; i < capacity; i++)
        error |= queue_push(q, i);
    
    for (QUEUE_TYPE i = 0; i < capacity; i++) {
        QUEUE_TYPE sink;
        error |= queue_pop(q, &sink);
        EXPECT_EQ(sink, i);
    }

    // Verify that no errors occurred
    EXPECT_EQ(error, 0);
    EXPECT_EQ(errno, 0);
    
    queue_destroy(q);
}

TEST(queue, size_reports_correctly) {
    constexpr QUEUE_INDEX capacity = queue_max_size - 1;
    constexpr QUEUE_INDEX half_capacity = capacity / 2;

    queue* q = queue_create(capacity);
    EXPECT_EQ(queue_size(q), 0);

    // Push the queue halfway through its capacity
    for (QUEUE_TYPE i = 0; i < half_capacity; i++) {
        queue_push(q, i);
        EXPECT_EQ(queue_size(q), i + 1);
    }

    // Pop the queue halfway through its capacity
    for (QUEUE_TYPE i = half_capacity; i > 0; i--) {
        QUEUE_TYPE sink;
        queue_pop(q, &sink);
        EXPECT_EQ(queue_size(q), i - 1);
    }

    for (QUEUE_TYPE i = 0; i < capacity; i++) {
        queue_push(q, i);
        EXPECT_EQ(queue_size(q), i + 1);
    }
    
    for (QUEUE_TYPE i = capacity; i > 0; i--) {
        QUEUE_TYPE sink;
        queue_pop(q, &sink);
        EXPECT_EQ(queue_size(q), i - 1);
    }

    // Verify that no errors occurred
    EXPECT_EQ(errno, 0);
    
    queue_destroy(q);
}

TEST(queue, base_case_array) {
    constexpr QUEUE_INDEX capacity = (queue_max_size) - 1;
    constexpr QUEUE_INDEX half_capacity = capacity / 2;

    queue* q = queue_create(capacity);



    QUEUE_TYPE values[capacity], result[capacity];
    for (QUEUE_TYPE i = 0; i < capacity; i++)
        values[i] = i;
    memset(result, 0, capacity * sizeof(QUEUE_TYPE));

    int elements_pushed = queue_push_array(q, values, half_capacity);
    int elements_popped = queue_pop_array(q, result, half_capacity);

    EXPECT_TRUE(0 == memcmp(result, values, half_capacity * sizeof(QUEUE_TYPE)));
    EXPECT_EQ(elements_pushed, half_capacity);
    EXPECT_EQ(elements_popped, half_capacity);
    memset(result, 0, half_capacity * sizeof(QUEUE_TYPE));



    elements_pushed = queue_push_array(q, values, capacity);
    elements_popped = queue_pop_array(q, result, capacity);

    EXPECT_TRUE(0 == memcmp(result, values, capacity * sizeof(QUEUE_TYPE)));
    EXPECT_EQ(elements_pushed, capacity);
    EXPECT_EQ(elements_popped, capacity);



    // Verify that no errors occurred
    EXPECT_EQ(errno, 0);
    
    queue_destroy(q);
}