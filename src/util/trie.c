#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../eks_math.h"
#include "trie.h"
#include "stack.h"

#define NODE_TYPE_NORMAL 0
#define NODE_TYPE_LEAF 1

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

/**
 * @brief Returns a matching SymbolCode of a leaf node of the
 * given tree.
 *
 * @param root the tree
 * @param value the value of the target leaf node
 *
 * @return a SymbolCode struct 
 */
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

/**
 * @brief Counts the nodes in the given tree.
 *
 * @param node a binary tree root node
 *
 * @return the number of nodes
 */
int32_t node_count_nodes(struct Node* node)
{
	if (!node)
		return 0;

	return 1 + node_count_nodes(node->left) + node_count_nodes(node->right);
}

/**
 * @brief Serializes a tree to a byte array.
 *
 * @param tree target tree root node
 * @param length_out output variable for byte array length
 *
 * @return the allocated array
 */
uint8_t* node_serialize_tree(struct Node* tree, int32_t* length_out)
{
	int32_t node_amount = node_count_nodes(tree);
	int32_t leaf_amount = node_count_leaf_nodes(tree);
	
	int32_t data_len = 4 + leaf_amount*2 + (node_amount-leaf_amount);
	uint8_t* data = malloc(data_len);
	int32_t pos = 0;

	memcpy(data, &node_amount, 4);
	pos+=4;

	struct Stack* stack = stack_new(16);
	struct Node* node = tree;

	// Iterate the tree in pre-order and save the nodes.
	while (!stack_is_empty(stack) || (node != NULL))
	{
		if (!node) {
			node = stack_pop(stack);
			continue;
		}

		if (node_is_leaf(node)) {
			data[pos] = NODE_TYPE_LEAF;
			pos++;
			assert(node->value < 256); // the values should fit in a byte
			data[pos] = node->value;
			pos++;
			assert(!node->left);
			assert(!node->right);
		} else {
			data[pos] = NODE_TYPE_NORMAL;

			pos++;
		}

		stack_push(stack, node->right);
		node = node->left;
	}

	stack_del(stack);

	assert(pos == data_len);

	*length_out = data_len;
	return data;	
}

/**
 * @brief Unserializes a serialized tree recursively, used in node_unserialize_tree.
 *
 * @param data serialized tree data
 * @param length length of the data in bytes
 * @param pos pointer to an external position indicator, should point to the beginning 
 * of the node data
 *
 * @return the built tree
 */
static struct Node* unserialize_iter(uint8_t* data, int32_t length, int32_t* pos)
{
	if (*pos >= length-1) 
		return NULL;

	uint8_t type = data[*pos];
	uint8_t value;
	struct Node* node = NULL;
	(*pos)++;

	switch (type) {
		case NODE_TYPE_NORMAL:
			node = node_new();

			struct Node* left_node = unserialize_iter(data, length, pos);
			struct Node* right_node= unserialize_iter(data, length, pos);

			node->value = NODE_VALUE_NONE;
			node->weight = 0; // the weight information is lost during serialization
			node->left = left_node;
			node->right = right_node;

			break;

		case NODE_TYPE_LEAF:
			value = data[*pos];
			(*pos)++;

			node = node_new();
			node->value = value;
			node->weight = 0;
			node->left = NULL;
			node->right = NULL;

			break;

		default:
			fprintf(stderr, "ERROR: Invalid tree node type 0x%x!\n", type);
			node = NULL;
			break;
	}

	return node;
}

/**
 * @brief Builds a Huffman-tree structure from a serialized tree.
 *
 * @param data serialized tree data
 * @param length data length in bytes
 * @param bytes_read the amount of bytes actually read
 *
 * @return the built tree
 */
struct Node* node_unserialize_tree(uint8_t* data, int32_t length, int32_t* bytes_read)
{
	int32_t node_amount = -1;
	int32_t pos = 0;

	memcpy(&node_amount, data, 4);
	pos+=4;

	struct Node* tree = unserialize_iter(data, length, &pos);

	*bytes_read = pos;
	
	return tree;
}


/**
 * @brief Prints the tree recursively.
 *
 * @param root tree root node
 * @param level indentation level, should be 0 on initial invocation
 */
void node_print_tree(struct Node* root, int32_t level)
{
	if (root == NULL)
		return;

	printf("%*s", level*4, " ");
	printf("  ");

	if (root->value == NODE_VALUE_NONE) {
		printf("(x) %d", root->weight);
	} else {
		printf("(%d) %d", root->value, root->weight);
	}

	printf("\n");

	node_print_tree(root->left, level+1);
	node_print_tree(root->right, level+1);
}


