#ifndef LEXER_H
#define LEXER_H

#include <token.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

// 2022 Ian Moffett

char scan(struct Token* tok);
void lex_init(FILE* fp);
uint8_t is_tokens_left(void);

#endif
