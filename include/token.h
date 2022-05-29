#ifndef TOKEN_H
#define TOKEN_H

// 2022 Ian Moffett


typedef enum {
    TT_PLUS,
    TT_MINUS,
    TT_STAR,
    TT_SLASH,
    TT_INTLIT,
    TT_EOF
} TOKEN_TYPE;



struct Token {
    TOKEN_TYPE type;
    int val_int;
};



#endif
