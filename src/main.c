#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <colors.h>
#include <lexer.h>
#include <token.h>
#include <parser.h>
#include <AST.h>

#if defined(_WIN32) || defined(WIN32)
#error LINUX ONLY!
#endif


static void start(const char* filename) {
    if (access(filename, F_OK) != 0) {
        printf(COLOR_ERROR "kcc: Input file \"%s\" does not exist!\n", filename);
        exit(1);
    }

    FILE* fp = fopen(filename, "r");
    lex_init(fp);

    parse();

    free_ast();
    fclose(fp);
}



int main(int argc, const char** argv) {
    if (argc < 2) {
        printf(COLOR_ERROR "kcc: No input files!\n");
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            start(argv[i]);
        }
    }

    return 0;
}
