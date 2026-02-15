#include "ctools/trie/rtree.h"
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    struct trie_node* trie = trie_create();

    struct str_dict {
        const char* str;
        uint16_t value;
    };

    struct str_dict entries[] = {
        (struct str_dict) {.str="Hello World!", .value=0},
        (struct str_dict) {.str="Health",       .value=0},
        (struct str_dict) {.str="Fire",         .value=0},
        (struct str_dict) {.str="app",          .value=0},
        (struct str_dict) {.str="apple",        .value=0},
        (struct str_dict) {.str="application",  .value=0},
        (struct str_dict) {.str="apply",        .value=0},
        (struct str_dict) {.str="apt",          .value=0},
        (struct str_dict) {.str="bat",          .value=0},
        (struct str_dict) {.str="batch",        .value=0},
        (struct str_dict) {.str="bath",         .value=0},
        (struct str_dict) {.str="batman",       .value=0},
        (struct str_dict) {.str="cat",          .value=0},
        (struct str_dict) {.str="catalog",      .value=0},
        (struct str_dict) {.str="cater",        .value=0},
        (struct str_dict) {.str="catering",     .value=0},
        (struct str_dict) {.str="dog",          .value=0},
        (struct str_dict) {.str="dove",         .value=0},
        (struct str_dict) {.str="dot",          .value=0},
        (struct str_dict) {.str="door",         .value=0},
        (struct str_dict) {.str="doom",         .value=0},
        (struct str_dict) {.str="zoo",          .value=0},
        (struct str_dict) {.str="zoom",         .value=0},
    };

    // Set the value of each entry
    for (int i = 0; i < sizeof(entries) / sizeof(struct str_dict); i++)
        entries[i].value = i + 1;

    // Add nodes to the trie
    for (int i = 0; i < sizeof(entries) / sizeof(struct str_dict); i++)
        trie_add(trie, entries[i].str, &entries[i].value);

    const unsigned int require_rtree_size = rtree_find_required_size(trie);

    printf("require_rtree_size=%d\n\n", require_rtree_size);

    struct rtree_node* radix_tree = malloc((require_rtree_size) * sizeof(struct rtree_node));

    rtree_create(radix_tree, trie);

    for (int i = 0; i < require_rtree_size; i++)
        printf("%d: char=%c - skip=%d - tree_size=%d, value=%d\n", i, radix_tree[i].character, radix_tree[i].key_length, radix_tree[i].tree_size, radix_tree[i].value);
    
    for (int i = 0; i < sizeof(entries) / sizeof(struct str_dict); i++) {
        uint16_t result = rtree_search(radix_tree, entries[i].str, strlen(entries[i].str));

        if (result != entries[i].value)
            printf("Failed to find key '%s'\n", entries[i].str);
    }

    uint16_t result = rtree_search(radix_tree, "He", 2);

    trie_destroy(trie);
}
