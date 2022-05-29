#include <parser.h>
#include <AST.h>
#include <token.h>
#include <lexer.h>
#include <expr.h>
#include <colors.h>
#include <codegen.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// 2022 Ian Moffett

static struct Token cur_token;


// Parse a primary factor.
static struct ASTNode* primary() {
    struct ASTNode* n;

    switch (cur_token.type) {
        case TT_INTLIT:
            n = mkastleaf(A_INTLIT, cur_token.val_int);
            scan(&cur_token);
            return n;
        default:
            if (cur_token.type == TT_EOF) return NULL;
            extern size_t line;
            printf(COLOR_ERROR "Syntax error on line %ld\n", line);
            return NULL;
    }

}


// Return an AST with a root binary operator.
static struct ASTNode* binexpr() { 
    int node_type;
    struct ASTNode *n, *left, *right;

    left = primary();

    if (left == NULL) return NULL;

    // Just return left if there is no tokens left.
    if (cur_token.type == TT_EOF) {
        return left;
    }

    // Convert token into a node type.
    node_type = arithop(cur_token.type);
    scan(&cur_token);

    // Get the right-hand tree.
    right = binexpr(cur_token);

    // Now build the tree.
    n = mkastnode(node_type, left, right, 0);
    return n;

}


void parse(void) {
    scan(&cur_token);
    struct ASTNode* root = binexpr();
    gencode(root);
}
