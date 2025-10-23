void normalize_flags(int *flags, int count, int *deleted_flags);
int file_proccess(const char *filename, int *flags);
void print_with_line_numbers(const char *string, int *line_number);
void print_with_line_numbers_non_empty(const char *string, int *line_number);
void print_dollar_at_end();
void replace_tab(char *string);