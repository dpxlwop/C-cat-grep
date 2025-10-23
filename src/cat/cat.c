#include "cat.h"

#include <getopt.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

struct option long_options[] = {
    // прописываем зависимость длинных флагов к коротким
    {"number-nonblank", no_argument, 0, 'b'},
    //{"E", no_argument, 0, 'e'},
    {"number", no_argument, 0, 'n'},
    {"squeeze-blank", no_argument, 0, 's'},
    //{"T", no_argument, 0, 't'},
    {"all", no_argument, 0, 'a'},
    {0, 0, 0, 0}  // требуется для getopt_long
};

int main(int argc, char **argv) {
    int flag_counter = 0;
    int flag_list[argc];

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <options> <file1> [file2] ...\n", argv[0]);
        return 1;
    }

    int rez = 0;
    int option_index;  // индекс выбранной опции
    while ((rez = getopt_long(argc, argv, "benstaET", long_options, &option_index)) !=
           -1) {  // getopt парсит флаги из argv
        switch (rez) {
            case 'b':  // нумеровать непустые строки
                flag_list[flag_counter++] = 1;
                break;
            case 'e':  // отображать $ в конце каждой строки
            case 'E':
                flag_list[flag_counter++] = 2;
                break;
            case 'n':  // нумеровать все строки
                flag_list[flag_counter++] = 3;
                break;
            case 's':  // сжимать несколько пустых строк в одну
                flag_list[flag_counter++] = 4;
                break;
            case 't':  // отображать \t как ^I
            case 'T':
                flag_list[flag_counter++] = 5;
                break;
            case 'a':  // = -e и -t
                flag_list[flag_counter++] = 6;
                break;
            default:
                fprintf(stderr, "Unknown option '%c'\n", optopt);
                break;
        }
    }

    flag_counter = normalize_flags(flag_list, flag_counter);

    // optind указывает на первый аргумент после флагов
    for (int i = optind; i < argc; i++) file_proccess(argv[i], flag_list, flag_counter);

    return 0;
}

int normalize_flags(int *flags, int count) {
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
                for (int j = i; j < count - 1; ++j) flags[j] = flags[j + 1];
                count--;
            } else
                i++;
        }
    }

    // '-b' приоритетнее '-n'
    if (has_b && has_n) {
        for (int i = 0; i < count;) {
            if (flags[i] == 3) {
                for (int j = i; j < count - 1; ++j) flags[j] = flags[j + 1];
                count--;
            } else
                i++;
        }
    }

    return count;
}

int file_proccess(const char *filename, int *flags, int flag_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "File %s does not exists.\n", filename);
        return 1;
    }

    int flag_b = 0, flag_e = 0, flag_n = 0, flag_s = 0, flag_t = 0;
    // переменные флагов
    for (int i = 0; i < flag_count; i++) {  // переносим флаги из списка в переменные
        switch (flags[i]) {
            case 1:
                flag_b = 1;
                break;
            case 2:
                flag_e = 1;
                break;
            case 3:
                flag_n = 1;
                break;
            case 4:
                flag_s = 1;
                break;
            case 5:
                flag_t = 1;
                break;
            case 6:  // -a = -e и -t
                flag_e = 1;
                flag_t = 1;
                break;
        }
    }

    char string[MAX_LINE_LENGTH];
    int line_number = 1;
    int empty_streak = 0;  // флаг для подряд идущих пустых строк

    while (fgets(string, MAX_LINE_LENGTH, file) != NULL) {
        // убираем символ новой строки
        string[strcspn(string, "\n")] = '\0';     // заменяет \n на \0
        int is_empty_line = (string[0] == '\0');  // пустая ли строка

        if (flag_s) {
            if (is_empty_line) {
                if (empty_streak) continue;
                empty_streak = 1;
            } else {
                empty_streak = 0;
            }
        }

        if (flag_t) replace_tab(string);

        if (flag_b && !is_empty_line) {
            print_with_line_numbers(string, &line_number);
        } else if (flag_n && !flag_b) {
            print_with_line_numbers(string, &line_number);
        } else if (!flag_b && !flag_n && !flag_t) {
            printf("%s", string);
        }

        if (flag_e) printf("$");
        printf("\n");
    }

    fclose(file);
    return 0;
}

void print_with_line_numbers(const char *string, int *line_number) {
    printf("%6d\t%s", (*line_number)++, string);
}

void replace_tab(char *string) {
    for (char *p = string; *p != '\0'; p++) {
        if (*p == '\t') {
            printf("^I");
        } else {
            putchar(*p);
        }
    }
}