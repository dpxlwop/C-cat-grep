#include "grep.h"

#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1024

// если есть -c или -l то -n игнорируюется.

int main(int argc, char** argv) {
    flags flag_container = {0};
    int e_args_counter = 0;
    //char e_arg[MAX_LINE_LENGTH];    //заменить на двумерный массив malloc
    char **e_args = (char **)malloc(e_args_counter * sizeof(char *));
    if (e_args == NULL) {
        // Обработка ошибки выделения памяти
        return 1;
    }
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <options> <pattern> <file1> [file2] ...\n", argv[0]);
        return 1;
    }
    int rez;
    while ((rez = getopt(argc, argv, "e:ivcln")) != -1) {
        switch (rez) {
            case 'e':  // шаблон
                flag_container.e = 1;
                e_args = realloc(e_args, (e_args_counter + 1) * sizeof(char *));
                e_args[e_args_counter] = realloc(e_args[e_args_counter], MAX_LINE_LENGTH);
                if (e_args == NULL || e_args[e_args_counter] == NULL) {
                    free_mem(e_args, e_args_counter);
                    return 1;
                }
                strcpy(e_args[e_args_counter], optarg);
                e_args_counter++;
                break;
            case 'i':  // игнор регистра
                flag_container.i = 1;
                break;
            case 'v':  // инвертировать поиск
                flag_container.v = 1;
                break;
            case 'c':  // количество совпадающих строк
                flag_container.c = 1;
                break;
            case 'l':  // только имена файлов с совпадениями
                flag_container.l = 1;
                break;
            case 'n':  // номер строки
                flag_container.n = 1;
                break;
            case '?':
                fprintf(stderr, "Unknown option '%c'\n", optopt);
                free_mem(e_args, e_args_counter);
                return 1;
        }
    }
    if (flag_container.c || flag_container.l)
        flag_container.n = 0;
    file_proccess(argc, argv, flag_container, e_args, e_args_counter);
    free_mem(e_args, e_args_counter);
    return 0;
}
    
void file_proccess(int argc, char** argv, flags flag_container, char** e_args, int e_args_counter) {
    int file_index;
    if (flag_container.e)
        file_index = optind;
    else 
        file_index = optind + 1;
    for (int i = file_index; i < argc; i++) {
        int count_matching_str = 0, line_number = 0;
        FILE* file = fopen(argv[i], "r");
        if (!file) {
            fprintf(stderr, "File %s does not exists.\n", argv[i]);
            free_mem(e_args, e_args_counter);
            return;
        }
        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file) != NULL) {
            line_number++;
            int match = 0;
            char result[MAX_LINE_LENGTH] = {0};
            if (flag_container.e) {   //если есть -e
                for (int k = 0; k < e_args_counter; k++) {
                    if ((match = search_in_line(line, e_args[k], flag_container.i, result)) == 1) {
                        k = e_args_counter;
                    }
                }
                if (flag_container.v) match = !match;
            } else {    //обычный поиск
                match = search_in_line(line, argv[optind], flag_container.i, result);
                if (flag_container.v) match = !match;
            }
            if (match) {
                count_matching_str++;
                if (!flag_container.c && !flag_container.l) {   //если совпадение, то вывод по флагам
                    if (flag_container.n) printf("%d:", line_number);
                    if (argc - file_index == 1)
                        printf("%s", line);
                    else
                        printf("%s:%s", argv[i], line);
                }
            }
        }
        if (flag_container.c && argc - file_index == 1)     //вывод чисто количества строк, без имени файла, если один файл
            printf("%d\n", count_matching_str);
        else if (flag_container.c)                          //вывод чисто количества строк с именем
            printf("%s:%d\n", argv[i], count_matching_str);
        if (flag_container.l && count_matching_str > 0)       //вывод только имен файлов
            printf("%s\n", argv[i]);
        fclose(file);
    }
}
void free_mem(char **e_args, int e_args_counter) {
    for (int j = 0; j < e_args_counter; j++) {
        free(e_args[j]);
    }
    free(e_args);
}

int search_in_line(const char* line, const char* pattern, int ignore_case, char* result) {
    int match = 0;
    result[0] = '\0';  // очищаем результат
    if (ignore_case) {
        match = (strcasestr(line, pattern) != NULL);  // ищем строку без учета регистра
    } else {
        match = (strstr(line, pattern) != NULL);  // ищем строку с учетом регистра
    }
    strcpy(result, line);  // копируем строку
    return match;
}
