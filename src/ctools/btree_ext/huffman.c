#include "ctools/btree_ext/huffman.h"
#include "ctools/stack.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Files are processed in blocks of 1MB
#define FILE_READING_BUFFER_SIZE 1 << 20
#define SYMBOL_CANDIDATE_COUNT 256

typedef struct huffman_tree {
    btree_node* top;
} huffman_tree;

void count_symbols(FILE* file, uint64_t count[SYMBOL_CANDIDATE_COUNT]) {
    fseek(file, 0, SEEK_SET);
    
    uint8_t buffer[FILE_READING_BUFFER_SIZE];

    size_t bytes_read = 0;

    do {
        bytes_read = fread(buffer, sizeof(buffer[0]), FILE_READING_BUFFER_SIZE, file);

        for (size_t i = 0; i < bytes_read; ++i)
            count[buffer[i]] += 1;

    } while (bytes_read == FILE_READING_BUFFER_SIZE);
}

int huffman_encode(FILE* in_file, FILE* out_file) {
    uint64_t symbol_count[SYMBOL_CANDIDATE_COUNT];
    count_symbols(in_file, symbol_count);

    huffman_tree* htree = huffman_tree_create(symbol_count);
}

int huffman_decode(FILE* in_file, FILE* out_file) {
    
}



#pragma region Huffman Tree

int byte_count_compare_desc(const void* lhs_p, const void* rhs_p) {
    uint64_t lhs = *(*((uint64_t**)lhs_p));
    uint64_t rhs = *(*((uint64_t**)rhs_p));
    if (lhs > rhs) return -1;
    if (lhs < rhs) return  1;
    return 0;
}

int huffman_tree_nodes_comp(const void* lhs_p, const void* rhs_p) {
    uint64_t lhs = *((uint64_t*)((btree_node*)lhs_p)->value);
    uint64_t rhs = *((uint64_t*)((btree_node*)rhs_p)->value);
    if (lhs > rhs) return  1;
    if (lhs < rhs) return -1;
    return 0;
}

int cleanup_huffman_tree_callback(btree_node* node) {
    if (is_leaf_node(node))
        return 0;
    
    free(node->value);
    node->value = NULL;

    return 0;
}



huffman_tree* huffman_tree_create(const uint64_t byte_counts_original[SYMBOL_CANDIDATE_COUNT]) {
    huffman_tree* tree = malloc(sizeof(huffman_tree));
    uint64_t byte_counts[SYMBOL_CANDIDATE_COUNT];
    uint64_t* byte_counts_p[SYMBOL_CANDIDATE_COUNT];
    
    // Copy contents of byte_counts_original to byte_counts
    memcpy(byte_counts, byte_counts_original, SYMBOL_CANDIDATE_COUNT * sizeof(uint64_t));

    // Generate the pointer array byte_counts_p
    for (int i = 0; i < SYMBOL_CANDIDATE_COUNT; i++)
        byte_counts_p[i] = &byte_counts[i];

    // Sort byte count pointers in descending order based on the byte counts.
    qsort(byte_counts_p, SYMBOL_CANDIDATE_COUNT, sizeof(byte_counts_p[0]), byte_count_compare_desc);

    // Define a helper stack for creating the Huffman tree
    stack* node_stack = stack_create();

    // Insert all leaf nodes into the helper stack
    for (int i = 0; i < SYMBOL_CANDIDATE_COUNT & *byte_counts_p[i] > 0; i++) {
        btree_node* node = btree_node_create();
        node->value = byte_counts_p[i];

        stack_push(node_stack, node);
    }

    // Create the Huffman tree
    while (stack_size(node_stack) > 1) {
        // Pop two nodes from the stack
        btree_node* node_1 = stack_pop(node_stack);
        btree_node* node_2 = stack_pop(node_stack);

        uint64_t node_1_count = *((uint64_t*)node_1->value);
        uint64_t node_2_count = *((uint64_t*)node_2->value);

        if (is_leaf_node(node_1)) {
            char byte_value = ((uint64_t*)node_1->value - &byte_counts[0]);
            node_1->value = (char*) malloc(sizeof(char));
            *((char*)node_1->value) = byte_value;
        }

        if (is_leaf_node(node_2)) {
            char byte_value = ((uint64_t*)node_2->value - &byte_counts[0]);
            node_2->value = (char*) malloc(sizeof(char));
            *((char*)node_2->value) = byte_value;
        }

        btree_node* hi = node_1_count >= node_2_count ? node_1 : node_2;
        btree_node* lo = node_1_count <  node_2_count ? node_1 : node_2;

        // Combine the two popped nodes under one full node
        btree_node* new_mid_node = btree_node_create();
        new_mid_node->hi = hi;
        new_mid_node->lo = lo;
        new_mid_node->value = (uint64_t*) malloc(sizeof(uint64_t)); // Only temporary heap allocation. Will be deallocated at the end of this function.
        *((uint64_t*)new_mid_node->value) = node_1_count + node_2_count;

        // Sort the new node into the stack
        stack_push_sort(node_stack, new_mid_node, huffman_tree_nodes_comp);
    }

    // The last node in the stack is the top node of the Huffman tree.
    tree->top = stack_pop(node_stack);

    // Cleanup.
    stack_destroy(node_stack);
    btree_depth_first_search(tree->top, cleanup_huffman_tree_callback); // TODO: change to using BFS.

    return tree;
}

void huffman_tree_destroy_internal(btree_node* node) {
    if (node->lo)
        huffman_tree_destroy_internal(node->lo);
    
    if (node->hi)
        huffman_tree_destroy_internal(node->hi);

    if (node->value)
        free(node->value);

    free(node);
}

void huffman_tree_destroy(huffman_tree* tree) {
    // Recursively destroy all nodes of the tree
    huffman_tree_destroy_internal(tree->top);

    // De-allocate the struct
    free(tree);
}

#pragma endregion
