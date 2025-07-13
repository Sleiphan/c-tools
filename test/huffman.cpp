#include <gtest/gtest.h>

extern "C" {
    #include "ctools/btree_ext/huffman.h"
}

TEST(huffman, test) {
    uint64_t byte_count[256];
    memset(byte_count, 0, sizeof(byte_count));

    byte_count[(uint8_t)'A'] = 10;
    byte_count[(uint8_t)'B'] = 9;
    byte_count[(uint8_t)'C'] = 4;
    byte_count[(uint8_t)'D'] = 1;

    huffman_tree* htree = huffman_tree_create(byte_count);
    huffman_tree_destroy(htree);

    return;
}