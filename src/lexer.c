#include <lexer.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>


static size_t line = 1;
static size_t seekset_off = 0;
static FILE* cur_fp = NULL;


static char next(void) {
    char ret = fgetc(cur_fp);

    if (ret == '\n') 
        ++line;

    ++seekset_off;
    return ret;
}


static char skip(void) {
    char tmp = next();

    while (tmp == ' ' || tmp == '\t' || tmp == '\r' || tmp == '\n' || tmp == '\f') {
        tmp = next();
    }

    return tmp;
}


// Lowers the file pointer.
static void dec_fp(void) {
    fseek(cur_fp, seekset_off - 1, SEEK_SET);
}


// Give position of character in string.
static int chrpos(char* s, char c) {
    char* p = strchr(s, c);
    return p ? p - s : -1;
}


// Read in a whole integer literal.
static int scan_int(char c) {
    int k, val = 0;

    while ((k = chrpos("0123456789", c)) >= 0) {
        val = val * 10 + k;
        c = next();
    }

    dec_fp();
    return val;
}


void lex_init(FILE* fp) {
    cur_fp = fp;
}


// If this returns zero there are no tokens left.
char scan(struct Token* tok) {
    // Get next character.
    char ch = skip();

    switch (ch) {
        case EOF:
            return 0;
        case '+':
            tok->type = TT_PLUS;
            break;
        case '-':
            tok->type = TT_MINUS;
            break;
        case '*':
            tok->type = TT_STAR;
            break;
        case '/':
            tok->type = TT_SLASH;
            break;
        default:
            // Check if digit.
            if (isdigit(ch)) {
                tok->type = TT_INTLIT;
                tok->val_int = scan_int(ch);

            }
    }

    return 1;
}
