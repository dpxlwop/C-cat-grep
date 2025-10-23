int normalize_flags(int *flags, int count);
int file_proccess(const char *filename, int *flags, int flag_count);
void print_with_line_numbers(char *string, int *line_number, int flag_t, int flag_v);
void replace_symbols(char *string, int flag_v, int flag_t);
void print_visible_char(unsigned char c);
