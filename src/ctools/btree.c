#include "ctools/btree.h"
#include "ctools/stack.h"
#include "ctools/queue.h"
#include <stddef.h> // For the definition of NULL
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

btree_node* btree_node_create() {
    btree_node* node = malloc(sizeof(btree_node));
    
    if (node == NULL)
        return NULL;

    btree_node_init(node);

    return node;
}

void btree_node_init(btree_node* node) {
    node->hi = NULL;
    node->lo = NULL;
    node->value = NULL;
}

void btree_node_destroy(btree_node* node) {
    free(node);
}

void btree_node_destroy_recursive(btree_node* node) {
    if (node->lo) btree_node_destroy_recursive(node->lo);
    if (node->hi) btree_node_destroy_recursive(node->hi);

    node->lo = NULL;
    node->hi = NULL;

    btree_node_destroy(node);
}

int is_leaf_node(const btree_node* node) {
    return node->hi == NULL & node->lo == NULL;
}



int btree_depth_first_search(btree_node* node, void* optional, int (*callback)(btree_node* node, void* optional)) {
    int result = 0;

    result = callback(node, optional);

    if (result)
        return result;

    if (node->lo)
        result = btree_depth_first_search(node->lo, optional, callback);

    if (result)
        return result;

    if (node->hi)
        result = btree_depth_first_search(node->hi, optional, callback);

    return result;
}

int btree_breadth_first_search(btree_node* top, void* optional, int (*callback)(btree_node* node, void* optional)) {
    queue* nodes = queue_create();

    // Enqueue the top node
    queue_push(nodes, top);

    // The value returned from the callback
    int return_code = 0;

    // While there are nodes left in the queue,
    // and the return code indicates that the search should continue...
    while (!queue_empty(nodes) & !return_code) {
        // Pop one node
        btree_node* node = (btree_node*) queue_pop(nodes);

        // Enqueue child nodes
        if (node->lo) queue_push(nodes, node->lo);
        if (node->hi) queue_push(nodes, node->hi);

        // Run the callback on the node
        return_code = callback(node, optional);
    }

    queue_destroy(nodes);

    return return_code;
}
