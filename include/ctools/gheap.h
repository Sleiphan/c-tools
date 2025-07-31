#include <stdlib.h>

#ifndef HEAP_TYPE
#error "HEAP_TYPE must be defined before including gheap.h"
#endif
#ifndef HEAP_NAME
#error "HEAP_NAME must be defined before including gheap.h"
#endif
#ifndef HEAP_COMP
#error "HEAP_COMP must be defined before including gheap.h"
#endif
#ifndef HEAP_INDEX
#define HEAP_INDEX unsigned int
#endif
#ifndef HEAP_SWAP
#define HEAP_SWAP(LHS, RHS) {\
    HEAP_TYPE temp = LHS;\
    LHS = RHS;\
    RHS = temp;\
}
#endif

#define CONCAT(a,b) a##b
#define EXPAND_CONCAT(a,b) CONCAT(a,b)



typedef struct HEAP_NAME HEAP_NAME;

/**
 * @brief Allocates and initializes a new heap.
 * @param initial_capacity The initial capacity of the internal storage array that contains the nodes of the heap. Must be greater than 0.
 * @return A pointer to the newly allocated and initialized heap object.
 */
HEAP_NAME* EXPAND_CONCAT(HEAP_NAME,_create)(const HEAP_INDEX initial_capacity);

/**
 * @brief De-allocates a heap.
 * @param h A pointer to a heap allocated by the _create() function.
 */
void EXPAND_CONCAT(HEAP_NAME,_destroy)(HEAP_NAME* h);

HEAP_INDEX EXPAND_CONCAT(HEAP_NAME,_size)(HEAP_NAME* h);
HEAP_TYPE EXPAND_CONCAT(HEAP_NAME,_peek)(HEAP_NAME* h);

void EXPAND_CONCAT(HEAP_NAME,_push)(HEAP_NAME* h, HEAP_TYPE value);
HEAP_TYPE EXPAND_CONCAT(HEAP_NAME,_pop)(HEAP_NAME* h);



typedef struct HEAP_NAME {
    HEAP_INDEX size;
    HEAP_INDEX capacity;
    HEAP_TYPE* array;
} HEAP_NAME;

HEAP_INDEX EXPAND_CONCAT(HEAP_NAME,_size)(HEAP_NAME* h) {
    return h->size;
}

HEAP_TYPE EXPAND_CONCAT(HEAP_NAME,_peek)(HEAP_NAME* h) {
    return h->array[0];
}

HEAP_NAME* EXPAND_CONCAT(HEAP_NAME,_create)(const HEAP_INDEX initial_capacity) {
    // Initial capacity must be greater than 0
    if (initial_capacity < 1)
        return (HEAP_NAME*) NULL;

    // Allocate a new heap struct
    HEAP_NAME* h = (HEAP_NAME*) malloc(sizeof(HEAP_NAME));

    // Assign default values
    h->size = 0;
    h->capacity = initial_capacity;
    
    // Allocate the array containing the heap nodes
    h->array = (HEAP_TYPE*) malloc(sizeof(HEAP_TYPE) * initial_capacity);

    // Return the new heap
    return h;
}

void EXPAND_CONCAT(HEAP_NAME,_destroy)(HEAP_NAME* h) {
    free(h->array);
    free(h);
}

void EXPAND_CONCAT(HEAP_NAME,_push)(HEAP_NAME* h, HEAP_TYPE value) {

    // If the storage array is full, double the capacity of the storage array
    if (h->size == h->capacity) {
        h->array = (HEAP_TYPE*) realloc(h->array, sizeof(HEAP_TYPE) * h->capacity * 2);
        h->capacity *= 2;
    }

    // The index of the last node in the storage array
    const HEAP_INDEX last_node = h->size;

    // Increase element count
    h->size++;

    // Assign the new value to the end of the heap
    h->array[last_node] = value;

    // Define the parent/child pair, with the new node as the child
    HEAP_INDEX parent_node = (last_node - 1) / 2;
    HEAP_INDEX child_node = last_node;

    // Move the child node upwards until the value of the parent node is larger than or equal to the value of the child node
    while (child_node > 0 && HEAP_COMP(h->array[child_node], h->array[parent_node]) < 0) {

        // Swap parent and child
        HEAP_SWAP(h->array[child_node], h->array[parent_node])

        // Iterate to the above parent/child pair
        child_node = parent_node;
        parent_node = (parent_node - 1) / 2;
    }
}

HEAP_TYPE EXPAND_CONCAT(HEAP_NAME,_pop)(HEAP_NAME* h) {

    // Skip if the heap is empty
    HEAP_TYPE return_value = h->array[0];

    // Store the root node to return it later
    h->array[0] = h->array[h->size - 1];

    // Overwrite the root node with the last element in the heap
    h->size--;

    // Reallocate if threshold reached
    if (h->size <= h->capacity / 4) {
        h->array = (HEAP_TYPE*) realloc(h->array, sizeof(HEAP_TYPE) * h->capacity / 2);
        h->capacity /= 2;
    }



    HEAP_INDEX parent = 0;

    // While there are nodes left to process
    while (1) {

        // Find the index of both child nodes
        const HEAP_INDEX left_child = (parent + 1) * 2 - 1;
        const HEAP_INDEX right_child = (parent + 1) * 2;

        // Calculate 
        const int parent_has_children = left_child >= h->size;
        const int parent_has_two_children = right_child < h->size;

        // Cancel if the node has no children
        if (parent_has_children)
            break;

        // Find the largest child
        HEAP_INDEX largest_child = left_child;
        if (parent_has_two_children) {
            const int right_child_is_largest = HEAP_COMP(h->array[left_child], h->array[right_child]) > 0;

            largest_child += right_child_is_largest;
        }

        // Swap if necessary
        if (HEAP_COMP(h->array[largest_child], h->array[parent]) < 0)
            HEAP_SWAP(h->array[largest_child], h->array[parent])
        else
            break;

        // Iterate down to the largest child
        parent = largest_child;
    }

    // Return the node that was removed from the heap
    return return_value;
}

#undef HEAP_NAME
#undef HEAP_TYPE
#undef HEAP_COMP
#undef HEAP_INDEX
#undef HEAP_SWAP