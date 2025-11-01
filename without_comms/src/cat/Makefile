CFLAGS = -Wall -Werror -Wextra -std=c11 -D_GNU_SOURCE

all: s21_cat 

s21_cat: clean
	gcc $(CFLAGS) cat.c -o s21_cat

test_cat: rebuild
	./test/test_s21_cat.sh

clean:
	rm -f s21_cat

rebuild: clean all

.PHONY: all clean rebuild s21_cat test_cat