#include <gtest/gtest.h>

extern "C" {
    #include "ctools/stack.h"
}

TEST(stack, elements_are_popped_in_the_correct_order) {
    stack s;
    stack_create(&s);

    int expected[] = { 0, 1, 2, 3, 4, 5 };
    const int NUM_VALUES = sizeof(expected) / sizeof(expected[0]);
    int actual[NUM_VALUES];

    // Insert values from 'expected' into the stack in reverse order
    for (int i = NUM_VALUES - 1; i >= 0; i--)
        stack_push(&s, &expected[i]);
    
    // Read from the stack into 'actual'
    for (int i = 0; i < NUM_VALUES; i++)
        actual[i] = *((int*)stack_pop(&s));
    
    // Verify
    for (int i = 0; i < NUM_VALUES; i++)
        EXPECT_EQ(expected[i], actual[i]);
    
    // Cleanup
    stack_destroy(&s);
}

TEST(stack, popping_empty_stack_returns_null) {
    stack s;
    stack_create(&s);

    EXPECT_EQ(stack_pop(&s), NULL);
    
    stack_destroy(&s);
}



#pragma region stack_push_sort

int stack_push_sort_tests_comp(const void* lhs_p, const void* rhs_p) {
    int lhs = *((int*)lhs_p);
    int rhs = *((int*)rhs_p);
    if (lhs > rhs) return  1;
    if (lhs < rhs) return -1;
    return 0;
}

void stack_push_sort_helper_function(const int expected[5], int initial_stack_content[4], int insertion_value) {
    int actual[] = { -1, -1, -1, -1, -1 };

    stack s;
    stack_create(&s);

    // Push initial_stack_content in reverse order into the stack
    for (int i = 3; i >= 0; i--)
        stack_push(&s, &initial_stack_content[i]);
    
    // Push the insertion value into the (hopefully) correct position
    stack_push_sort(&s, &insertion_value, stack_push_sort_tests_comp);

    // Pop all values into the 'actual' array to analyse the order of the elements
    for (int i = 0; i < 5; i++)
        actual[i] = *((int*)stack_pop(&s));

    // Compare actual ordering with expected ordering
    for (int i = 0; i < 5; i++)
        EXPECT_EQ(actual[i], expected[i]);
    
    // Cleanup
    stack_destroy(&s);
}

TEST(stack, push_sort_base_case) {
    const int expected[] = { 0, 1, 2, 3, 4 };

    int initial_stack_content[] = { 0, 1, 3, 4 };
    int insertion_value = 2;

    stack_push_sort_helper_function(expected, initial_stack_content, insertion_value);
}

TEST(stack, push_sort_inserting_at_the_top) {
    const int expected[] = { 0, 1, 2, 3, 4 };

    int initial_stack_content[] = { 1, 2, 3, 4 };
    int insertion_value = 0;

    stack_push_sort_helper_function(expected, initial_stack_content, insertion_value);
}

TEST(stack, push_sort_inserting_at_the_bottom) {
    const int expected[] = { 0, 1, 2, 3, 4 };

    int initial_stack_content[] = { 0, 1, 2, 3 };
    int insertion_value = 4;

    stack_push_sort_helper_function(expected, initial_stack_content, insertion_value);
}

TEST(stack, push_sort_insert_into_empty_stack) {
    const int expected = 42;
    int insertion_value = 42;
    int actual = -1;

    stack s;
    stack_create(&s);
    
    // Push the insertion value into the stack
    stack_push_sort(&s, &insertion_value, stack_push_sort_tests_comp);

    // Pop the value into the 'actual' variable
    actual = *((int*)stack_pop(&s));

    // Compare actual value with expected value
    EXPECT_EQ(actual, expected);
    
    // Cleanup
    stack_destroy(&s);
}

#pragma endregion