#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

S21_GREP="./s21_grep"
SYS_GREP="grep"

INT="input.txt"
INT2="input2.txt"
PATTERN_FILE="patterns.txt"
OUT1="output1.txt"
OUT2="output2.txt"
LOG_FILE="failures.log"

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
cat > "$PATTERN_FILE" << EOF
hello
test
EOF

run_test() {
    local description="$1"
    local cmd1="$2"
    local cmd2="$3"

    echo "Testing: $description ..."

    eval "$cmd1" > "$OUT1" 2>/dev/null || true
    eval "$cmd2" > "$OUT2" 2>/dev/null || true

    if diff -q "$OUT1" "$OUT2"; then
        echo -e "${GREEN}OK${NC}"
        passed=$((passed + 1))
    else
        echo -e "${RED}FAIL${NC}"
        failed=$((failed + 1))
        echo "FAIL" >> "$LOG_FILE"
        echo "Command: $cmd1" >> "$LOG_FILE"
        echo "Expected: $cmd2" >> "$LOG_FILE"
        echo "Diff output:"  >> "$LOG_FILE"
        diff "$OUT1" "$OUT2" >> "$LOG_FILE"
        echo "---" >> "$LOG_FILE"
    fi
}

test_with_e() {
    local flags="$1"
    local files="$2"
    local desc="$S21_GREP $flags -e 'hello' $files"
    local cmd1="$S21_GREP $flags -e 'hello' $files"
    local cmd2="$SYS_GREP $flags -e 'hello' $files"
    run_test "$desc" "$cmd1" "$cmd2"
}

test_with_f() {
    local flags="$1"
    local files="$2"
    local desc="$S21_GREP $flags -f $PATTERN_FILE $files"
    local cmd1="$S21_GREP $flags -f $PATTERN_FILE $files"
    local cmd2="$SYS_GREP $flags -f $PATTERN_FILE $files"
    run_test "$desc" "$cmd1" "$cmd2"
}

generate_flag_combinations() {
    local base_flags=("-i" "-v" "-c" "-l" "-n" "-h" "-s")
    local n=${#base_flags[@]}
    local total=$((1 << n))

    for ((mask = 0; mask < total; mask++)); do
        local combo=""
        for ((j = 0; j < n; j++)); do
            if (( (mask >> j) & 1 )); then
                combo+="${base_flags[j]} "
            fi
        done
        combo=$(echo "$combo" | sed 's/ $//')

        test_with_e "$combo" "$INT"
        test_with_e "$combo" "$INT $INT2"

        test_with_f "$combo" "$INT"
        test_with_f "$combo" "$INT $INT2"
    done
}

> "$LOG_FILE"

generate_flag_combinations

rm -f "$INT" "$INT2" "$PATTERN_FILE" "$OUT1" "$OUT2"

echo -e "\nПройдено: ${GREEN}$passed${NC}"
echo -e "Провалено: ${RED}$failed${NC}\n"

if [ "$failed" -eq 0 ]; then
    exit 0
else
    echo "Подробности в $LOG_FILE"
    exit 1
fi
