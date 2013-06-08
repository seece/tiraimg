#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include "eks_math.h"
#include "trie.h"

/**
 * @brief Creates and initializes a new node.
 *
 * @return the node
 */
struct Node* node_new(void) 
{
	struct Node* node = malloc(sizeof(struct Node));
	node->left = NULL;
	node->right = NULL;
	node->weight = 0;
	node->value = NODE_VALUE_NONE;

	return node;
}

/**
 * @brief Recursively deletes an allocated node and its children.
 *
 * @param node the node to delete
 */
void node_del(struct Node* node)
{
	assert(node);

	if (node->left)
		node_del(node->left);

	if (node->right)
		node_del(node->right);

	free(node);
}

/**
 * @brief Joins to nodes to create a new node with left and right
 * members pointing to the given parameters. New weight is calculated
 * by summing the weights of the two nodes.
 *
 * @param left first node
 * @param right second node
 *
 * @return the new allocated node with the given nodes as children
 */
struct Node* node_join(struct Node* left, struct Node* right)
{
	assert(left);	
	assert(right);	

	struct Node* root = node_new();
	root->left = left;
	root->right = right;
	root->weight= left->weight+ right->weight;

	return root;
}

/**
 * @brief Checks if the given node is a leaf node. A node is leaf if it doesn't
 * have any children.
 *
 * @param node target node
 *
 * @return true if leaf, false if not
 */
bool node_is_leaf(struct Node* node)
{
	assert(node);

	if (!node->left && !node->right)
		return true;

	return false;
}


/**
 * @brief Generates a path from tree root to the given leaf node value. The
 * resulting path is saved as an unsigned integer to the code_out variable.
 * Each zero bit represents a turn to the left, and one bit a turn to the
 * right respectively.
 *
 * @param node tree root node
 * @param needle route end point leaf value
 * @param level iteration level, should be 0 on initial call
 * @param code_out output of the route code
 * @param length_out output of route length 
 *
 * @return same value as needle if successful, otherwise something else
 */
static int32_t find_value_path(struct Node* node, int32_t needle, int32_t level,
		uint32_t* code_out, int32_t* length_out)
{
	assert(level < 32);
	if (!node)
		return NODE_VALUE_NONE;

	if (node->value == needle) {
		*length_out = level;
		return node->value;
	}

	uint32_t code = *code_out;
	uint32_t leftcode  = (code << 1) | 0;
	uint32_t rightcode = (code << 1) | 1;

	int32_t leftval = find_value_path(node->left, needle, level+1, 
			&leftcode, length_out);

	if (leftval == needle) {
		*code_out= leftcode;
		return leftval;
	}

	int32_t rightval = find_value_path(node->right, needle, level+1, 
			&rightcode, length_out);

	if (rightval == needle) {
		*code_out= rightcode;
		return rightval;
	}

	return node->value;
}

struct SymbolCode node_get_code(struct Node* root, int32_t value)
{
	struct SymbolCode symbol = {.code = 0, .length = 0, .value = value};
	find_value_path(root, value, 0, &symbol.code, &symbol.length);

	return symbol;
}

/**
 * @brief Calculates a depth of a tree.
 *
 * @param root tree root node
 * @param depth the level where to start counting, should be 0 on initial call
 *
 * @return the number of levels in the tree
 */
int32_t node_tree_depth(struct Node* root, int32_t depth)
{
	if (!root)
		return depth;

	return MAX(node_tree_depth(root->left, depth+1), 
		node_tree_depth(root->right, depth+1));
}

/**
 * @brief Counts the amount of leaf nodes in the given tree.
 *
 * @param node tree root node
 *
 * @return the number of leaf nodes
 */
int32_t node_count_leaf_nodes(struct Node* node)
{
	if (!node)
		return 0;

	if (node_is_leaf(node))
		return 1;
	else
		return node_count_leaf_nodes(node->left) + node_count_leaf_nodes(node->right);
}

