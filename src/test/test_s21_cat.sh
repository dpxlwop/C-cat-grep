#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

S21_CAT="./s21_cat"
SYS_CAT="cat"

INT="input.txt"
OUT1="output1.txt"
OUT2="output2.txt"

passed=0
failed=0

run_test() {
    echo "\nHello\tworld\t\n\t\t\n\n\ntest test\t\nbye12345678Invisible chars testEND" > "$INT"
	local cmd="$1"
    echo "Testing: $cmd ... "

    # Заменяем './s21_cat' на 'cat', чтобы вызвать системную утилиту
    local sys_cmd="${cmd#./}"  # Убирает './' в начале строки, если есть
    sys_cmd="${sys_cmd/s21_cat/cat}"

    # Выполняем обе команды
    eval "$cmd" > "$OUT1"
	eval "$sys_cmd" > "$OUT2"

    # Сравниваем вывод
    if diff -q "$OUT1" "$OUT2" > /dev/null; then
        echo -e "${GREEN}OK${NC}"
        passed=$((passed + 1))
    else
        echo -e "${RED}FAIL${NC}"
        failed=$((failed + 1))
    fi
}

run_test_gnu() {
	echo "\nHello\tworld\t\n\t\t\n\n\ntest test\t\nbye" > "$INT"
    local gnu_cmd="$1"
    local native_flag="$2"
    echo "Testing GNU [$gnu_cmd] vs native [$native_flag] ... "

    # Выполняем обе команды
    eval "$gnu_cmd" > "$OUT1"
    eval "$SYS_CAT $native_flag $INT" > "$OUT2"

    # Сравниваем вывод
    if diff -q "$OUT1" "$OUT2" > /dev/null; then
        echo -e "${GREEN}OK${NC}"
        passed=$((passed + 1))
    else
        echo -e "${RED}FAIL${NC}"
        failed=$((failed + 1))
    fi
}

run_test "$S21_CAT -b $INT"
run_test "$S21_CAT -e $INT"
run_test "$S21_CAT -n $INT"
run_test "$S21_CAT -s $INT"
run_test "$S21_CAT -t $INT"
run_test "$S21_CAT -v $INT"
run_test "$S21_CAT -be $INT"
run_test "$S21_CAT -en $INT"
run_test "$S21_CAT -nt $INT"
run_test "$S21_CAT -st $INT"
run_test "$S21_CAT -bstn $INT"
run_test "$S21_CAT -btse $INT"
run_test "$S21_CAT -ntv $INT"
run_test_gnu "$S21_CAT --number-nonblank $INT" "-b"
run_test_gnu "$S21_CAT -E $INT" "-e"
run_test_gnu "$S21_CAT --number $INT" "-n"
run_test_gnu "$S21_CAT --squeeze-blank $INT" "-s"
run_test_gnu "$S21_CAT -T $INT" "-t"

rm -f "$INT" "$OUT1" "$OUT2"

echo -e "\nПройдено: ${GREEN}$passed${NC}"
echo -e "Провалено: ${RED}$failed${NC}\n"

if [ "$failed" -eq 0 ]; then
    exit 0
else
    exit 1
fi