#!/usr/bin/env bash

COMPILER="../build/bin/mythril"
PASSED=0
FAILED=0

RED='\033[0;31m'
GREEN='\033[0;32m'
RESET='\033[0m'

echo -e "Running tests...\n"

echo "Testing valid inputs"

for file in ./valid/*.myth; do
    name=$(basename "$file")

    echo -n "Testing $name..."

    $COMPILER "$file" > /dev/null 2>&1
    exit_code=$?

    if [ $exit_code -eq 0 ]; then 
        echo -e "${GREEN}Passed${RESET}"
        ((PASSED++))
    else
        echo -e "${RED}Failed${RESET}"
        ((FAILED++))
    fi
done

echo -e "\nTesting invalid inputs"

for file in ./invalid/*.myth; do
    name=$(basename "$file")

    echo -n "Testing $name..."

    $COMPILER "$file" > /dev/null 2>&1
    exit_code=$?

    if [ $exit_code -eq 0 ]; then 
        echo -e "${RED}Failed${RESET}"
        ((FAILED++))
    else
        echo -e "${GREEN}Passed${RESET}"
        ((PASSED++))
    fi
done

echo -e "\n=== Test Summary ===\n"
echo "Passed: $PASSED"
echo "Failed: $FAILED"

if [ $FAILED -eq 0 ]; then
    echo -e "\n${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "\n${RED}Some tests failed!${NC}"
    exit 1
fi
