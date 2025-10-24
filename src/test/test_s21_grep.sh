#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

S21_GREP="./s21_grep"
SYS_GREP="grep"

INT="input.txt"
INT2="input2.txt"
OUT1="output1.txt"
OUT2="output2.txt"

passed=0
failed=0

cat > "$INT" << EOF
Hello World
hello world
This is a test.
Another line with HELLO.
End of file.
EOF

cp "$INT" "$INT2"

run_test() {
    local options="$1"
    local pattern="$2"
    local files="$3"
    echo "Testing: $S21_GREP $options '$pattern' $files ..."

    # Выполняем обе команды
    $S21_GREP $options "$pattern" $files > "$OUT1"
    $SYS_GREP $options "$pattern" $files > "$OUT2"

    # Сравниваем вывод
    if diff -q "$OUT1" "$OUT2" > /dev/null; then
        echo -e "${GREEN}OK${NC}"
        passed=$((passed + 1))
    else
        echo -e "${RED}FAIL${NC}"
        echo "Различия в выводе:"
        diff "$OUT1" "$OUT2"
        failed=$((failed + 1))
    fi
}
echo "\n--- Одиночные флаги ---"
run_test "-e" "hello" "$INT"
run_test "-i" "HELLO" "$INT"
run_test "-v" "hello" "$INT"
run_test "-c" "hello" "$INT"
run_test "-l" "hello" "$INT"
run_test "-n" "hello" "$INT"
#run_test "-h" "hello" "$INT"
#run_test "-s" "xyz" "$INT"
run_test "" "hello" "$INT"

echo "\n--- Комбинации флагов ---"
run_test "-iv" "HELLO" "$INT"
run_test "-in" "HELLO" "$INT"
run_test "-vc" "hello" "$INT"
run_test "-nc" "hello" "$INT"
#run_test "-ih" "HELLO" "$INT"
run_test "-cv" "hello" "$INT"
run_test "-lv" "hello" "$INT"
run_test "-ivn" "HELLO" "$INT"
run_test "-ivc" "HELLO" "$INT"
run_test "-ivl" "HELLO" "$INT"
#run_test "-ivh" "HELLO" "$INT"
#run_test "-ivs" "HELLO" "$INT"
run_test "-inc" "HELLO" "$INT"
run_test "-inl" "HELLO" "$INT"
#run_test "-inh" "HELLO" "$INT"
#run_test "-ins" "HELLO" "$INT"
#run_test "-vch" "hello" "$INT"
#run_test "-vcs" "hello" "$INT"
#run_test "-nch" "hello" "$INT"
#run_test "-ncs" "hello" "$INT"
run_test "-ivnc" "HELLO" "$INT"
run_test "-ivnl" "HELLO" "$INT"
#run_test "-ivnh" "HELLO" "$INT"
#run_test "-ivns" "HELLO" "$INT"
#run_test "-ivch" "HELLO" "$INT"
#run_test "-ivcs" "HELLO" "$INT"
#run_test "-inch" "HELLO" "$INT"
#run_test "-incs" "HELLO" "$INT"
#run_test "-vchs" "hello" "$INT"
#run_test "-nchs" "hello" "$INT"
#run_test "-ivnch" "HELLO" "$INT"
#run_test "-ivncs" "HELLO" "$INT"
#run_test "-ivchs" "HELLO" "$INT"
#run_test "-inchs" "HELLO" "$INT"

echo "\n--- Много паттернов через -e ---"
run_test "-e hello -e HELLO" "$INT"
run_test "-i -e hello -e WORLD" "$INT"
run_test "-v -e hello -e test" "$INT"

echo "\n--- Работа с несколькими файлами ---"
run_test "-e" "hello" "$INT $INT2"
run_test "-c -i" "hello" "$INT $INT2"
#run_test "-h -v" "test" "$INT $INT2"

rm -f "$INT" "$INT2" "$OUT1" "$OUT2"

echo -e "\nПройдено: ${GREEN}$passed${NC}"
echo -e "Провалено: ${RED}$failed${NC}\n"

if [ "$failed" -eq 0 ]; then
    exit 0
else
    exit 1
fi