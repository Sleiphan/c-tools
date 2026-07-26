#ifndef QUEUE_NAME
#error "QUEUE_NAME must be defined before including kqueue.h"
#endif

#ifndef QUEUE_TYPE
#error "QUEUE_TYPE must be defined before including kqueue.h"
#endif

#ifndef QUEUE_INDEX
#define QUEUE_INDEX unsigned int
#endif

#ifndef QUEUE_SUBQUEUE_INDEX
#define QUEUE_SUBQUEUE_INDEX unsigned char
#endif

#include <stdbool.h>
#include <stdlib.h> // Only for malloc()
#include <errno.h>

#include "ctools/define_concat.h"

static const QUEUE_INDEX __EXPAND_CONCAT(QUEUE_NAME,_max_size) = ((QUEUE_INDEX)-1) ^ ((((QUEUE_INDEX)-1) < 0) << (sizeof(QUEUE_INDEX) * 8 - 1));

struct __EXPAND_CONCAT(QUEUE_NAME,_subqueue) {
    QUEUE_INDEX head;
    QUEUE_INDEX tail;
};

struct QUEUE_NAME {
    QUEUE_TYPE* array;
    QUEUE_INDEX* nexts;
    QUEUE_INDEX capacity;

    QUEUE_INDEX* free_stack;
    QUEUE_INDEX free_stack_head;

    struct __EXPAND_CONCAT(QUEUE_NAME,_subqueue)* queues;
    QUEUE_SUBQUEUE_INDEX queue_count;
};

static int __EXPAND_CONCAT(QUEUE_NAME,_create)(struct QUEUE_NAME* queue_dst, const QUEUE_INDEX capacity, const QUEUE_SUBQUEUE_INDEX queue_count) {
    // Allocate the container array
    QUEUE_TYPE* array = (QUEUE_TYPE*) malloc(capacity * sizeof(QUEUE_TYPE));
    if (!array)
        return -1;

    // Allocate the queues array
    struct __EXPAND_CONCAT(QUEUE_NAME,_subqueue)* queues = (struct __EXPAND_CONCAT(QUEUE_NAME,_subqueue)*) malloc(queue_count * sizeof(struct __EXPAND_CONCAT(QUEUE_NAME,_subqueue)));
    if (!queues) {
        free(array);
        return -1;
    }

    // Allocate nexts
    QUEUE_INDEX* nexts = (QUEUE_INDEX*) malloc(capacity * sizeof(QUEUE_INDEX));
    if (!nexts) {
        free(queues);
        free(array);
        return -1;
    }

    // Allocate the free stack
    QUEUE_INDEX* free_stack = (QUEUE_INDEX*) malloc(capacity * sizeof(QUEUE_INDEX));
    if (!free_stack) {
        free(queues);
        free(array);
        free(nexts);
        return -1;
    }

    // Initialize the nexts array
    for (QUEUE_INDEX i = 0; i < capacity; i++)
        nexts[i] = __EXPAND_CONCAT(QUEUE_NAME,_max_size);

    // Initialize the nexts stack
    for (QUEUE_INDEX i = 0; i < capacity; i++)
        free_stack[i] = i;

    // Initialize all queues
    for (QUEUE_SUBQUEUE_INDEX i = 0; i < queue_count; i++)
        queues[i] = (struct __EXPAND_CONCAT(QUEUE_NAME,_subqueue)) {
            .head = __EXPAND_CONCAT(QUEUE_NAME,_max_size),
            .tail = __EXPAND_CONCAT(QUEUE_NAME,_max_size),
        };

    // Create and return the actual object
    *queue_dst = (struct QUEUE_NAME) {
        .array = array,
        .nexts = nexts,
        .capacity = capacity,
        .free_stack = free_stack,
        .free_stack_head = 0,
        .queues = queues,
        .queue_count = queue_count,
    };

    return 0;
}

static inline void __EXPAND_CONCAT(QUEUE_NAME,_destroy)(struct QUEUE_NAME* q) {
    free(q->array);
    free(q->free_stack);
    free(q->nexts);
    free(q->queues);
}

static inline QUEUE_INDEX __EXPAND_CONCAT(QUEUE_NAME,_size)(struct QUEUE_NAME* q) {
    return q->free_stack_head;
}

static inline QUEUE_INDEX __EXPAND_CONCAT(QUEUE_NAME,_capacity)(struct QUEUE_NAME* q) {
    return q->capacity;
}

static inline bool __EXPAND_CONCAT(QUEUE_NAME,_is_full)(struct QUEUE_NAME* q) {
    return q->free_stack_head >= q->capacity;
}

static inline bool __EXPAND_CONCAT(QUEUE_NAME,_is_empty)(struct QUEUE_NAME* q) {
    return !q->free_stack_head;
}

static inline int __EXPAND_CONCAT(QUEUE_NAME,_peek)(struct QUEUE_NAME* q, QUEUE_SUBQUEUE_INDEX queue_idx, QUEUE_TYPE* dst) {
    // Skip if queue is empty
    if (__EXPAND_CONCAT(QUEUE_NAME,_is_empty(q))) {
        errno = ENOENT;
        return -1;
    }

    *dst = q->array[q->queues[queue_idx].head];
    return 0;
}

static int __EXPAND_CONCAT(QUEUE_NAME,_push)(struct QUEUE_NAME* q, const QUEUE_SUBQUEUE_INDEX queue_idx, const QUEUE_TYPE value) {
    // Skip if queue is full
    if (__EXPAND_CONCAT(QUEUE_NAME,_is_full(q))) {
        errno = ENOBUFS;
        return -1;
    }

    // Error if the queue_idx is invalid
    if (queue_idx >= q->queue_count) {
        errno = EINVAL;
        return -1;
    }

    // Pop new entry from the free stack
    const QUEUE_INDEX new_entry_idx = q->free_stack[q->free_stack_head++];

    // Assign the submitted value to the new element
    q->array[new_entry_idx] = value;

    int value_set = q->array[new_entry_idx];

    // Set the next-value of the tail entry to point to the new entry
    if (q->queues[queue_idx].tail != __EXPAND_CONCAT(QUEUE_NAME,_max_size))
        q->nexts[q->queues[queue_idx].tail] = new_entry_idx;

    // Update the tail
    q->queues[queue_idx].tail = new_entry_idx;

    // If this is the first entry in this queue ...
    if (q->queues[queue_idx].head == __EXPAND_CONCAT(QUEUE_NAME,_max_size))
        q->queues[queue_idx].head = new_entry_idx; // ... set the head to the new entry

    return 0;
}

static int __EXPAND_CONCAT(QUEUE_NAME,_pop)(struct QUEUE_NAME* q, const QUEUE_SUBQUEUE_INDEX queue_idx, QUEUE_TYPE* dst) {
    // Skip if queue is empty
    if (__EXPAND_CONCAT(QUEUE_NAME,_is_empty(q))) {
        errno = ENOENT;
        return -1;
    }

    const QUEUE_INDEX head = q->queues[queue_idx].head;

    // Return the value to the caller
    *dst = q->array[head];

    // Return the entry at the head back to the stack
    q->free_stack[--q->free_stack_head] = head;

    // If this is the last element in the given queue ...
    if (head == q->queues[queue_idx].tail) {
        // ... reset the given queue
        q->queues[queue_idx] = (struct __EXPAND_CONCAT(QUEUE_NAME,_subqueue)) {
            .head = __EXPAND_CONCAT(QUEUE_NAME,_max_size),
            .tail = __EXPAND_CONCAT(QUEUE_NAME,_max_size),
        };
    } else {
        // Else, forward the head
        q->queues[queue_idx].head = q->nexts[head];
    }

    return 0;
}
