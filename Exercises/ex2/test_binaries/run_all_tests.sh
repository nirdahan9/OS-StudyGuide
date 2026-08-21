#!/bin/bash

##############################################################################
#                    RUN ALL TESTS - COMPREHENSIVE SUMMARY                  #
##############################################################################

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║                   UTHREADS TEST SUITE RUNNER                    ║"
echo "║                    Running All Test Binaries                    ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Find all executable test binaries (not .c or .txt files)
test_files=()
test_files+=("/workspaces/Exercises/ex2/test_provided")
test_files+=("/workspaces/Exercises/ex2/test_uthreads")
test_files+=("/workspaces/Exercises/ex2/my_tests/test_comprehensive_balanced")
test_files+=("/workspaces/Exercises/ex2/my_tests/test_comprehensive_extended")
test_files+=("/workspaces/Exercises/ex2/my_tests/test_bin_01")
test_files+=("/workspaces/Exercises/ex2/my_tests/test_bin_02")
test_files+=("/workspaces/Exercises/ex2/my_tests/test_bin_03")
test_files+=("/workspaces/Exercises/ex2/my_tests/test_bin_04")
test_files+=("/workspaces/Exercises/ex2/my_tests/test_bin_05")
test_files+=("/workspaces/Exercises/ex2/my_tests/test_bin_debug")

total_passed=0
total_failed=0
tests_run=0
tests_exist=0

for test_file in "${test_files[@]}"; do
    # Check if file exists and is executable
    if [[ -x "$test_file" ]]; then
        tests_exist=$((tests_exist + 1))
        test_name=$(basename "$test_file")
        
        echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
        echo -e "${BLUE}▶ Running: ${YELLOW}$test_name${NC}"
        echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
        
        # Run test and capture output
        output=$(timeout 10 "$test_file" 2>&1)
        exit_code=$?
        
        # Count PASS occurrences
        pass_count=$(echo "$output" | grep -c "\[PASS\]\|✓" || true)
        
        # Count FAIL occurrences  
        fail_count=$(echo "$output" | grep -c "\[FAIL\]\|✗" || true)
        
        # Handle timeout
        if [[ $exit_code -eq 124 ]]; then
            echo -e "${RED}⏱ TIMEOUT${NC} - Test exceeded 10 seconds"
            status_symbol="⏱"
            result="${RED}TIMEOUT${NC}"
        elif [[ $exit_code -eq 0 ]]; then
            status_symbol="✓"
            result="${GREEN}PASSED${NC}"
        else
            status_symbol="✗"
            result="${RED}FAILED${NC}"
        fi
        
        # Display results
        if [[ $pass_count -gt 0 || $fail_count -gt 0 ]]; then
            echo -e "$status_symbol Tests: ${GREEN}$pass_count passed${NC}, ${RED}$fail_count failed${NC}"
            total_passed=$((total_passed + pass_count))
            total_failed=$((total_failed + fail_count))
        else
            echo -e "$status_symbol Result: $result (exit code: $exit_code)"
        fi
        
        tests_run=$((tests_run + 1))
        echo ""
    fi
done

# Final Summary
echo ""
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║                         FINAL SUMMARY                           ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""
echo -e "Test Binaries Found:  ${BLUE}$tests_exist${NC}"
echo -e "Tests Executed:       ${BLUE}$tests_run${NC}"
echo -e "Total PASS:           ${GREEN}$total_passed${NC}"
echo -e "Total FAIL:           ${RED}$total_failed${NC}"

if [[ $total_failed -eq 0 && $total_passed -gt 0 ]]; then
    echo ""
    echo -e "${GREEN}✓ ALL TESTS PASSED!${NC}"
elif [[ $total_passed -gt 0 ]]; then
    echo ""
    echo -e "${YELLOW}⚠ Some tests failed${NC}"
fi

echo ""
