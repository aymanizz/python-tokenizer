#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"

static char *readFile(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "error: could not open file \"%s\".\n", path);
        exit(10);
    }

    fseek(file, 0L, SEEK_END);
    size_t filesize = ftell(file);
    rewind(file);

    char *source = malloc (filesize + 1);
    if (!source) {
        fprintf(stderr, "error: not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    const size_t objects_num = filesize / sizeof(*source);
    size_t bytes_read = fread(source, sizeof(*source), objects_num, file);
    if (bytes_read < objects_num) {
        fprintf(stderr, "error: could not read file \"%s\".\n", path);
        exit(74);
    }

    source[bytes_read] = '\0';

    fclose(file);

    return source;
}

static void printToken(Token const token) {
    const char *name = Token_Names[token.type];

    switch (token.type) {
        case TOKEN_NEWLINE:
        case TOKEN_ENDMARKER:
            printf("%02i, %02i: \t %-16s\n", token.line, token.column, name);
            break;
        default:
            printf("%02i, %02i: \t %-16s \'%.*s\'\n",
                token.line, token.column, name, token.length, token.start);
    }
}

static void runFile(const char *path) {
    char *source = readFile(path);

    Scanner scanner;
    initScanner(&scanner, source);
    for (Token tok = scanToken(&scanner);
        tok.type != TOKEN_ENDMARKER;
        tok = scanToken(&scanner))
    {
        printToken(tok);
    }

    free(source);
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        runFile(argv[1]);
    } else {
        printf("usage: %s filepath\n", argv[0]);
    }
}
