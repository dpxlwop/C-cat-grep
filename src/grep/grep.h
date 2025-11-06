#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
} flags;

void file_proccess(int argc, char **argv, flags flag_container, char **e_args,
                   int e_args_counter);
void output(char *line, char *filename, int filecounter, int line_number,
            int count_matching_str, flags flag_container,
            int need_to_print_line);
int search_in_line(const char *line, const char *pattern, flags flag_container,
                   char *result);
void free_mem(char **e_args, int e_args_counter);
char **get_arg_array(flags flag_container, char **e_args, int *e_args_counter);
