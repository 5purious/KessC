#include <parser.h>
#include <AST.h>
#include <token.h>
#include <lexer.h>
#include <expr.h>
#include <colors.h>
#include <codegen.h>
#include <stddef.h>
#include <symbol.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

// 2022 Ian Moffett

static struct Token cur_token;

void clean_and_exit(void);


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
            printf(COLOR_ERROR "Error: Syntax error on line %ld\n", get_line());
            clean_and_exit();
            return NULL;            // This will never be reached.
    }

}

static void match(TOKEN_TYPE t, char* what) {
    extern uint8_t error;

    if (cur_token.type != t) {
        printf(COLOR_ERROR "Error: %s expected on line %ld\n", what, get_line());
        error = 1;
        clean_and_exit();
    }
}


// Return an AST with a root binary operator.
static struct ASTNode* binexpr() { 
    int node_type;
    struct ASTNode *n, *left, *right;

    left = primary();

    if (left == NULL) return NULL;

    // Just return left if there is no tokens left.
    if (cur_token.type == TT_RPAREN || cur_token.type == TT_SEMI || cur_token.type == TT_LBRACE) {
        return left;
    } else if (cur_token.type == TT_EOF) {
        match(TT_SEMI, "';'");                    // This will automatically fail.
    }

    // Convert token into a node type.
    node_type = arithop(cur_token.type);
    scan(&cur_token);

    // Get the right-hand tree.
    right = binexpr(cur_token);

    // Now build the tree.
    n = mkastnode(node_type, left, NULL, right, 0);
    return n;

}



static void end_statement(void) {
    scan(&cur_token);
    match(TT_SEMI, "';'");            // Check if semicolon.
    scan(&cur_token);
}


static void assignment(void) {
    struct ASTNode *left, *right, *tree;

    int id = locate_glob((char*)lexer_get_last_ident());

    if (id == -1) {
        printf(COLOR_ERROR "Error: Trying to assign to an undeclared variable on line %ld\n", get_line());
        clean_and_exit();
    }

    right = mkastleaf(A_LVIDENT, id);
    match(TT_EQUALS, "'='");
    scan(&cur_token);
    left = binexpr();
    tree = mkastnode(A_ASSIGN, left, NULL, right, 0);
    interpret_ast(tree, -1, -1);
}

static struct ASTNode* prints(void) {
    struct ASTNode* tree; 

    scan(&cur_token);
    match(TT_LPAREN, "'('");
    scan(&cur_token);

    if (cur_token.type != TT_IDENT)
        tree = binexpr();
    else {
        tree = mkastleaf(A_INTLIT, rload_glob((char*)lexer_get_last_ident()));
        scan(&cur_token);
        tree = mkastunary(A_PRINTVAR, tree, 0);
        // Check if rparen.
        match(TT_RPAREN, "')");
        end_statement();
        return tree;
    }

    // Check if rparen.
    match(TT_RPAREN, "')");
    end_statement();
    tree = mkastunary(A_PRINT, tree, 0);
    return tree;

}


// Parse a compund statement and
// return it's AST.
struct ASTNode* compound_statement(void) {
    struct ASTNode* left = NULL;
    struct ASTNode* tree;

    // We need a left curly brace.
    match(TT_LBRACE, "'{'");
    scan(&cur_token);

    while (1) {
       switch (cur_token.type) {
           case TT_PRINTS:
               tree = prints();
               break;
            case TT_RBRACE:
               scan(&cur_token);
               return left;
            default:
               printf(COLOR_ERROR "Syntax error on line %ld\n", get_line());
               clean_and_exit();
       }

        if (tree && left == NULL)
            left = tree;
        else
            left = mkastnode(A_GLUE, left, NULL, tree, 0);
    }
}


static struct ASTNode* if_statement(void) {
    struct ASTNode *condAST, *trueAST = NULL;

    // Make sure we have lparen.
    scan(&cur_token);
    match(TT_LPAREN, "'('");
    scan(&cur_token);

    condAST = binexpr();

    if (condAST->op < A_CMP || condAST->op > A_GE) {
        printf(COLOR_ERROR "ERROR: Bad operator in if statement condition on line %ld\n", get_line());
        clean_and_exit();
    } 

    // Check for rparen.
    match(TT_RPAREN, "')");
    scan(&cur_token);

    // Get AST for compound statement.
    trueAST = compound_statement();


    return mkastnode(A_IF, condAST, trueAST, NULL, 0);
}


static void keyword(void) {
    switch (cur_token.type) {
        case TT_PRINTS:
            interpret_ast(prints(), -1, -1);
            break;
        case TT_INT8:
            scan(&cur_token);
            match(TT_IDENT, "Identifier");

            // Allocate memory for glob.
            char* glob_ident = malloc(sizeof(char) * strlen(lexer_get_last_ident()) + 1);
            
            // Copy contents of old identifier into buffer.
            strcpy(glob_ident, lexer_get_last_ident());

            // Submit symbol.
            add_glob(glob_ident);
            scan(&cur_token);

            // Make space for a global symbol (1 byte).
            rmkglob_sym((char*)lexer_get_last_ident(), 1);

            // Check if we are defining or just declaring.
            if (cur_token.type == TT_EQUALS) {
                assignment();
            } else {
                // We are ending statmenet up here
                // and not at the end of
                // this case because
                // assignment() calls binexpr() which
                // does it's own check or semis.
                end_statement();
            }

            // Eat token.
            scan(&cur_token);
            break;
        case TT_IDENT:
             int id = locate_glob((char*)lexer_get_last_ident());

            if (id == -1) {
                printf(COLOR_ERROR "Error: Referencing an undeclared identifier on line %ld\n", get_line());
                clean_and_exit();
            }
            break;
        case TT_IF:
            interpret_ast(if_statement(), -1, -1);
            break;
        default: break;
    }
}


void parse(void) {
    codegen_init();

    scan(&cur_token);

    extern uint8_t error;

    while (is_tokens_left()) {
        keyword();

        if (error) return;
    }    
}
