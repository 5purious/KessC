#ifndef TOKEN_H
#define TOKEN_H

// 2022 Ian Moffett


typedef enum {
    TT_EOF,
    TT_PLUS,
    TT_MINUS,
    TT_STAR,
    TT_SLASH,
    TT_CMP,
    TT_NE,
    TT_LT,
    TT_GT,
    TT_LE,
    TT_GE,
    TT_INTLIT,
    TT_SEMI,
    TT_EQUALS,
    TT_IDENT,
    TT_PRINTS,
    TT_INT8,
    TT_IF,
    TT_LBRACE,
    TT_RBRACE,
    TT_LPAREN,
    TT_RPAREN,
    TT_INVALID,
} TOKEN_TYPE;


struct Token {
    TOKEN_TYPE type;
    int val_int;
};



#endif
