#include <gtest/gtest.h>

extern "C" {
    #include "ctools/btree.h"
}

int BFS_base_case_callback(btree_node* node, void* optional) {
    int* i = (int*) optional;
    int node_value = *((int*)node->value);
    int i_value = *i;

    EXPECT_EQ(node_value, i_value);
    (*i)++;

    return 0;
}

TEST(btree, BFS_base_case) {
    int numbers[] = { 0, 1, 2, 3, 4, 5, 6 };

    btree_node* top = btree_node_create();
    top->lo = btree_node_create();
    top->lo->lo = btree_node_create();
    top->lo->hi = btree_node_create();
    top->hi = btree_node_create();
    top->hi->lo = btree_node_create();
    top->hi->hi = btree_node_create();

    top->value = &numbers[0];
    top->lo->value = &numbers[1];
    top->lo->lo->value = &numbers[3];
    top->lo->hi->value = &numbers[4];
    top->hi->value = &numbers[2];
    top->hi->lo->value = &numbers[5];
    top->hi->hi->value = &numbers[6];

    int i = 0;

    btree_breadth_first_search(top, &i, BFS_base_case_callback);

    btree_node_destroy_recursive(top);
}

TEST(btree, DFS_base_case) {
    int numbers[] = { 0, 1, 2, 3, 4, 5, 6 };

    btree_node* top = btree_node_create();
    top->lo = btree_node_create();
    top->lo->lo = btree_node_create();
    top->lo->hi = btree_node_create();
    top->hi = btree_node_create();
    top->hi->lo = btree_node_create();
    top->hi->hi = btree_node_create();

    top->value = &numbers[0];
    top->lo->value = &numbers[1];
    top->lo->lo->value = &numbers[2];
    top->lo->hi->value = &numbers[3];
    top->hi->value = &numbers[4];
    top->hi->lo->value = &numbers[5];
    top->hi->hi->value = &numbers[6];

    int i = 0;

    btree_depth_first_search(top, &i, BFS_base_case_callback);

    btree_node_destroy_recursive(top);
}