#include "ctools/heap.h"
#include "ctools/stack.h"
#include <stdlib.h>
#include <string.h>

void memswp(void* a, void* b, int size) {
    void* temp = malloc(size);
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

void heap_build(void* heap_array, const size_t heap_array_size, const int element_size, int (*comp)(const void* lhs, const void* rhs)) {
    const size_t first_parent = heap_array_size / 2 - 1;

    // Use a FILO processing queue
    stack* nodes = stack_create();
    
    // Queue all parent nodes for processing in the correct order
    for (size_t i = 0; i <= first_parent; i++) {
        size_t* index = malloc(sizeof(size_t));
        *index = i;
        stack_push(nodes, index);
    }

    // While there are nodes left to process
    while (stack_size(nodes)) {
        
        // Fetch the next node to process
        size_t* parent_p = stack_pop(nodes);
        const size_t parent = *parent_p;
        free(parent_p);

        // Find the index of both child nodes
        const size_t left_child = (parent + 1) * 2 - 1;
        const size_t right_child = (parent + 1) * 2;
        
        // Skip if the node has no children
        if (left_child >= heap_array_size)
            continue;

        // Find largest child
        size_t largest_child = left_child;
        if (right_child < heap_array_size)
            if (comp(heap_array + left_child * element_size, heap_array + right_child * element_size) > 0)
                largest_child = right_child;
        
        // Swap if necessary
        if (comp(heap_array + largest_child * element_size, heap_array + parent * element_size) < 0) {
            memswp(heap_array + largest_child * element_size, heap_array + parent * element_size, element_size);
            
            // Queue the node's children for processing
            size_t* left_child_p = malloc(sizeof(size_t));
            size_t* right_child_p = malloc(sizeof(size_t));
            *left_child_p = left_child;
            *right_child_p = right_child;
            stack_push(nodes, left_child_p);
            stack_push(nodes, right_child_p);
        }
    }
}

int heap_verify(const void* heap_array, const size_t heap_array_size, const int element_size, int (*comp)(const void* lhs, const void* rhs)) {
    int violations = 0;
    size_t parent = heap_array_size / 2 - 1;

    // Take care of the edge case where the last sub-tree only has one child.
    if ((heap_array_size & 1) == 0) {
        const size_t left_child = (parent + 1) * 2 - 1;
        violations += comp(heap_array + left_child * element_size, heap_array + parent * element_size) < 0;
        parent--;
    }
    
    for (; parent < heap_array_size; parent--) {
        const size_t left_child = (parent + 1) * 2 - 1;
        const size_t right_child = (parent + 1) * 2;
        const size_t largest_child = comp(heap_array + left_child * element_size, heap_array + right_child * element_size) < 0 ? left_child : right_child;
        violations += comp(heap_array + largest_child * element_size, heap_array + parent * element_size) < 0;
    }

    return violations;
}