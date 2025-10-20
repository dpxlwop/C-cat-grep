#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cat.h"

#define MAX_ARGS 7

void normalize_flags(int* flags, int count, int* deleted_flags);
int print_file(const char *filename);

int main(int argc, char **argv) {
    int flag_counter = 0, deleted_flags = 0;
    int flag_list[MAX_ARGS] = {0};

    if (argc < 2) {
        printf("Usage: %s <options> <file1> [file2] ...\n", argv[0]);
        return 1;
    }

    int rez = 0;
    while ((rez = getopt(argc, argv, "bensta")) != -1) { // убрал двоеточие после 'a'
        switch (rez) {
            case 'b': flag_list[flag_counter++] = 1; break;
            case 'e': flag_list[flag_counter++] = 2; break;
            case 'n': flag_list[flag_counter++] = 3; break;
            case 's': flag_list[flag_counter++] = 4; break;
            case 't': flag_list[flag_counter++] = 5; break;
            case 'a': flag_list[flag_counter++] = 6; break;
            default: printf("Unknown option '%c'\n", optopt); break;
        }
    }

    normalize_flags(flag_list, flag_counter, &deleted_flags);

    for (int i = 0; i < flag_counter - deleted_flags; i++)
        printf("Flag[%d]: %d\n", i, flag_list[i]);

    for (int i = optind; i < argc; i++) {
        if (print_file(argv[i]) != 0)
            return 1;
    }

    return 0;
}

void normalize_flags(int* flags, int count, int* deleted_flags) {
    int has_a = 0, has_b = 0, has_n = 0;

    for (int i = 0; i < count; ++i) {
        if (flags[i] == 6) has_a = 1;
        if (flags[i] == 1) has_b = 1;
        if (flags[i] == 3) has_n = 1;
    }

    // Если есть '-a', удалить '-e' и '-t'
    if (has_a) {
        for (int i = 0; i < count;) {
            if (flags[i] == 2 || flags[i] == 5) {
                for (int j = i; j < count - 1; ++j)
                    flags[j] = flags[j + 1];
                count--;
                (*deleted_flags)++;
            } else i++;
        }
    }

    // '-b' приоритетнее '-n'
    if (has_b && has_n) {
        for (int i = 0; i < count;) {
            if (flags[i] == 3) {
                for (int j = i; j < count - 1; ++j)
                    flags[j] = flags[j + 1];
                count--;
                (*deleted_flags)++;
            } else i++;
        }
    }
}

int print_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error opening file %s.\n", filename);
        return 1;
    }

    int ch;
    while ((ch = fgetc(file)) != EOF)
        putchar(ch);

    fclose(file);
    return 0;
}