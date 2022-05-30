#ifndef SYMBOL_H
#define SYMBOL_H

// 2022 Ian Moffett

#define NSYMBOLS 2302


struct SymbolTable {
    char* name;
};


int locate_glob(char* sym);
int add_glob(char* sym);
void symbol_tbl_init(void);
void symbol_tbl_free(void);

#endif
