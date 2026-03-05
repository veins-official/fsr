#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

TOTAL=0
PASSED=0
FAILED=0

run_test() {
	local test_name="$1"
	local expected="$2"
	local rows="$3"
	local cols="$4"
	local data="$5"
	local output

	TOTAL=$((TOTAL + 1))

	echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
	echo -e "${YELLOW}Test $TOTAL: $test_name${NC}"
	echo -e "${BLUE}Arguments: $rows $cols${NC}"
	echo -e "${BLUE}Data: $data${NC}"

	output=$(echo "$data" | ./main.out "$rows" "$cols" 2>&1)

	if echo "$output" | grep -q "$expected"; then
		echo -e "${GREEN}Success${NC}"
		PASSED=$((PASSED + 1))
	else
		echo -e "${RED}Expected: $expected${NC}"
		FAILED=$((FAILED + 1))
	fi
	echo "Program output:"
	echo "$output"
	echo ""
}

echo -e "${BLUE}Compiling a program...${NC}"
make clean && make
if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation error${NC}"
    exit 1
fi
echo -e "${GREEN}Compilation successful${NC}\n"


run_test "Просто определитель" "Determinant = -2.000000" 2 2 "5 6 7 8"
run_test "Вырожденная матрица" "Determinant = 0" 3 3 "1 2 3 2 4 6 3 6 9"
run_test "Не квадратная" "Error: Not square" 3 2 "1 2 3 2 4 6"
run_test "Не правильные параметры" "Usage: ./main.out rows cols" q q ""


echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo -e "Total tests: $TOTAL"
echo -e "${GREEN}Success: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"

