#include <AST.h>
#include <colors.h>
#include <stdlib.h>
#include <stdio.h>

// 2022 Ian Moffett


static struct ASTNode** nodes_allocated = NULL;
static size_t alloc_idx = 0;


struct ASTNode* mkastnode(AST_NODE_TYPE op, struct ASTNode* left, struct ASTNode* mid, struct ASTNode* right, int val_int) {
    if (nodes_allocated == NULL) {
        nodes_allocated = malloc(sizeof(struct ASTNode));
    } else {
        nodes_allocated = realloc(nodes_allocated, sizeof(struct ASTNode) * (alloc_idx + 3));
    }

    struct ASTNode* node = malloc(sizeof(struct ASTNode));
    nodes_allocated[alloc_idx++] = node;

    if (node == NULL) {
        printf(COLOR_ERROR "Unable to call malloc in: %s", __func__);
        exit(1);
    }

    node->op = op;
    node->left = left;
    node->mid = mid;
    node->right = right;
    node->val_int = val_int;
    return node;
}

struct ASTNode* mkastleaf(AST_NODE_TYPE op, int val_int) {
    return mkastnode(op, NULL, NULL, NULL, val_int);
}

struct ASTNode* mkastunary(AST_NODE_TYPE op, struct ASTNode* left, int val_int) {
    return mkastnode(op, left, NULL, NULL, val_int);
}


void free_ast(void) {
    for (size_t i = 0; i < alloc_idx; ++i) {
        free(nodes_allocated[i]);
    }

    free(nodes_allocated);
    nodes_allocated = NULL;
}
