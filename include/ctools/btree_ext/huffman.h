#ifndef CTOOLS_HUFFMANN_ENCODING
#define CTOOLS_HUFFMANN_ENCODING

#include <stdint.h>
#include <stdio.h>
#include "ctools/btree.h"

typedef struct huffman_tree huffman_tree;

/// @brief Creates a Huffman tree from an array of byte counts.
/// @param top_node A pointer to a 
/// @param byte_counts An array containing the so-called "symbol frequencies" of the compression target.
huffman_tree* huffman_tree_create(const uint64_t byte_counts[1 << 8]);

/// @brief De-allocates a Huffman tree after use.
/// @param htree A pointer to the Huffman tree to deallocate.
void huffman_tree_destroy(huffman_tree* htree);

int huffman_encode(FILE* in_file, FILE* out_file);
int huffman_decode(FILE* in_file, FILE* out_file);

#endif // CTOOLS_HUFFMANN_ENCODING