#ifndef AST_H
#define AST_H

// 2022 Ian Moffett


typedef enum {
    A_ADD,
    A_SUB,
    A_MUL,
    A_DIV,
    A_INTLIT
} AST_NODE_TYPE;


// A node for our AST.
struct ASTNode {
    AST_NODE_TYPE op;
    struct ASTNode* left;
    struct ASTNode* right;
    int val_int;
};


struct ASTNode* mkastnode(AST_NODE_TYPE op, struct ASTNode* left, struct ASTNode* right, int val_int);
struct ASTNode* mkastleaf(AST_NODE_TYPE op, int val_int);
struct ASTNode* mkastunary(AST_NODE_TYPE op, struct ASTNode* left, int val_int);
void free_ast(void);

#endif
