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
        default:
            printf("Unknown type %d in %s.\n", token, __func__);
            exit(1);
    }
}
