CFLAGS = -Wall -Werror -Wextra -std=c11 -D_GNU_SOURCE

all: s21_grep

s21_grep: clean
	gcc $(CFLAGS) grep.c -o s21_grep

test_grep: rebuild
	./test/test_s21_grep.sh

clean:
	rm -f s21_grep

rebuild: clean all

.PHONY: all clean rebuild s21_grep test_grep