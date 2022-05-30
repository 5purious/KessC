#include <expr.h>
#include <colors.h>
#include <stdio.h>
#include <stdlib.h>

// 2022 Ian Moffett


AST_NODE_TYPE arithop(TOKEN_TYPE token) {
    switch (token) {
        case TT_PLUS:
            return A_ADD;
        case TT_MINUS:
            return A_SUB;
        case TT_STAR:
            return A_MUL;
        case TT_SLASH:
            return A_DIV;
        case TT_INTLIT:
            return A_INTLIT;
        case TT_LT:
            return A_LT;
        case TT_GT:
            return A_GT;
        case TT_GE:
            return A_GE;
        case TT_LE:
            return A_LE;
        default:
            printf("Unknown type %d in %s.\n", token, __func__);
            exit(1);
    }
}
