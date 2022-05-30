#include <lexer.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <colors.h>

// 2022 Ian Moffett


#define MAX_KEYWORD_LENGTH 45


size_t line = 0;
static size_t seekset_off = 0;
static FILE* cur_fp = NULL;


static char next(void) {
    char ret = fgetc(cur_fp);

    ++seekset_off;
    return ret;
}

// Lowers the file pointer.
static void dec_fp(void) {
    fseek(cur_fp, seekset_off -= 1, SEEK_SET);
}


static char skip(void) {
    char tmp = next();

    while (tmp == ' ' || tmp == '\t' || tmp == '\r' || tmp == '\n' || tmp == '\f') {
        if (tmp == '\n') ++line;

        tmp = next();
    }

    return tmp;
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

    dec_fp();               // Non-integer character reached, put it back.
    return val;
}


static ssize_t scanident(char ch, char* buf, int limit) {
    size_t i = 0;

    while (isalpha(ch) || isdigit(ch) || ch == '_') {
        if (i == limit -1) {
            printf(COLOR_ERROR "Error: Identifier too long on line %ld!\n", line);
            return -1;
        }

        buf[i++] = ch;
        ch = next();
    }

    // We have hit a character that is not in a identifier,
    // we will put it back.
    dec_fp();
    buf[i] = '\0';
    return i;
}


void lex_init(FILE* fp) {
    cur_fp = fp;
}


/*
 * @pkw: Possible key word.
 *
 */
static TOKEN_TYPE check_keyword(char* pkw) {
    if (strcmp(pkw, "prints") == 0) {
        return TT_PRINTS;
    }

    // Didn't find a keyword.
    return TT_INVALID;
}


// If this returns zero there are no tokens left.
char scan(struct Token* tok) {
    // Get next character.
    char ch = skip();

    switch (ch) {
        case EOF:
            tok->type = TT_EOF;
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
        case '(':
            tok->type = TT_LPAREN;
            break;
        case ')':
            tok->type = TT_RPAREN;
            break;
        case ';':
            tok->type = TT_SEMI;
            break;
        default:
            // Check if digit.
            if (isdigit(ch)) {
                tok->type = TT_INTLIT;
                tok->val_int = scan_int(ch);
                break;
            }

            char possible_keywrd[MAX_KEYWORD_LENGTH];

            if (scanident(ch, possible_keywrd, MAX_KEYWORD_LENGTH) == -1) {
                tok->type = TT_INVALID;
                return 0;
            }

            // No keyword was found.
            if ((tok->type = check_keyword(possible_keywrd)) == TT_INVALID) {
                extern uint8_t error;
                error = 1;
                printf(COLOR_ERROR "Error: Invalid keyword on line %ld\n", line);
                tok->type = TT_INVALID;
                return 0;
            }

            break;
    }

    return 1;
}
