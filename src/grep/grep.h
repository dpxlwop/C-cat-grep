void normalize_flags(int* flags, int* count, int* ignore_case, int* invert_match);
void search_in_line(const char* line, const char* pattern, int ignore_case, int invert_match, char* result);
void output(const char* filename, int count_matching_str, int* flags, int flag_count);