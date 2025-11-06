#include "cat.h"

int main(int argc, char **argv) {
  flags flag_container = {0};
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <options> <file1> [file2] ...\n", argv[0]);
    return 1;
  }
  int rez = 0;
  int option_index;
  while ((rez = getopt_long(argc, argv, "+benstvET", long_options,
                            &option_index)) != -1) {
    switch (rez) {
      case 'b':
        flag_container.b = 1;
        break;
      case 'e':
        flag_container.v = 1;
        /* FALL THROUGH */
      case 'E':
        flag_container.e = 1;
        break;
      case 'n':
        flag_container.n = 1;
        break;
      case 's':
        flag_container.s = 1;
        break;
      case 't':
        flag_container.v = 1;
        /* FALL THROUGH */
      case 'T':
        flag_container.t = 1;
        break;
      case 'v':
        flag_container.v = 1;
        break;
      default:
        fprintf(stderr, "Unknown option '%c'\n", optopt);
        return 1;
    }
  }
  if (flag_container.b == 1 && flag_container.n == 1) flag_container.n = 0;
  for (int i = optind; i < argc; i++) file_proccess(argv[i], flag_container);
  return 0;
}

int file_proccess(const char *filename, flags flag_container) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "File %s does not exists.\n", filename);
    return 1;
  }
  int is_new_line = 1, empty_streak = 0, line_number = 1;
  int c;
  while ((c = fgetc(file)) != EOF) {
    int is_empty_line = (is_new_line && c == '\n');
    if (flag_container.s && is_empty_line) {
      if (empty_streak) continue;
      empty_streak = 1;
    } else
      empty_streak = 0;
    int need_number = (((flag_container.b && !is_empty_line) ||
                        (flag_container.n && !flag_container.b)) &&
                       is_new_line);
    if (need_number) printf("%6d\t", line_number++);
    if (c != '\n') {
      print_visible_char(c, flag_container);
      is_new_line = 0;
    } else {
      if (flag_container.e) {
        if (!flag_container.n && flag_container.b && is_empty_line) {
          printf("      	$");
        } else {
          printf("$");
        }
      }
      putchar('\n');
      is_new_line = 1;
    }
  }
  fclose(file);
  return 0;
}

void print_visible_char(unsigned char c, flags flag_container) {
  if (flag_container.t && c == '\t') {
    printf("^I");
  } else if (flag_container.v && c != 9) {
    if (c >= 32 && c < 127) {
      putchar(c);
    } else if (c < 32) {
      printf("^%c", c + 64);
    } else if (c == 127) {
      printf("^?");
    } else {
      printf("M-");
    }
  } else {
    putchar(c);
  }
}