/**
 * @file trie.h
 * @brief Huffman-binary tree handling functions and datatypes.
 */

#ifndef TRIE_H_
#define TRIE_H_

#include <stdint.h>
#include <stdbool.h>

// Used in Node.value field to mark a non-leaf node
#define NODE_VALUE_NONE -1


/**
 * @brief A binary tree node.
 */
struct Node {
	struct Node* left;
	struct Node* right;
	int32_t weight;
	int32_t value;
};


/**
 * @brief A Huffman code symbol.
 */
struct SymbolCode {

	/** Code binary representation */
	uint32_t code;
	/** Length of the binary representation in bits */
	int32_t length;
	/** The actual symbol this code represents */
	int32_t value;
};

struct Node* node_new(void);
void node_del(struct Node* node);
struct Node* node_join(struct Node* left, struct Node* right);
bool node_is_leaf(struct Node* node);
struct SymbolCode node_get_code(struct Node* root, int32_t value);
int32_t node_tree_depth(struct Node* root, int32_t depth);
int32_t node_count_leaf_nodes(struct Node* node);

uint8_t* node_serialize_tree(struct Node* tree, int32_t* length_out);
struct Node* node_unserialize_tree(uint8_t* data, int32_t length);
#endif
