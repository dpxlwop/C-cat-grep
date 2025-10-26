typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
} flags;
void file_proccess(int argc, char** argv, flags flag_container, char** e_args,
                   int e_args_counter);
void output(char* line, char* filename, int filecounter, int line_number, int count_matching_str, flags flag_container, int need_to_print_line);
int search_in_line(const char* line, const char* pattern, int ignore_case,
                   char* result);
void free_mem(char** e_args, int e_args_counter);
