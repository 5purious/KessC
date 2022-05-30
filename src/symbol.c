#include <symbol.h>
#include <string.h>
#include <stdint.h>
#include <colors.h>
#include <stdio.h>
#include <stdlib.h>


struct SymbolTable globl_sym_tbl[NSYMBOLS];
static int globs = 0;


int locate_glob(char* sym) {
    for (int i = 0; i < globs; ++i) {
        if (globl_sym_tbl[i].name[0] == sym[0] && strcmp(sym, globl_sym_tbl[i].name) == 0) {
            return i;
        }
    }

    return -1;
}


/*  
 *  Returns position of a new global symbol slot.
 *
 *  If we don't have any room to allocate
 *  the error var will be set and
 *  this will return -1.
 *
 */

static int newglob(void) {
    int pos = globs++;

    if (pos >= NSYMBOLS) {
        extern uint8_t error;
        printf(COLOR_ERROR "Too many global symols!\n");
        error = 1;
        return -1;
    }

    return pos;
}

/*
 *  @sym MUST BE ALLOCATED ON HEAP.
 *
 */
int add_glob(char* sym) {
    int y = locate_glob(sym);

    // Symbol already exists.
    if (y != -1) {
        free(sym);
        return y;
    }

    y = newglob();
    globl_sym_tbl[y].name = sym;
    return y;
}


void symbol_tbl_init(void) {
    for (size_t i = 0; i < NSYMBOLS; ++i) {
        globl_sym_tbl[i].name = NULL;
    }
}


void symbol_tbl_free(void) {
    for (size_t i = 0; i < NSYMBOLS; ++i) {
        if (globl_sym_tbl[i].name != NULL) {
            free(globl_sym_tbl[i].name);
        }
    }
}
