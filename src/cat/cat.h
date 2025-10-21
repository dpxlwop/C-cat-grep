int normalize_flags(int *flags, int count);
int file_proccess(const char *filename, int *flags, int flag_count);
void print_with_line_numbers(const char *string, int *line_number);
void print_with_line_numbers_non_empty(const char *string, int *line_number);
void replace_tab(char *string);