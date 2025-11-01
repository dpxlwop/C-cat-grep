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

TEST_CONTENT=$'\nHello\tworld\t\n\t\t\n\n\ntest test\t\nbye\x01\x0212345\x08\x08\x08678\x04\x1b\x7fInvisible chars test\x05END'

run_test() {
    local flags="$1"
    echo "$TEST_CONTENT" > "$INT"
    local cmd="$S21_CAT $flags $INT"
    echo "Testing: $cmd ... "
    local sys_cmd="${cmd#./}"
    sys_cmd="${sys_cmd/s21_cat/cat}"
    
    eval "$cmd" > "$OUT1" 2>/dev/null || true
    eval "$sys_cmd" > "$OUT2" 2>/dev/null || true

    if diff -q "$OUT1" "$OUT2" > /dev/null; then
        echo -e "${GREEN}OK${NC}"
        passed=$((passed + 1))
    else
        echo -e "${RED}FAIL${NC}"
        failed=$((failed + 1))
    fi
}

generate_combinations() {
    local flags=("-b" "-e" "-n" "-s" "-t" "-v")
    local n=${#flags[@]}
    local max=$((1 << n))  # 2^n

    for ((i=0; i<max; i++)); do
        local combo=""
        for ((j=0; j<n; j++)); do
            if (( (i >> j) & 1 )); then
                combo+="${flags[j]} "
            fi
        done
        combo=$(echo "$combo" | sed 's/ $//')
        if [ -z "$combo" ]; then
            run_test ""
        else
            run_test "$combo"
        fi
    done
}

generate_combinations

rm -f "$INT" "$OUT1" "$OUT2"

echo -e "\nПройдено: ${GREEN}$passed${NC}"
echo -e "Провалено: ${RED}$failed${NC}\n"

if [ "$failed" -eq 0 ]; then
    exit 0
else
    exit 1
fi