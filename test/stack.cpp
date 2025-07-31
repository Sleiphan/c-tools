#include <gtest/gtest.h>

#define STACK_NAME stack
#define STACK_TYPE int
extern "C" {
    #include "ctools/stack.h"
}

TEST(stack, elements_are_popped_in_the_correct_order) {
    stack* s = stack_create(0);

    const int NUM_VALUES = 10;
    int actual[NUM_VALUES];

    // Insert values into the stack in descending order
    for (int i = NUM_VALUES - 1; i >= 0; i--)
        stack_push(s, i);

    // Read from the stack into 'actual'
    for (int i = 0; i < NUM_VALUES; i++)
        stack_pop(s, &actual[i]);
    
    // Verify
    for (int i = 0; i < NUM_VALUES; i++)
        EXPECT_EQ(i, actual[i]);
    
    // Cleanup
    stack_destroy(s);
}
