#include <getopt.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

struct option long_options[] = {{"number-nonblank", no_argument, 0, 'b'},
                                {"number", no_argument, 0, 'n'},
                                {"squeeze-blank", no_argument, 0, 's'},
                                {0, 0, 0, 0}};

typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} flags;

int file_proccess(const char *filename, flags flag_container);
void replace_symbols(char *string, int flag_v, int flag_t);
void print_visible_char(unsigned char c, flags flag_container);