CFLAGS = -Wall -Werror -Wextra
BUILD_DIR = ../build

all: cat grep

cat: 
	gcc $(CFLAGS) cat/cat.c -o $(BUILD_DIR)/s21_cat

grep: 
	gcc $(CFLAGS) grep/grep.c -o $(BUILD_DIR)/s21_grep

clean:
	rm -f $(BUILD_DIR)/*

rebuild: clean all

.PHONY: all cat grep clean rebuild