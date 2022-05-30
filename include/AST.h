#ifndef AST_H
#define AST_H

// 2022 Ian Moffett


typedef enum {
    A_ADD,
    A_SUB,
    A_MUL,
    A_DIV,
    A_INTLIT,
    A_LVIDENT,
    A_ASSIGN
} AST_NODE_TYPE;


// A node for our AST.
struct ASTNode {
    AST_NODE_TYPE op;
    struct ASTNode* left;
    struct ASTNode* right;

    union {
        int val_int;
        int symbol_id;          // For A_IDENT.
    };
};


struct ASTNode* mkastnode(AST_NODE_TYPE op, struct ASTNode* left, struct ASTNode* right, int val_int);
struct ASTNode* mkastleaf(AST_NODE_TYPE op, int val_int);
struct ASTNode* mkastunary(AST_NODE_TYPE op, struct ASTNode* left, int val_int);
void free_ast(void);

#endif
