#include "ctools/trie/rtree.h"

#define STACK_NAME stack
#define STACK_TYPE uint16_t
#include "ctools/stack.h"

void serialize_node(const struct trie_node* src, struct rtree_node* out, uint16_t* next_index) {
    uint16_t my_index = (*next_index)++;

    out[my_index].character = src->key;

    unsigned int key_length = 0;
    for (;(src->subnodes_count == 1) & (src->value == NULL); src = src->subnodes, key_length++);

    out[my_index].key_length = key_length;
    out[my_index].value = 0;
    if (src->value != NULL)
        out[my_index].value = *((uint16_t*) src->value);

    uint16_t before_children = *next_index;

    for (unsigned int i = 0; i < src->subnodes_count; i++)
        serialize_node(&src->subnodes[i], out, next_index);

    uint16_t after_children = *next_index;

    out[my_index].tree_size = after_children - before_children;
}

unsigned int rtree_find_required_size(const struct trie_node* top_node) {
    // The number of would-be nodes in an rtree produced from the top node.
    // Initialized to 1 because we are counting the top node as well.
    unsigned int num_nodes = 1;

    // Will hold a pointer to the next node of interest, which we find in the next step.
    const struct trie_node* node_it = top_node;

    // Travel down the subnodes until we find either a branching or leaf node.
    for (;node_it->subnodes_count == 1; node_it = node_it->subnodes)
        if (node_it->value != NULL)
            num_nodes++;

    // Call this function recursively on each subnode.
    for (unsigned int i = 0; i < node_it->subnodes_count; i++)
        num_nodes += rtree_find_required_size(&node_it->subnodes[i]);
    
    return num_nodes;
}

void rtree_create(struct rtree_node* out, const struct trie_node* top_node) {
    // unsigned int required_size = find_required_rtree_size(top_node);

    // struct rtree_node* out = malloc(required_size * sizeof(struct rtree_node));

    uint16_t next_index = 0;
    serialize_node(top_node, out, &next_index);
}

// void rtree_destroy(struct rtree_node* top_node) {
//     free(top_node);
// }

uint16_t rtree_search(const struct rtree_node* nodes, const char* query_string, const unsigned int query_string_length) {
    // An iterator to seek through the query_string
    unsigned int query_string_it = 0;

    // We start searching from the top node
    uint16_t current_node = 0;

    // Loop until we hit a leaf node.
    while (nodes[current_node].tree_size) {
        // If we have scanned the exact length of the query string, we have to return whatever value
        // registered in the current node. We might have reached a branching node that contains a value.
        // This is a typical case when e.g. an API has endpoints like these:
        //   - /api/user
        //   - /api/user/login
        //   - /api/user/create
        // if (query_string_it == query_string_length)
        //     break;
            
        // Seek to to the character that this node's subnodes compares with.
        query_string_it += nodes[current_node].key_length;

        // If we have scanned the exact length of the query string, we have to return whatever value
        // registered in the current node. We might have reached a branching node that contains a value.
        // This is a typical case when e.g. an API has endpoints like these:
        //   - /api/user
        //   - /api/user/login
        //   - /api/user/create
        if (query_string_it == query_string_length)
            break;

        // If the subnodes wants to match with a character that is beyond the length of 
        // the query_string, the query string does not match anything in this trie.
        if (query_string_it >= query_string_length)
            return -1;

        // If a matching subnode is found, it will be stored here.
        uint16_t matching_node = 0;

        // Search for a subnode that matches with the query string
        for (uint16_t subnode = current_node + 1; !matching_node & (subnode < nodes->tree_size + 1); subnode += nodes[subnode].tree_size + 1)
            if (nodes[subnode].character == query_string[query_string_it])
                matching_node = subnode;

        // If none of the subnodes match, the query string does not exist in this trie.
        if (!matching_node)
            return -1;
        
        // Pass by the character we matched on
        query_string_it++;
        
        // Continue processing from the subnode.
        current_node = matching_node;
    }

    return nodes[current_node].value;
}