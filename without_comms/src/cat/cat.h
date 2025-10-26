typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} flags;

int file_proccess(const char* filename, flags flag_container);
void replace_symbols(char* string, int flag_v, int flag_t);
void print_visible_char(unsigned char c);
