CFLAGS = -Wall -Werror -Wextra -std=c11

all: s21_cat s21_grep

s21_cat: 
	gcc $(CFLAGS) cat/cat.c -o cat/s21_cat

s21_grep: 
	gcc $(CFLAGS) grep/grep.c -o grep/s21_grep

clean:
	rm -f cat/s_21cat grep/s21_grep

rebuild: clean all

.PHONY: all cat grep clean rebuild s21_grep s21_cat