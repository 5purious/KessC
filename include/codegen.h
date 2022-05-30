#ifndef CODEGEN_H
#define CODEGEN_H

#include <AST.h>
#include <stdint.h>


void codegen_init(void);
void codegen_done(void);
void codegen_print_int(uint8_t reg);
int interpret_ast(struct ASTNode* root, uint8_t reg, int parent_ast_top);
void rmkglob_sym(char* symbol, uint8_t sz_bytes);
uint8_t rload_glob(char* identifier);

#endif
