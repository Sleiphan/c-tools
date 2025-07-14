#include "ctools/stack.h"
#include <stddef.h> // For the definition of NULL
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct stack_node {
    void* value;
    struct stack_node* next;
} stack_node;

void stack_node_init(stack_node* node) {
    node->value = NULL;
    node->next = NULL;
}

stack_node* stack_node_create() {
    stack_node* node = (stack_node*) malloc(sizeof(stack_node));
    stack_node_init(node);
    return node;
}

void stack_node_destroy(stack_node* node) {
    free(node);
}

typedef struct stack {
    stack_node* top;
    int size;
    pthread_mutex_t lock;
    bool shutting_down;
} stack;

stack* stack_create() {
    stack* s = malloc(sizeof(stack));
    if (!s) return NULL;

    if (pthread_mutex_init(&s->lock, NULL)) {
        free(s);
        return NULL;
    }

    s->top = NULL;
    s->size = 0;
    s->shutting_down = false;

    return s;
}

void stack_shutdown(stack* stack) {
    // Lock
    pthread_mutex_lock(&stack->lock);

    stack->shutting_down = true;

    // Unlock
    pthread_mutex_unlock(&stack->lock);
}

void stack_destroy(stack* stack) {
    // Aquire lock
    pthread_mutex_lock(&stack->lock);
    
    // Destroy all nodes in the stack
    while (stack->top != NULL) {
        // Copy the address of the top node
        stack_node* pop_node = stack->top;

        // Pop the top node from the stack
        stack->top = stack->top->next;

        // Destroy the node
        stack_node_destroy(pop_node);
    }

    // Unlock
    pthread_mutex_lock(&stack->lock);

    // Destroy the lock
    pthread_mutex_destroy(&stack->lock);

    // De-allocate the stack struct
    free(stack);
}

int stack_size(stack* stack) {
    pthread_mutex_lock(&stack->lock);

    int size = stack->size;

    pthread_mutex_unlock(&stack->lock);

    return size;
}

int stack_push(stack* stack, void* value) {
    // Create the new node
    stack_node* new_node = stack_node_create();

    // Return if the node could not be allocated
    if (!new_node)
        return 1;

    // Assign the value to the new node
    new_node->value = value;

    // Lock
    pthread_mutex_lock(&stack->lock);

    // Cancel if the stack is shutting down
    if (stack->shutting_down) {
        pthread_mutex_unlock(&stack->lock);
        stack_node_destroy(new_node);
        return 2;
    }
    
    // Set the next node of the new node to the current top
    new_node->next = stack->top;

    // Push the new node to the top of the stack
    stack->top = new_node;

    // Increase the size record
    ++stack->size;

    // Unlock
    pthread_mutex_unlock(&stack->lock);

    return 0;
}

void* stack_pop(stack* stack) {
    // Lock
    pthread_mutex_lock(&stack->lock);

    // If the stack is empty or shutting down, skip
    if (stack->top == NULL || stack->shutting_down) {
        pthread_mutex_unlock(&stack->lock);
        return NULL;
    }

    // Save a reference to the top node
    stack_node* pop_node = stack->top;

    // Decrease the size record
    --stack->size;

    // Remove the top of the stack
    stack->top = stack->top->next;

    // Unlock
    pthread_mutex_unlock(&stack->lock);

    // Copy the top value, to return it at the end
    void* pop_node_value = pop_node->value;

    // Release the top node's allocated memory
    stack_node_destroy(pop_node);

    // Return the value of the removed node
    return pop_node_value;
}



int stack_push_sort(stack* stack, void* value, int (*comp)(const void* lhs, const void* rhs)) {
    // Create the new node
    stack_node* new_node = stack_node_create();

    // Return if the node could not be allocated
    if (!new_node)
        return 1;

    // Assign the value to the new node
    new_node->value = value;
    
    // Lock
    pthread_mutex_lock(&stack->lock);

    // If the stack is shutting down, skip
    if (stack->shutting_down) {
        pthread_mutex_unlock(&stack->lock);
        stack_node_destroy(new_node);
        return 2;
    }

    stack_node** prev = &stack->top;
    stack_node* it = stack->top;

    // Seek to the correct position
    while (it != NULL && comp(it->value, value) < 0) {
        it = it->next;
        prev = &(*prev)->next;
    }

    // Insert the new node between 'prev' and 'it' like this:
    // prev -> new_node -> it.
    new_node->next = it;
    *prev = new_node;

    // Increase the size record
    ++stack->size;

    // Unlock
    pthread_mutex_unlock(&stack->lock);

    return 0;
}