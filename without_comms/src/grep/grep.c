#include "grep.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

int main(int argc, char** argv) {
  flags flag_container = {0};
  int e_args_counter = 0;
  char** e_args = calloc(e_args_counter * sizeof(char*), 0);
  if (e_args == NULL) {
    return 1;
  }
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <options> <pattern> <file1> [file2] ...\n",
            argv[0]);
    return 1;
  }
  int rez;
  while ((rez = getopt(argc, argv, "e:ivclnhsf:")) != -1) {
    switch (rez) {
      case 'e':
        flag_container.e = 1;
        get_arg_array(flag_container, e_args, &e_args_counter, argv);
        break;
      case 'i':
        flag_container.i = 1;
        break;
      case 'v':
        flag_container.v = 1;
        break;
      case 'c':
        flag_container.c = 1;
        break;
      case 'l':
        flag_container.l = 1;
        break;
      case 'n':
        flag_container.n = 1;
        break;
      case 'h':
        flag_container.h = 1;
        break;
      case 's':
        flag_container.s = 1;
        break;
      case 'f':
        flag_container.f = optind + 1;
        flag_container.e = 1;
        get_arg_array(flag_container, e_args, &e_args_counter, argv);
        break;
      case '?':
        fprintf(stderr, "Unknown option '%c'\n", optopt);
        free_mem(e_args, e_args_counter);
        return 1;
    }
  }
  if (flag_container.c || flag_container.l) flag_container.n = 0;
  file_proccess(argc, argv, flag_container, e_args, e_args_counter);
  free_mem(e_args, e_args_counter);
  return 0;
}

int get_arg_array(flags flag_container, char** e_args, int* e_args_counter,
                  char** argv) {
  if (flag_container.f != 0) {
    char line[MAX_LINE_LENGTH];
    FILE* file = fopen(optarg, "r");
    if (!file && !flag_container.s) {
      fprintf(stderr, "File %s does not exists.\n", argv[flag_container.f]);
      return 1;
    }
    while (fgets(line, sizeof(line), file) != NULL) {
      e_args = realloc(e_args, (*e_args_counter + 1) * sizeof(char*));
      if (e_args == NULL) {
        free_mem(e_args, *e_args_counter);
        printf("Error allocating memory\n");
        return 1;
      }
      e_args[*e_args_counter] =
          realloc(e_args[*e_args_counter], MAX_LINE_LENGTH);
      if (e_args[*e_args_counter] == NULL) {
        printf("Error allocating memory\n");
        free_mem(e_args, *e_args_counter);
        return 1;
      }
      line[strcspn(line, "\n")] = '\0';
      strcpy(e_args[*e_args_counter], line);
      (*e_args_counter)++;
    }
  } else if (flag_container.e) {
    e_args = realloc(e_args, (*e_args_counter + 1) * sizeof(char*));
    if (e_args == NULL) {
      free_mem(e_args, *e_args_counter);
      return 1;
    }
    e_args[*e_args_counter] = realloc(e_args[*e_args_counter], MAX_LINE_LENGTH);
    if (e_args[*e_args_counter] == NULL) {
      free_mem(e_args, *e_args_counter);
      return 1;
    }
    strcpy(e_args[*e_args_counter], optarg);
    (*e_args_counter)++;
  }
  return 0;
}

void file_proccess(int argc, char** argv, flags flag_container, char** e_args,
                   int e_args_counter) {
  int file_index;
  char pattern[MAX_LINE_LENGTH];
  if (flag_container.e || flag_container.f)
    file_index = optind;
  else
    file_index = optind + 1;
  for (int i = file_index; i < argc; i++) {
    int count_matching_str = 0, line_number = 0;
    FILE* file = fopen(argv[i], "r");
    if (!file && !flag_container.s) {
      fprintf(stderr, "File %s does not exists.\n", argv[i]);
      return;
    }
    if (flag_container.s && !file) {
      if (i++ != argc) continue;
    }
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
      line_number++;
      int match = 0;
      char result[MAX_LINE_LENGTH] = {0};
      if (flag_container.e) {
        for (int k = 0; k < e_args_counter; k++)
          if ((match = search_in_line(line, e_args[k], flag_container,
                                      result)) == 1) {
            strcpy(pattern, e_args[k]);
            k = e_args_counter;
          }
      } else {
        match = search_in_line(line, argv[optind], flag_container, result);
        strcpy(pattern, argv[optind]);
      }
      if (flag_container.v) match = !match;
      if (match) {
        count_matching_str++;
        if (!flag_container.c && !flag_container.l)
          output(line, argv[i], argc - file_index, line_number,
                 count_matching_str, flag_container, 1);
      }
    }
    output(NULL, argv[i], argc - file_index, line_number, count_matching_str,
           flag_container, 0);
    fclose(file);
  }
}

void output(char* line, char* filename, int filecounter, int line_number,
            int count_matching_str, flags flag_container,
            int need_to_print_line) {
  if (need_to_print_line) {
    if (filecounter == 1)
      if (flag_container.n)
        printf("%d:%s", line_number, line);
      else
        printf("%s", line);
    else if (flag_container.n && !flag_container.h)
      printf("%s:%d:%s", filename, line_number, line);
    else if (flag_container.n && flag_container.h)
      printf("%d:%s", line_number, line);
    else if (flag_container.h)
      printf("%s", line);
    else
      printf("%s:%s", filename, line);
  } else {
    if (flag_container.c && flag_container.l && count_matching_str >= 1)
      count_matching_str = 1;
    if (flag_container.c && filecounter == 1)
      printf("%d\n", count_matching_str);
    else if (flag_container.c && flag_container.h)
      printf("%d\n", count_matching_str);
    else if (flag_container.c)
      printf("%s:%d\n", filename, count_matching_str);
    if (flag_container.l && count_matching_str > 0) printf("%s\n", filename);
  }
}

void free_mem(char** e_args, int e_args_counter) {
  for (int j = 0; j < e_args_counter; j++) {
    free(e_args[j]);
  }
  free(e_args);
}

int search_in_line(const char* line, const char* pattern, flags flag_container,
                   char* result) {
  int match = 0;
  result[0] = '\0';
  if (flag_container.i) {
    match = (strcasestr(line, pattern) != NULL);
  } else {
    match = (strstr(line, pattern) != NULL);
  }
  return match;
}
