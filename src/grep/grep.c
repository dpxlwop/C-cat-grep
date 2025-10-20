#include "grep.h"

#include <getopt.h>
#include <stdio.h>
#include <string.h>

#define MAX_ARGS 10
#define MAX_LINE_LENGTH 1024

// максимально сырой, вывод работает через очко, но ищет все корректно

// если есть -c или -l то -n, -h, -o игнорируются.
//-n > -h > -o

int main(int argc, char** argv) {
    int flag_list[MAX_ARGS] = {0}, flag_counter = 0, ignore_case = 0, invert_match = 0,
        count_matching_str = 0;
    char result[MAX_LINE_LENGTH];
    if (argc < 2) {
        printf("Usage: %s <options> <pattern> <file1> [file2] ...\n", argv[0]);
        return 1;
    }
    int rez;
    while ((rez = getopt(argc, argv, "eivclnhsfo")) != -1) {
        switch (rez) {
            case 'e':  // шаблон
                flag_list[flag_counter++] = 1;
                break;
            case 'i':  // игнор регистра
                flag_list[flag_counter++] = 2;
                break;
            case 'v':  // инвертировать поиск
                flag_list[flag_counter++] = 3;
                break;
            case 'c':  // количество совпадающих строк
                flag_list[flag_counter++] = 4;
                break;
            case 'l':  // только имена файлов с совпадениями
                flag_list[flag_counter++] = 5;
                break;
            case 'n':  // номер строки
                flag_list[flag_counter++] = 6;
                break;
            case 'h':  // без имен файлов
                flag_list[flag_counter++] = 7;
                break;
            case 's':  // подавить ошибки
                flag_list[flag_counter++] = 8;
                break;
            case 'f':  // файл с шаблонами
                flag_list[flag_counter++] = 9;
                break;
            case 'o':  // только совпадающие части строки
                flag_list[flag_counter++] = 10;
                break;
            case '?':
                return 1;
        }
    }

    normalize_flags(flag_list, &flag_counter, &ignore_case, &invert_match);

    for (int i = optind + 1; i < argc; i++) {
        count_matching_str = 0;
        printf("Processing file: %s\n", argv[i]);
        FILE* file = fopen(argv[i], "r");
        if (!file) {
            printf("File %s does not exists.\n", argv[i]);
            return 1;
        }
        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file) != NULL) {
            search_in_line(line, argv[optind], ignore_case, invert_match, result);
            if (result[0] != '\0') {
                count_matching_str++;
            }
        }
        output(argv[i], count_matching_str, flag_list, flag_counter);
    }
    return 0;
}

void normalize_flags(int* flags, int* count, int* ignore_case, int* invert_match) {
    int has_c = 0, has_l = 0, has_n = 0, has_h = 0, has_o = 0;

    for (int i = 0; i < *count; i++) {  // проверяем наличие флагов
        switch (flags[i]) {
            case 4:
                has_c = 1;
                break;
            case 5:
                has_l = 1;
                break;
            case 6:
                has_n = 1;
                break;
            case 7:
                has_h = 1;
                break;
            case 10:
                has_o = 1;
                break;
        }
    }

    // если есть -c или -l
    if (has_c || has_l) {
        for (int i = 0; i < *count;) {
            if (flags[i] == 6 || flags[i] == 7 || flags[i] == 10) {
                for (int j = i; j < *count - 1; j++) flags[j] = flags[j + 1];
                (*count)--;
            } else {
                i++;
            }
        }
        return;
    }

    // приоритет вывода: -n > -h > -o
    if (has_n && has_h) {
        // удалить -h, оставить -n
        for (int i = 0; i < *count;) {
            if (flags[i] == 7) {
                for (int j = i; j < *count - 1; j++) flags[j] = flags[j + 1];
                (*count)--;
            } else
                i++;
        }
    }
    if (has_h && has_o) {
        // удалить -o, оставить -h
        for (int i = 0; i < *count;) {
            if (flags[i] == 10) {
                for (int j = i; j < *count - 1; j++) flags[j] = flags[j + 1];
                (*count)--;
            } else
                i++;
        }
    }
    for (int i = 0; i < MAX_ARGS; i++) {  // переносим флаги поиска из списка в переменные
        switch (flags[i]) {
            case 2:
                (*ignore_case) = 1;
                break;
            case 3:
                (*invert_match) = 1;
                break;
        }
    }
}

void search_in_line(const char* line, const char* pattern, int ignore_case, int invert_match, char* result) {
    int match = 0;
    result[0] = '\0';  // очищаем результат
    if (ignore_case) {
        match = (strcasestr(line, pattern) != NULL);  // ищем строку без учета регистра
    } else {
        match = (strstr(line, pattern) != NULL);  // ищем строку с учетом регистра
    }
    if (invert_match) {
        match = !match;
    }
    if (match) {
        strcpy(result, line);
    }
}

void output(const char* filename, int count_matching_str, int* flags, int flag_count) {
    int has_c = 0, has_l = 0;
    for (int i = 0; i < flag_count; i++) {
        if (flags[i] == 4) has_c = 1;
        if (flags[i] == 5) has_l = 1;
    }
    if (has_c) {
        printf("%s: %d\n", filename, count_matching_str);
    }
    if (has_l && count_matching_str > 0) {
        printf("%s\n", filename);
    }
}