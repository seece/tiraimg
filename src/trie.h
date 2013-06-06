#ifndef TRIE_H_
#define TRIE_H_

#include <stdint.h>

struct Node {
	struct Node* left;
	struct Node* right;
	int32_t value;
};

struct Node* node_new(void);
void node_del(struct Node* node);
struct Node* node_join(struct Node* left, struct Node* right);
#endif
