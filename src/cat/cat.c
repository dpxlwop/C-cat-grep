#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cat.h"

#define MAX_ARGS 7
#define MAX_LINE_LENGTH 1024

int main(int argc, char** argv) {
    int flag_counter = 0, deleted_flags = 0;
    int flag_list[MAX_ARGS] = {0};

    if (argc < 2) {
        printf("Usage: %s <options> <file1> [file2] ...\n", argv[0]);
        return 1;
    }

    int rez = 0;
    while ((rez = getopt(argc, argv, "bensta")) != -1) {
        switch (rez) {
            case 'b':  // нумеровать непустые строки
                flag_list[flag_counter++] = 1;
                break;
            case 'e':  // отображать $ в конце каждой строки
                flag_list[flag_counter++] = 2;
                break;
            case 'n':  // нумеровать все строки
                flag_list[flag_counter++] = 3;
                break;
            case 's':  // сжимать несколько пустых строк в одну
                flag_list[flag_counter++] = 4;
                break;
            case 't':  // отображать \t как ^I
                flag_list[flag_counter++] = 5;
                break;
            case 'a':  // = -e и -t
                flag_list[flag_counter++] = 6;
                break;
            default:
                printf("Unknown option '%c'\n", optopt);
                break;
        }
    }

    normalize_flags(flag_list, flag_counter, &deleted_flags);

    for (int i = optind; i < argc; i++) {
        if (file_proccess(argv[i], flag_list) != 0)
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
            } else
                i++;
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
            } else
                i++;
        }
    }
}

int file_proccess(const char* filename, int* flags) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("File %s does not exists.\n", filename);
        return 1;
    }

    int flag_b = 0, flag_e = 0, flag_n = 0, flag_s = 0, flag_t = 0;
    // переменные флагов
    for (int i = 0; i < MAX_ARGS; i++) {  // переносим флаги из списка в переменные
        switch (flags[i]) {
            case 1: flag_b = 1; break;
            case 2: flag_e = 1; break;
            case 3: flag_n = 1; break;
            case 4: flag_s = 1; break;
            case 5: flag_t = 1; break;
            case 6:  // -a = -e и -t
                flag_e = 1;
                flag_t = 1;
                break;
        }
    }

    char string[MAX_LINE_LENGTH];
    char prev_line[MAX_LINE_LENGTH] = "";
    int line_number = 1;

    while (fgets(string, MAX_LINE_LENGTH, file) != NULL) {
        // убираем символ новой строки
        string[strcspn(string, "\n")] = '\0';  // заменяет \n на \0
        // strcspn ищет первое вхождение элемента
        int is_empty_line = (strcmp(string, "\0") == 0);    //пустая ли строка
        if (flag_s && is_empty_line && strcmp(prev_line, "\0") == 0) {
            continue;
        }//скип пустой строки
        //обработка флагов
        if (flag_b) {       //нумеровать непустые строки
            if (!is_empty_line) {
                print_with_line_numbers(string, &line_number);
                printf("\n");
            } else {
                printf("\n");
            }
        } else if (flag_n) {    //нумеровать все строки
            print_with_line_numbers(string, &line_number);
            printf("\n");
        } else {    //ecли нет флагов нумерации то выводим строку
            printf("%s", string);
        }
        if (flag_e) {           //отображать $ в конце каждой строки
            printf("$\n");
        }
        if (flag_t) {       //отображать \t как ^I (и это тоже)
            for (char* p = string; *p != '\0'; p++) {
                if (*p == '\t')
                    printf("^I");
                else
                    putchar(*p);
            }
            printf("\n");
        }

        strcpy(prev_line, string);  //копируем предыдущую строку
    }

    fclose(file);
    return 0;
}

void print_with_line_numbers(const char* string, int* line_number) {
    printf("%6d\t%s", (*line_number)++, string);
}
