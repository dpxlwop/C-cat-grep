#include "grep.h"

#include <getopt.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

// если есть -c или -l то -n игнорируюется.

int main(int argc, char** argv) {
    int flag_i = 0, flag_e = 0, flag_v = 0, flag_l = 0, flag_c = 0, flag_n = 0;
    char result[MAX_LINE_LENGTH];
    char e_arg[MAX_LINE_LENGTH];    //заменить на двумерный массив malloc
    int earg_counter = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <options> <pattern> <file1> [file2] ...\n", argv[0]);
        return 1;
    }
    int rez;

    while ((rez = getopt(argc, argv, "e:ivcln")) != -1) {
        switch (rez) {
            case 'e':  // шаблон
                flag_e = 1;
                //расширение через realloc
                strcpy(e_arg[earg_counter++], optarg);
                break;
            case 'i':  // игнор регистра
                flag_i = 1;
                break;
            case 'v':  // инвертировать поиск
                flag_v = 1;
                break;
            case 'c':  // количество совпадающих строк
                flag_c = 1;
                break;
            case 'l':  // только имена файлов с совпадениями
                flag_l = 1;
                break;
            case 'n':  // номер строки
                flag_n = 1;
                break;
            case '?':
                fprintf(stderr, "Unknown option '%c'\n", optopt);
                return 1;
        }
    }
    for (int i = optind; i < argc; i++) {
        int count_matching_str = 0, line_number = 0;
        FILE* file = fopen(argv[i], "r");
        if (!file) {
            fprintf(stderr, "File %s does not exists.\n", argv[i]);
            return 1;
        }
        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file) != NULL) {
            line_number++;
            int match = 0;
            char result[MAX_LINE_LENGTH] = {0};
            if (flag_e) {       //если есть -e
                for (int k = 0; k < earg_counter; k++) {
                    search_in_line(line, e_arg[k], flag_i, flag_v, result);
                    if (result[0] != '\0') {    //смотрим по всем шаблонам, если хотя бы один совпадет - выходим из цикла
                        match = 1;
                        k = earg_counter;
                    }
                }
            } else {    //обычный поиск
                search_in_line(line, argv[pattern_index], flag_i, flag_v, result);
                if (result[0] != '\0')
                    match = 1;
            }
            if (match) {
                count_matching_str++;
                if (!flag_c && !flag_l) {   //если совпадение, то вывод по флагам
                    if (flag_n) printf("%d:", line_number);
                    printf("%s", line);
                }
            }
        }
        if (flag_c)     //вывод чисто количества строк
            printf("%s:%d\n", argv[i], count_matching_str);
        if (flag_l && count_matching_str > 0)       //вывод только имен файлов
            printf("%s\n", argv[i]);
        fclose(file);
    }
    return 0;
    }

void search_in_line(const char* line, const char* pattern, int ignore_case, int invert_match, char* result) {
    int match = 0;
    result[0] = '\0';  // очищаем результат
    if (ignore_case) {
        match = (strcasestr(line, pattern) != NULL);  // ищем строку без учета регистра
    } else {
        match = (strstr(line, pattern) != NULL);  // ищем строку с учетом регистра
    }
    if (invert_match) {  // флаг -v
        match = !match;
    }
    if (match) {
        strcpy(result, line);  // копируем строку
    }
}
