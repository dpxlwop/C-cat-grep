CFLAGS = -Wall -Werror -Wextra
BUILD_DIR = ../build

all: cat grep

cat: 
	gcc $(CFLAGS) cat/cat.c -o $(BUILD_DIR)/s21_cat

grep: 
	gcc $(CFLAGS) grep/grep.c -o $(BUILD_DIR)/s21_grep

test: test_cat test_grep

test_cat: cat
	mv ../build/s21_cat .
	mv test/test_s21_cat.sh .
	./test_s21_cat.sh
	rm s21_cat test_s21_cat.sh

test_grep: grep
	mv ../build/s21_grep .
	mv test/test_s21_grep.sh .
	./test_s21_grep.sh
	rm s21_grep test_s21_grep.sh

clean:
	rm -f $(BUILD_DIR)/*

rebuild: clean all

.PHONY: all cat grep clean rebuild test_cat test_grep