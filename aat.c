#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "stack.h"

typedef struct AatNode {
	int key;
	int level;
	struct AatNode* left;
	struct AatNode* right;
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
	bottom->key = 0;

	tree->root = bottom;
	tree->last = bottom;
	tree->deleted = bottom;

	return tree;
}

void aat_tree_free_helper(AatNode* root) {
	if (root == NULL || root == bottom) return;
	aat_tree_free_helper(root->left);
	aat_tree_free_helper(root->right);
	free(root);
}

void aat_tree_free(AatTree* tree) {
	aat_tree_free_helper(tree->root);
	free(tree);
}

void aat_tree_insert_helper(int key_, AatNode** root);
void aat_tree_insert_rebalance(int key_, AatNode** root) {
	if (key_ < (*root)->key) aat_tree_insert_helper(key_, &(*root)->left);
	else aat_tree_insert_helper(key_, &(*root)->right);
	aat_node_skew(root);
	aat_node_split(root);
}

void aat_tree_insert_helper(int key_, AatNode** root) {
	if (*root == bottom) {
		*root = aat_node_make(key_);
	} else {
		aat_tree_insert_rebalance(key_, root);
	}
}

void aat_tree_insert(int key_, AatTree* tree) {
	aat_tree_insert_helper(key_, &tree->root);
}

bool aat_tree_delete_helper(int key_, AatTree* tree, AatNode** root) {
	// base case, reach bottom
	if (*root == bottom) return false;

	bool success = false;
	// search down the tree
	tree->last = *root;
	if (key_ < (*root)->key) {
		success = aat_tree_delete_helper(key_, tree, &(*root)->left);
	}
	else {
		tree->deleted = *root;
		success = aat_tree_delete_helper(key_, tree, &(*root)->right);
	}

	// remove
	if (
		(*root == tree->last) && 
		(tree->deleted != bottom) && 
		(key_ == tree->deleted->key)
	) {
		tree->deleted->key = (*root)->key;
		tree->deleted = bottom;
		*root = (*root)->right;
		aat_node_free(tree->last);
		success = true;
	}

	// rebalance on the way back
	else if (
		((*root)->left->level < (*root)->level - 1) || 
		((*root)->right->level < (*root)-> level - 1)
	) {
		(*root)->level--;
		if ((*root)->right->level > (*root)->level) (*root)->right->level--;
		aat_node_skew(root);
		aat_node_skew(&(*root)->right);
		aat_node_skew(&(*root)->right->right);
		aat_node_split(root);
		aat_node_split(&(*root)->right);
	}

	return success;
}

bool aat_tree_delete(int key_, AatTree* tree) {
	return aat_tree_delete_helper(key_, tree, &tree->root);
}

// using a stack implementation that is ChatGPT-generated for inorder print
void aat_tree_inorder_print(AatTree* tree) {
	NodeStack stack;
	stack_init(&stack);

	AatNode* current = tree->root;

	while (!stack_is_empty(&stack) || current != bottom) {
		while (current != bottom) {
			stack_push(&stack, current);
			current = current->left;
		}
		current = stack_pop(&stack);
		printf("%d, ", current-> key);
		current = current->right;
	}

	stack_free(&stack);
}

int main(int argc, char* argv[]) {
	size_t n = 100;
	int*  v = malloc(n * sizeof(int));
	for (int i = 0; i < n; i++) {
		v[i] = i;
	}

	for (int i = 0; i < n / 2; i++) {
		int j = rand() % n;
		int temp = v[i];
		v[i] = v[j];
		v[j] = temp;
	}

	AatTree* tree = aat_tree_make();
	printf("The list of input by order of insertion: \n");
	for (int i=0; i < n; i++) {
		printf("%d, ", v[i]);
		aat_tree_insert(v[i], tree);
	}
	printf("\n");

	printf("Inorder traversal of the tree: \n");
	aat_tree_inorder_print(tree);
	printf("\n");
	aat_tree_free(tree);
	free(bottom); // bad practice, probably, but good for demo
	free(v);

	return 0;
}