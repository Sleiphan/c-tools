#include "ctools/btree.h"
#include "ctools/stack.h"
#include <stddef.h> // For the definition of NULL
#include <stdint.h>
#include <stdlib.h>

void btree_node_create(btree_node* node) {
    node->hi = NULL;
    node->lo = NULL;
    node->value = NULL;
}

int is_leaf_node(const btree_node* node) {
    return node->hi == NULL & node->lo == NULL;
}



int btree_depth_first_search(btree_node* node, int (*callback)(btree_node* node)) {
    int result = 0;

    result = callback(node);

    if (result)
        return result;

    result = btree_depth_first_search(node->lo, callback);

    if (result)
        return result;

    result = btree_depth_first_search(node->hi, callback);

    return result;
}

int btree_breadth_first_search(btree_node* node, int (*callback)(btree_node* node)) {
    return *((int*)0); // Not implemented yet; needs a queue. Cause segfault to discourage use.
}
