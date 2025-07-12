#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

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

void aat_node_free(AatNode* node_ptr) {
	free(node_ptr);
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

void aat_node_split(AatNode** node_ptr) {
	if ((*node_ptr)->level == (*node_ptr)->right->right->level) {
		rotate_left(node_ptr);
		(*node_ptr)->level++;
	}
}

typedef struct {
	AatNode* root;
	AatNode* last;
	AatNode* deleted;
} AatTree;

AatTree* aat_tree_make() {
	AatTree* tree = malloc(sizeof(AatTree));

	bottom = malloc(sizeof(AatNode));
	bottom->left = bottom;
	bottom->right = bottom;
	bottom->level = 0;
	bottom->key = NULL;

	tree->root = bottom;
	tree->last = bottom;
	tree->deleted = bottom;
}

void aat_tree_free(AatTree* tree) {
	aat_tree_free_helper(tree->root);
}

void aat_tree_free_helper(AatNode* root) {
	if (root == NULL) return;
	if (root == bottom) {
		free(bottom);
		return;
	}
	aat_tree_free(root->left);
	aat_tree_free(root->right);
	aat_node_free(root);
}

void aat_tree_insert(int key_, AatTree* tree) {
	aat_tree_insert_helper(key_, tree->root);
}

void aat_tree_insert_helper(int key_, AatNode* root) {
	if (root == bottom) {
		AatNode* new_node = aat_node_make(key_);
	} else {
		aat_tree_insert_rebalance(key_, root);
	}
}

void aat_tree_insert_rebalance(int key_, AatNode* root) {
	if (key_ < root->key) aat_tree_insert(key_, root->left);
	else aat_tree_insert(key_, root->right);
	aat_node_skew(root);
	aat_node_split(root);
}

bool aat_tree_delete(int key_, AatTree* tree) {
	return aat_tree_delete_helper(key_, tree, tree->root);
}

bool aat_tree_delete_helper(int key_, AatTree* tree, AatNode* root) {
	// base case, reach bottom
	if (root == bottom) return false;

	bool success = false;
	// search down the tree
	tree->last = root;
	if (key_ < root->key) {
		success = aat_tree_delete_helper(key_, tree, root->left);
	}
	else {
		tree->deleted = root;
		success = aat_tree_delete_helper(key_, tree, root->right);
	}

	// remove
	if (
		(root == tree->last) && 
		(tree->deleted != bottom) && 
		(key_ == tree->deleted->key)
	) {
		tree->deleted->key = root->key;
		tree->deleted = bottom;
		root = root->right;
		aat_node_free(tree->last);
		success = true;
	}

	// rebalance on the way back
	else if (
		(root->left->level < root->level - 1) || 
		(root->right->level < root-> level - 1)
	) {
		root->level--;
		if (root->right->level > root->level) root->right->level--;
		aat_node_skew(root);
		aat_node_skew(root->right);
		aat_node_skew(root->right->right);
		aat_node_split(root);
		aat_node_split(root->right);
	}

	return success;
}