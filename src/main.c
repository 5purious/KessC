#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <colors.h>


static void start(const char* filename) {
    if (access(filename, F_OK) != 0) {
        printf(COLOR_ERROR "kcc: Input file \"%s\" does not exist!\n", filename);
        exit(1);
    }


    FILE* fp = fopen(filename, "r");

    // Get file size.
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Allocate memory for contents.
    char* contents = calloc(file_size + 1, sizeof(char));

    free(contents);
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
