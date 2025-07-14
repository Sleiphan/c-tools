#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include "ctools/queue.h"



typedef struct queue_node {
    void* value;
    struct queue_node* next;
} queue_node;

queue_node* queue_node_create() {
    queue_node* node = malloc(sizeof(queue_node));
    if (!node)
        return NULL;

    node->next = NULL;
    node->value = NULL;

    return node;
}

void queue_node_destroy(queue_node* node) {
    free(node);
}



typedef struct queue {
    queue_node* front;
    queue_node* back;

    pthread_mutex_t lock;
    pthread_cond_t pop_cond;

    bool shutting_down;
} queue;



queue* queue_create() {
    queue* q = (queue*) malloc(sizeof(queue));
    if (!q) return NULL;

    queue_node* initial_node = queue_node_create();
    if (!initial_node) {
        free(q);
        return NULL;
    }

    if (pthread_mutex_init(&q->lock, NULL)) {
        queue_node_destroy(initial_node);
        free(q);
        return NULL;
    }

    if (pthread_cond_init(&q->pop_cond, NULL)) {
        pthread_mutex_destroy(&q->lock);
        queue_node_destroy(initial_node);
        free(q);
        return NULL;
    }

    q->back = q->front = initial_node;
    q->shutting_down = false;

    return q;
}

void queue_shutdown(queue* queue) {
    pthread_mutex_lock(&queue->lock);
    queue->shutting_down = true;
    pthread_cond_broadcast(&queue->pop_cond);
    pthread_mutex_unlock(&queue->lock);
}

void queue_destroy(queue* queue) {
    queue_node* node = queue->front;

    while (node) {
        queue_node* next = node->next;
        queue_node_destroy(node);
        node = next;
    }

    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->pop_cond);

    free(queue);
}

int queue_empty(queue* queue) {
    return queue->front == queue->back;
}



int queue_push(queue* queue, void* value) {
    // Allocate a new node
    queue_node* new_node = queue_node_create();
    if (!new_node)
        return 1;

    // Only one thread may push at once
    pthread_mutex_lock(&queue->lock);

    if (queue->shutting_down) {
        pthread_mutex_unlock(&queue->lock);
        queue_node_destroy(new_node);
        return 2;
    }

    // Assign the value to the node currently at the back.
    // The reason for doing this rather than assigning it to the new node,
    // is because of the fact that a queue always has at least one node.
    queue->back->value = value;

    // Make the node at the back of the queue point to the new node
    queue->back->next = new_node;

    // Place the new node at the back of the queue
    queue->back = new_node;

    // Signal one thread that is waiting to pop a node
    pthread_cond_signal(&queue->pop_cond);
    
    // Unlock
    pthread_mutex_unlock(&queue->lock);

    return 0;
}



void* queue_pop_common(queue* queue) {
    // Save a pointer to the node at the front of the queue
    queue_node* pop_node = queue->front;

    // Remove the node at the front of the queue
    queue->front = queue->front->next;

    // When the node has been safely removed from the queue, other threads can start popping
    pthread_mutex_unlock(&queue->lock);

    // Copy the value, to return it later
    void* value = pop_node->value;

    // De-allocate the node that was removed from the front of the queue
    queue_node_destroy(pop_node);

    // Return the value
    return value;
}

void* queue_pop(queue* queue) {
    // Only one thread may pop at once
    pthread_mutex_lock(&queue->lock);
    
    if (queue_empty(queue)) {
        pthread_mutex_unlock(&queue->lock);
        return NULL;
    }

    return queue_pop_common(queue);
}

void* queue_pop_wait(queue* queue) {
    // Only one thread may pop at once
    pthread_mutex_lock(&queue->lock);

    // Wait until there are elements available
    while (queue_empty(queue) && !queue->shutting_down)
        pthread_cond_wait(&queue->pop_cond, &queue->lock);
    
    if (queue_empty(queue)) {
        pthread_mutex_unlock(&queue->lock);
        return NULL;
    }
    
    return queue_pop_common(queue);
}