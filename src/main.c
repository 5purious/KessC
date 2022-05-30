#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <colors.h>
#include <lexer.h>
#include <token.h>
#include <parser.h>
#include <codegen.h>
#include <AST.h>

#if defined(_WIN32) || defined(WIN32)
#error LINUX ONLY!
#endif

uint8_t only_assembly = 0;
uint8_t error = 0;

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
    codegen_done();

    if (error) exit(1);
}



int main(int argc, const char** argv) {
    if (argc < 2) {
        printf(COLOR_ERROR "kcc: No input files!\n");
        return 1;
    } else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        printf("Usage: kcc file.kessc\n");
        printf("Flags:\n-S: Compiles only to assembly and returns the .s file\n");
    }


    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            start(argv[i]);
        } else if (strcmp(argv[i], "-S") == 0) {
            only_assembly = 1;
        }
    }

    return 0;
}
