
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

constexpr QUEUE_INDEX __EXPAND_CONCAT(QUEUE_NAME,_max_size) = ((QUEUE_INDEX)1 << (sizeof(QUEUE_INDEX) * 8 - 1 - !(((QUEUE_INDEX)-1) > 0)));



static inline QUEUE_NAME* __EXPAND_CONCAT(QUEUE_NAME,_create)(QUEUE_INDEX minimum_capacity) {
    // Skip if the requested size is not supported, given the QUEUE_INDEX type
    if (minimum_capacity > __EXPAND_CONCAT(QUEUE_NAME,_max_size) - 1)
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

static inline QUEUE_INDEX __EXPAND_CONCAT(QUEUE_NAME,_size)(QUEUE_NAME* q) {
    QUEUE_INDEX back = q->back + (q->capacity_mask + 1) * (q->back < q->front);
    return back - q->front;
}

static inline bool __EXPAND_CONCAT(QUEUE_NAME,_capacity)(QUEUE_NAME* q) {
    return q->capacity_mask + 1;
}

static inline bool __EXPAND_CONCAT(QUEUE_NAME,_is_full)(QUEUE_NAME* q) {
    return q->front == ((q->back + 1) & q->capacity_mask);
}

static inline bool __EXPAND_CONCAT(QUEUE_NAME,_is_empty)(QUEUE_NAME* q) {
    return q->front == q->back;
}



static inline int __EXPAND_CONCAT(QUEUE_NAME,_push)(QUEUE_NAME* q, QUEUE_TYPE value) {
    // Skip if queue is full
    if (q->front == ((q->back + 1) & q->capacity_mask))
        return -1;

    q->array[q->back] = value;
    q->back = (q->back + 1) & q->capacity_mask;

    return 0;
}

static inline int __EXPAND_CONCAT(QUEUE_NAME,_pop)(QUEUE_NAME* q, QUEUE_TYPE* dst) {
    // Skip if queue is empty
    if (q->front == q->back)
        return -1;

    *dst = q->array[q->front];
    q->front = (q->front + 1) & q->capacity_mask;

    return 0;
}



static inline int __EXPAND_CONCAT(QUEUE_NAME,_push_array)(QUEUE_NAME* q, const QUEUE_TYPE* values, const QUEUE_INDEX element_count) {
    // Skip if queue is full
    if (q->front == ((q->back + 1) & q->capacity_mask))
        return 0;

    const bool single_writing_area = q->back < q->front;

    if (single_writing_area) {
        const QUEUE_INDEX space = q->front - q->back - 1;
        const QUEUE_INDEX push_count = element_count < space ? element_count : space;
        memcpy(&q->array[q->back], values, push_count * sizeof(QUEUE_TYPE));
        q->back += push_count;
        return push_count;
    } else {
        const QUEUE_INDEX space_section_3 = q->capacity_mask + 1 - q->back;
        
        QUEUE_INDEX total_space_left = q->capacity_mask - __EXPAND_CONCAT(QUEUE_NAME,_size)(q);
        QUEUE_INDEX elements_left = element_count;

        const QUEUE_INDEX push_count_1 = space_section_3 < element_count ? space_section_3 : element_count;
        elements_left -= push_count_1;
        total_space_left -= push_count_1;

        const QUEUE_INDEX push_count_2 = total_space_left < elements_left ? total_space_left : elements_left;

        memcpy(&q->array[q->back], values, push_count_1 * sizeof(QUEUE_TYPE));

        if (push_count_2 > 0)
            memcpy(q->array, &values[push_count_1], push_count_2 * sizeof(QUEUE_TYPE));
        
        q->back = (q->back + push_count_1 + push_count_2) & q->capacity_mask;
        
        return push_count_1 + push_count_2;
    }
}



static inline int __EXPAND_CONCAT(QUEUE_NAME,_pop_array)(QUEUE_NAME* q, QUEUE_TYPE* dst, const QUEUE_INDEX dst_size) {
    // Skip if queue is empty
    if (q->front == q->back)
        return 0;

    const bool single_reading_area = q->front < q->back;

    if (single_reading_area) {
        const QUEUE_INDEX size = q->back - q->front;
        const QUEUE_INDEX read_count = dst_size < size ? dst_size : size;
        memcpy(dst, &q->array[q->front], read_count * sizeof(QUEUE_TYPE));
        q->front += read_count;
        return read_count;
    } else {
        const QUEUE_INDEX elements_section_3 = q->capacity_mask + 1 - q->front;
        
        QUEUE_INDEX queued_elements_left = __EXPAND_CONCAT(QUEUE_NAME,_size)(q);
        QUEUE_INDEX dst_space_left = dst_size;

        const QUEUE_INDEX read_count_1 = elements_section_3 < dst_size ? elements_section_3 : dst_size;
        dst_space_left -= read_count_1;
        queued_elements_left -= read_count_1;

        const QUEUE_INDEX read_count_2 = queued_elements_left < dst_space_left ? queued_elements_left : dst_space_left;

        memcpy(dst, &q->array[q->front], read_count_1 * sizeof(QUEUE_TYPE));

        if (read_count_2 > 0)
            memcpy(&dst[read_count_1], q->array, read_count_2 * sizeof(QUEUE_TYPE));
        
        q->front = (q->front + read_count_1 + read_count_2) & q->capacity_mask;
        
        return read_count_1 + read_count_2;
    }
}