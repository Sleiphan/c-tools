#include "ctools/stack.h"
#include <stddef.h> // For the definition of NULL
#include <stdlib.h>

typedef struct stack_node {
    void* value;
    struct stack_node* next;
} stack_node;

typedef struct stack {
    stack_node* top;
    int size;
} stack;

stack* stack_create() {
    stack* s = malloc(sizeof(stack));
    s->top = NULL;
    s->size = 0;
}

void stack_destroy(stack* stack) {
    while (stack->top != NULL) {
        stack_node* pop_node = stack->top;
        stack->top = stack->top->next;
        free(pop_node);
    }
}

int stack_size(const stack* stack) {
    return stack->size;
}

void stack_push(stack* stack, void* value) {
    // Create the new node
    stack_node* new_node = (stack_node*) malloc(sizeof(stack_node));
    new_node->value = value;
    new_node->next = stack->top;

    // Push the new node to the top of the stack
    stack->top = new_node;

    // Increase the size record
    ++stack->size;
}

void* stack_pop(stack* stack) {
    // If the stack is empty, skip
    if (stack->top == NULL)
        return NULL;

    // Save a reference to the top node
    stack_node* pop_node = stack->top;

    // Copy the top value, to return it at the end
    void* pop_node_value = stack->top->value;

    // Decrease the size record
    --stack->size;

    // Remove the top of the stack
    stack->top = stack->top->next;

    // Release the top node's allocated memory
    free(pop_node);

    // Return the value of the removed node
    return pop_node_value;
}



void stack_push_sort(stack* stack, void* value, int (*comp)(const void* lhs, const void* rhs)) {
    // Handle the edge-case where the new value should be placed at the top of the stack
    if (stack->top == NULL || comp(stack->top->value, value) >= 0) {
        stack_push(stack, value);
        return;
    }

    stack_node* prev = stack->top;
    stack_node* it = stack->top->next;

    // Seek to the correct position
    // while (it->value < value)
    while (it != NULL && comp(it->value, value) < 0) {
        prev = it;
        it = it->next;
    }

    // Create the new node
    stack_node* new_node = (stack_node*) malloc(sizeof(stack_node));
    new_node->value = value;

    // Insert the new node between 'prev' and 'it' like this:
    // prev -> new_node -> it.
    new_node->next = it;
    prev->next = new_node;

    // Increase the size record
    ++stack->size;
}