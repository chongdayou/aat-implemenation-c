#include <stdlib.h>
#include <stdio.h>

typedef struct {
	int key;
	int level;
	AatNode* left;
	AatNode* right;
} AatNode;

AatNode* bottom;

AatNode* aat_node_make(int key_) {
	AatNode* node = malloc(sizeof(AatNode));
	node->key = key_;
	node->level = 1;
	node->left = bottom;
	node->right = bottom;
	return node;
}

void rotate_right(AatNode** node_ptr) {
	AatNode* new_parent;
	new_parent = (*node_ptr)->left;
	(*node_ptr)->left = new_parent->right;
	new_parent->right = *node_ptr;
	*node_ptr = new_parent;
}

void rotate_left(AatNode** node_ptr) {
	AatNode* new_parent;
	new_parent = (*node_ptr)->right;
	(*node_ptr)->right = new_parent->left;
	new_parent->left = *node_ptr;
	*node_ptr = new_parent;
}

void aat_node_skew(AatNode** node_ptr) {
	if ((*node_ptr)->level == (*node_ptr)->left->level) {
		rotate_right(node_ptr);
	}
}

void aat_node_skew(AatNode** node_ptr) {
	if ((*node_ptr)->level == (*node_ptr)->right->right->level) {
		rotate_left(node_ptr);
		(*node_ptr)->level++;
	}
}

typedef struct {
	AatNode* root;
	AatNode* bottom;
	AatNode* last;
	AatNode* deleted;
} AatTree;