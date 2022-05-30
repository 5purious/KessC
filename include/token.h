#ifndef TOKEN_H
#define TOKEN_H

// 2022 Ian Moffett


typedef enum {
    TT_PLUS,                    // '+'
    TT_MINUS,                   // '-'
    TT_STAR,                    // '*'
    TT_SLASH,                   // '/'
    TT_INTLIT,                  // [0-9]
    TT_PRINTS,                  // 'prints'
    TT_LPAREN,                  // '('
    TT_RPAREN,                  // ')'
    TT_SEMI,                    // ';'
    TT_EQUALS,                  // '='
    TT_INT8,
    TT_IDENT,
    TT_INVALID, 
    TT_EOF
} TOKEN_TYPE;



struct Token {
    TOKEN_TYPE type;
    int val_int;
};



#endif
