#ifndef STACK_TYPE
#error "STACK_TYPE must be defined before including stack.h"
#endif
#ifndef STACK_NAME
#error "STACK_NAME must be defined before including stack.h"
#endif
#ifndef STACK_INDEX
#define STACK_INDEX unsigned int
#endif

#ifndef STACK_MIN_CAPACITY
#define STACK_MIN_CAPACITY 8
#endif


#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include "ctools/define_concat.h"



// typedef struct STACK_NAME STACK_NAME;

// STACK_NAME* __EXPAND_CONCAT(STACK_NAME,_create)(const STACK_INDEX initial_capacity);
// STACK_INDEX __EXPAND_CONCAT(STACK_NAME,_size)(STACK_NAME* s);
// int __EXPAND_CONCAT(STACK_NAME,_push)(STACK_NAME* s, STACK_TYPE value);
// int __EXPAND_CONCAT(STACK_NAME,_pop)(STACK_NAME* s, STACK_TYPE* dst);
// void __EXPAND_CONCAT(STACK_NAME,_shutdown)(STACK_NAME* s);
// void __EXPAND_CONCAT(STACK_NAME,_destroy)(STACK_NAME* s);



typedef struct STACK_NAME {
    STACK_TYPE* array;
    STACK_INDEX size;
    STACK_INDEX capacity;

    pthread_mutex_t lock;
    bool shutting_down;
} STACK_NAME;

static inline STACK_NAME* __EXPAND_CONCAT(STACK_NAME,_create)(const STACK_INDEX initial_capacity) {
    STACK_NAME* s = (STACK_NAME*) malloc(sizeof(STACK_NAME));
    if (!s)
        return NULL;

    if (pthread_mutex_init(&s->lock, NULL)) {
        free(s);
        return NULL;
    }

    // Choose the largest of `initial_capacity` and `STACK_MIN_CAPACITY` as the starting capacity.
    const STACK_INDEX starting_capacity = initial_capacity > STACK_MIN_CAPACITY ? initial_capacity : STACK_MIN_CAPACITY;

    s->array = (STACK_TYPE*) malloc(starting_capacity * sizeof(STACK_TYPE));
    if (!s->array) {
        pthread_mutex_destroy(&s->lock);
        free(s);
        return NULL;
    }

    s->capacity = starting_capacity;
    s->size = 0;
    s->shutting_down = false;

    return s;
}

static inline void __EXPAND_CONCAT(STACK_NAME,_shutdown)(STACK_NAME* s) {
    // Lock
    pthread_mutex_lock(&s->lock);

    s->shutting_down = true;

    // Unlock
    pthread_mutex_unlock(&s->lock);
}

static inline void __EXPAND_CONCAT(STACK_NAME,_destroy)(STACK_NAME* s) {
    // Aquire lock
    pthread_mutex_lock(&s->lock);
    
    // Destroy all nodes in the stack
    free(s->array);

    // Unlock
    pthread_mutex_unlock(&s->lock);

    // Destroy the lock
    pthread_mutex_destroy(&s->lock);

    // De-allocate the stack struct
    free(s);
}

static inline STACK_INDEX __EXPAND_CONCAT(STACK_NAME,_size)(STACK_NAME* s) {
    pthread_mutex_lock(&s->lock);

    STACK_INDEX size = s->size;

    pthread_mutex_unlock(&s->lock);

    return size;
}

static inline int __EXPAND_CONCAT(STACK_NAME,_push)(STACK_NAME* s, STACK_TYPE value) {
    // Lock
    pthread_mutex_lock(&s->lock);

    // Cancel if the stack is shutting down
    if (s->shutting_down) {
        pthread_mutex_unlock(&s->lock);
        return -2;
    }

    // Increase capacity if the stack is full
    if (s->size == s->capacity) {
        STACK_TYPE* new_array = (STACK_TYPE*) realloc(s->array, 2 * s->capacity * sizeof(STACK_TYPE));
        
        // If we failed to allocate more memory, return an error
        if (!new_array) {
            pthread_mutex_unlock(&s->lock);
            return -3;
        }
        
        // Continue with the reallocated array
        s->array = new_array;
        s->capacity *= 2;
    }

    // Assign the value to the top of the stack,
    // then increase the size counter
    s->array[s->size++] = value;

    // Unlock
    pthread_mutex_unlock(&s->lock);

    return 0;
}

static inline int __EXPAND_CONCAT(STACK_NAME,_pop) (STACK_NAME* s, STACK_TYPE* dst) {
    // Lock
    pthread_mutex_lock(&s->lock);

    // If the stack is shutting down, skip
    if (s->shutting_down) {
        pthread_mutex_unlock(&s->lock);
        return -2;
    }

    // Return error code if the stack is empty
    if (s->size == 0) {
        pthread_mutex_unlock(&s->lock);
        return -1;
    }

    // Decrease size counter by one,
    // then assign the removed value to the destination pointer
    *dst = s->array[--s->size];

    // Decrease capacity if the stack size is a quarter of the capacity
    if ((s->size <= s->capacity / 4) && (s->capacity / 2 >= STACK_MIN_CAPACITY)) {
        STACK_TYPE* new_array = (STACK_TYPE*) realloc(s->array, (s->capacity / 2) * sizeof(STACK_TYPE));
        
        // If we failed to allocate more memory, return an error
        if (!new_array) {
            pthread_mutex_unlock(&s->lock);
            return -3;
        }
        
        s->array = new_array;
        s->capacity /= 2;
    }

    // Unlock
    pthread_mutex_unlock(&s->lock);

    // Return success
    return 0;
}
