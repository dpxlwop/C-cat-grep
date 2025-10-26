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

# Подготовка входных файлов
cat > "$INT" << EOF
Hello World
hello world
This is a test.
Another line with HELLO.
End of file.
EOF

cp "$INT" "$INT2"

# Паттерн для поиска
PATTERN="hello"

run_test() {
    local flags="$1"
    local files="$2"
    echo "Testing: $S21_GREP $flags -e '$PATTERN' $files ..."

    # Формируем команды
    local cmd1="$S21_GREP $flags -e \"$PATTERN\" $files"
    local cmd2="$SYS_GREP $flags -e \"$PATTERN\" $files"

    # Выполняем
    eval "$cmd1" > "$OUT1" 2>/dev/null || true
    eval "$cmd2" > "$OUT2" 2>/dev/null || true

    # Сравниваем
    if diff -q "$OUT1" "$OUT2" > /dev/null; then
        echo -e "${GREEN}OK${NC}"
        passed=$((passed + 1))
    else
        echo -e "${RED}FAIL${NC}"
        echo "Различия:"
        diff "$OUT1" "$OUT2"
        failed=$((failed + 1))
    fi
}

# Генерация всех комбинаций флагов из списка
generate_all_combinations() {
    local flags=("-e" "-i" "-v" "-c" "-l" "-n")
    local n=${#flags[@]}
    local total=$((1 << n))  # 2^6 = 64

    for ((mask = 0; mask < total; mask++)); do
        local combo=""
        for ((j = 0; j < n; j++)); do
            if (( (mask >> j) & 1 )); then
                combo+="${flags[j]} "
            fi
        done
        combo=$(echo "$combo" | sed 's/ $//')  # убрать завершающий пробел

        # Тестируем с одним и двумя файлами
        run_test "$combo" "$INT"
        run_test "$combo" "$INT $INT2"
    done
}

# Запуск всех тестов
generate_all_combinations

# Очистка
rm -f "$INT" "$INT2" "$OUT1" "$OUT2"

# Итог
echo -e "\nПройдено: ${GREEN}$passed${NC}"
echo -e "Провалено: ${RED}$failed${NC}\n"

if [ "$failed" -eq 0 ]; then
    exit 0
else
    exit 1
fi