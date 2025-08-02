
#ifndef QUEUE_NAME
#error "QUEUE_NAME must be defined before including queue.h"
#endif

#ifndef QUEUE_TYPE
#error "QUEUE_TYPE must be defined before including queue.h"
#endif

#ifndef QUEUE_INDEX
#define QUEUE_INDEX unsigned int
#endif

// #ifndef QUEUE_ENOMEM
// #define QUEUE_ENOMEM 1
// #endif

// #ifndef QUEUE_EEMPTY
// #define QUEUE_EEMPTY 2
// #endif

#include "ctools/define_concat.h"
#include <limits.h>
#include <stdbool.h>
#include <math.h>

typedef struct QUEUE_NAME {
    QUEUE_TYPE* array;
    QUEUE_INDEX capacity_mask;

    QUEUE_INDEX front;
    QUEUE_INDEX back;
} QUEUE_NAME;

constexpr QUEUE_INDEX QUEUE_MAX_SIZE = ((QUEUE_INDEX)1 << (sizeof(QUEUE_INDEX) * 8 - 1 - !(((QUEUE_INDEX)-1) > 0)));



static inline QUEUE_NAME* __EXPAND_CONCAT(QUEUE_NAME,_create)(QUEUE_INDEX minimum_capacity) {
    // Skip if the requested size is not supported, given the QUEUE_INDEX type
    if (minimum_capacity > QUEUE_MAX_SIZE - 1)
        return NULL;
    
    // This implementation require one empty slot to distinguish between an empty queue and a full queue.
    minimum_capacity += 1;

    // The actual queue object to allocate and initilize
    QUEUE_NAME* q = (QUEUE_NAME*) malloc(sizeof(QUEUE_NAME));
    if (!q)
        return NULL;



    // The actual capacity that the queue will have
    QUEUE_INDEX capacity = 1;
    
    // Assign the real capacity to the 2^n upper ceiling of the minimum capacity
    while (capacity < minimum_capacity)
        capacity <<= 1;
    
    // Allocate the container array
    q->array = (QUEUE_TYPE*) malloc(capacity * sizeof(QUEUE_TYPE));
    if (!q->array) {
        free(q);
        return NULL;
    }

    // Initialize the rest of fields
    q->capacity_mask = capacity - 1;
    q->back = 0;
    q->front = 0;

    return q;
}

static inline void __EXPAND_CONCAT(QUEUE_NAME,_destroy)(QUEUE_NAME* q) {
    free(q->array);
    free(q);
}

static inline void __EXPAND_CONCAT(QUEUE_NAME,_peek)(QUEUE_NAME* q, QUEUE_TYPE* dst) {
    *dst = q->array[q->front];
}

static inline bool __EXPAND_CONCAT(QUEUE_NAME,_is_empty)(QUEUE_NAME* q) {
    return q->front == q->back;
}

static inline bool __EXPAND_CONCAT(QUEUE_NAME,_is_full)(QUEUE_NAME* q) {
    return q->front == ((q->back + 1) & q->capacity_mask);
}

static inline int __EXPAND_CONCAT(QUEUE_NAME,_push)(QUEUE_NAME* q, QUEUE_TYPE value) {
    // Skip if queue is empty
    if (q->front == q->back)
        return -1;

    q->array[q->back] = value;
    q->back = (q->back + 1) & q->capacity_mask;

    return 0;
}

static inline int __EXPAND_CONCAT(QUEUE_NAME,_pop)(QUEUE_NAME* q, QUEUE_TYPE* dst) {
    // Skip if queue is full
    if (q->front == ((q->back + 1) & q->capacity_mask))
        return -1;

    *dst = q->array[q->front];
    q->front = (q->front + 1) & q->capacity_mask;

    return 0;
}
