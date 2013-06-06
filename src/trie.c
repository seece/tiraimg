#include <stdlib.h>
#include <assert.h>
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
	node->value = 0;

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
 * members pointing to the given parameters. New value is calculated
 * by summing the values of the two nodes.
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
	root->value = left->value + right->value;

	return root;
}
