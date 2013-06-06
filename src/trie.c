#include <stdlib.h>
#include <assert.h>
#include "trie.h"

struct Node* node_new(void) 
{
	struct Node* node = malloc(sizeof(struct Node));
	node->left = NULL;
	node->right = NULL;
	node->value = 0;

	return node;
}

void node_del(struct Node* node)
{
	assert(node);

	if (node->left)
		node_del(node->left);

	if (node->right)
		node_del(node->right);

	free(node);
}

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
