#include <parser.h>
#include <AST.h>
#include <token.h>
#include <lexer.h>
#include <expr.h>
#include <colors.h>
#include <codegen.h>
#include <stddef.h>
#include <stdint.h>
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
            printf(COLOR_ERROR "Error: Syntax error on line %ld\n", line);
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


static void match(TOKEN_TYPE t, char* what) {
    extern size_t line;
    extern uint8_t error;

    if (cur_token.type != t) {
        printf(COLOR_ERROR "Error: %s expected on line %ld\n", what, line);
        error = 1;
    }
}


static void keyword(void) {
    switch (cur_token.type) {
        case TT_PRINTS:
            scan(&cur_token);
            match(TT_INTLIT, "Error: Integer literal");
            struct ASTNode* root = binexpr();
            codegen_print_int(interpret_ast(root));         // Write our integer from expression or integer.
            break;
        default: break;
    }
}


void parse(void) {
    codegen_init();
    scan(&cur_token);
    keyword();
}
