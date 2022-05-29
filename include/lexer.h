#ifndef LEXER_H
#define LEXER_H

#include <token.h>
#include <stddef.h>
#include <stdio.h>

char scan(struct Token* tok);
void lex_init(FILE* fp);

#endif
