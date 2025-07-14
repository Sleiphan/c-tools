#ifndef CTOOLS_QUEUE
#define CTOOLS_QUEUE

typedef struct queue queue;

queue* queue_create();
void queue_shutdown(queue*);
void queue_destroy(queue*);

/// @brief Checks whether a queue is empty. The complexity of this operation is O(1).
/// @param The queue to check for emptiness.
/// @return A non-zero value if the queue is empty. Otherwise, returns 0.
int queue_empty(queue* queue);

/// @brief Adds a value to the back of a queue.
/// @param queue The queue that the value should be pushed to.
/// @param value The value to push to the queue.
/// @return 0 if the operation was successful,
/// 1 if a new node could not be allocated,
/// and 2 if the queue is shutting down.
int queue_push(queue* queue, void* value);

void* queue_pop(queue*);
void* queue_pop_wait(queue*);

#endif // CTOOLS_QUEUE