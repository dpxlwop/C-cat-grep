typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
} flags;
void file_proccess(int argc, char** argv, flags flag_container, char** e_args,
                   int e_args_counter);
int search_in_line(const char* line, const char* pattern, int ignore_case,
                   char* result);
void free_mem(char** e_args, int e_args_counter);
