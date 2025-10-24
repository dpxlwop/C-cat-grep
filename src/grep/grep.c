#include "grep.h"

#include <getopt.h>
#include <stdio.h>
#include <string.h>

#define MAX_ARGS 10
#define MAX_LINE_LENGTH 1024

// если есть -c или -l то -n игнорируюется.

int main(int argc, char** argv) {
    int ignore_case = 0, flag_e = 0, invert_match = 0, print_names_only = 0, print_counter_only = 0,
        show_line_numbers = 0;
    char result[MAX_LINE_LENGTH];
    char e_arg[MAX_LINE_LENGTH];
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <options> <pattern> <file1> [file2] ...\n", argv[0]);
        return 1;
    }
    int rez;

    while ((rez = getopt(argc, argv, "e:ivcln")) != -1) {
        switch (rez) {
            case 'e':  // шаблон
                flag_e = 1;
                strcpy(e_arg, optarg);
                break;
            case 'i':  // игнор регистра
                ignore_case = 1;
                break;
            case 'v':  // инвертировать поиск
                invert_match = 1;
                break;
            case 'c':  // количество совпадающих строк
                print_counter_only = 1;
                break;
            case 'l':  // только имена файлов с совпадениями
                print_names_only = 1;
                break;
            case 'n':  // номер строки
                show_line_numbers = 1;
                break;
            case '?':
                fprintf(stderr, "Unknown option '%c'\n", optopt);
                return 1;
        }
    }
    int pattern_index = -1; // индекс шаблона в argv
    if (!flag_e)
        pattern_index = optind++;          // первый аргумент — шаблон
            
    for (int i = optind; i < argc; i++) {
        int count_matching_str = 0, line_number = 0;
        FILE* file = fopen(argv[i], "r");
        if (!file) {
            fprintf(stderr, "File %s does not exists.\n", argv[i]);
            return 1;
        }
        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file) != NULL) {  // читаем строку
            line_number++;
            if (flag_e) {
                search_in_line(line, e_arg, ignore_case, invert_match, result);  // ищем в ней шаблон из -e
            } else {
                search_in_line(line, argv[pattern_index], ignore_case, invert_match,
                               result);  // ищем в ней шаблон из аргументов
            }
            if (result[0] != '\0') {  // если нашли, то добавляем счетчик совпадений
                count_matching_str++;
                if (print_counter_only == 0 && print_names_only == 0) {  // проверка на флаги -c и -l
                    if (show_line_numbers) printf("%d ", line_number);   // если нужно печать номер строки
                    printf("%s", line);                                  // выводим найденную строку
                }
            }
        }
        if (print_counter_only == 1 && count_matching_str > 0) {  // вывод если флаги -c или -l
            printf("%s: %d\n", argv[i], count_matching_str);
        } else if (print_names_only == 1 && count_matching_str > 0) {
            printf("%s\n", argv[i]);
        }
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
