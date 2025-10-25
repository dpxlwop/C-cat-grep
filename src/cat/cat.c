#include "cat.h"

#include <getopt.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

struct option long_options[] = {
    // прописываем зависимость длинных флагов к коротким
    {"number-nonblank", no_argument, 0, 'b'},
    {"number", no_argument, 0, 'n'},
    {"squeeze-blank", no_argument, 0, 's'},
    {0, 0, 0, 0}  // требуется для getopt_long
};

int main(int argc, char** argv) {
    flags flag_container={0};
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <options> <file1> [file2] ...\n", argv[0]);
        return 1;
    }
    int rez = 0;
    int option_index;  // индекс выбранной опции - нужен для getopt_long, но не используется
    while ((rez = getopt_long(argc, argv, "benstvET", long_options, &option_index)) !=
           -1) {  // getopt парсит флаги из argv
        switch (rez) {
            case 'b':  // нумеровать непустые строки
                flag_container.b = 1;
                break;
            case 'e':  // отображать $ в конце каждой строки
                flag_container.v = 1;       //включает -v
                /* FALL THROUGH */
            case 'E':
                flag_container.e = 1;
                break;
            case 'n':  // нумеровать все строки
                flag_container.n = 1;
                break;
            case 's':  // сжимать несколько пустых строк в одну
                flag_container.s = 1;
                break;
            case 't':  // отображать \t как ^I
                flag_container.v = 1;       //включает -v
                 /* FALL THROUGH */
            case 'T':
                flag_container.t = 1;
                break;
            case 'v':  // отображать скрытые символы
                flag_container.v = 1;
                break;
            default:
                fprintf(stderr, "Unknown option '%c'\n", optopt);
                return 1;
        }
    }
    if (flag_container.b == 1 && flag_container.n == 1) flag_container.n = 0;
    // optind указывает на первый аргумент после флагов
    for (int i = optind; i < argc; i++) file_proccess(argv[i], flag_container);
    return 0;
}

int file_proccess(const char* filename, flags flag_container) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "File %s does not exists.\n", filename);
        return 1;
    }
    char string[MAX_LINE_LENGTH];
    int line_number = 1;
    int empty_streak = 0;  // флаг для подряд идущих пустых строк

    while (fgets(string, MAX_LINE_LENGTH, file) != NULL) {
        int is_empty_line = (string[0] == '\n');  // пустая ли строка
        if (flag_container.s && is_empty_line) {    //s
            if (empty_streak) continue;
            empty_streak = 1;
            } else {
                empty_streak = 0;
            }
        int need_number = (flag_container.b && !is_empty_line) || (flag_container.n && !flag_container.b);
        if (need_number) {      //печать номера, если совпадают условия
            printf("%6d\t", line_number++);
        }
        replace_symbols(string, flag_container.v, flag_container.t);    //вывод строки
        if (flag_container.e) {     //e
            printf("$");
        }
        printf("\n");
    }
    fclose(file);
    return 0;
}


void replace_symbols(char *string, int flag_v, int flag_t) {
    for (unsigned char *p = (unsigned char *)string; *p != '\n'; p++) {     //идем по строке
        if (*p == '\t' && flag_t)
            printf("^I");       //t
        else if (flag_v)    //v
            print_visible_char(*p);
        else               //просто выводим, если нет флагов
            putchar(*p);
    }
}

void print_visible_char(unsigned char c) {
    if (c >= 32 && c < 127)   //обычные символы
        putchar(c);
    else if (c == 9 || c == 10)  // таб или \n — обычные
        putchar(c);
    else if (c < 32)    //управляющие символы типо \0 EOF и тд
        printf("^%c", c + 64);  // ^A–^Z
    else if (c == 127)
        printf("^?");
    else {  // обработка не ascii символов 
        printf("M-");
        if (c >= 160)   
            putchar(c - 128);
        else
            print_visible_char(c - 128);
    }
}