#ifndef CTOOLS_BINARY_TREE
#define CTOOLS_BINARY_TREE

typedef struct btree_node {
    void* value;
    struct btree_node* hi;
    struct btree_node* lo;
} btree_node;

/// @brief Initializes all members of a btree_node to NULL.
/// @param node The node to initialize
void btree_node_create(btree_node* node);
// void btree_node_destroy(btree_node* node);

/// @brief Checks whether a tree node is a leaf node.
/// A btree_node is a leaf node when its child pointers are set to NULL, meaning that it has no children.
/// If the node has one child, it is not considered a leaf node.
/// @param node The node to check.
/// @return 1 if the node is a leaf node. Otherwise, returns 0.
int is_leaf_node(const btree_node* node);

/// @brief Performs a Depth First Search (DFS) on a binary tree. The traversal is done in pre-order mode, visiting the parent before visiting both child nodes.
/// @param top_node The top node of the binary tree.
/// @param callback The function to call when visiting each node in the tree. The traversal will stop if this function returns a non-zero value.
/// @return The result of the 'callback' function.
int btree_depth_first_search(btree_node* top_node, int (*callback)(btree_node* node));

/// @brief Performs a Breadth First Search (BFS) on a binary tree.
/// @param top_node The top node of the binary tree.
/// @param callback The function to call when visiting each node in the tree. The traversal will stop if this function returns a non-zero value.
/// @return The result of the 'callback' function.
int btree_breadth_first_search(btree_node* top_node, int (*callback)(btree_node* node));

// void serialize(const btree* tree, char* dst);
// void deserialize(char* src, const btree* dst);

#endif // CTOOLS_BINARY_TREE