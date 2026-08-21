#!/bin/bash
# Test script to run all edge case tests

set -e

TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EX2_DIR="$(dirname "$TEST_DIR")"
RESULTS_FILE="$TEST_DIR/test_results.txt"

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "========================================" | tee "$RESULTS_FILE"
echo "   RUNNING COMPREHENSIVE TESTS" | tee -a "$RESULTS_FILE"
echo "========================================" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Function to compile and run a test
run_test() {
    local test_name=$1
    local test_file=$2
    local timeout=$3
    
    echo -e "${YELLOW}Running: $test_name${NC}" | tee -a "$RESULTS_FILE"
    echo "File: $test_file" | tee -a "$RESULTS_FILE"
    
    # Compile
    if ! gcc-13 -std=gnu17 -I"$EX2_DIR" -o "$TEST_DIR/${test_name}_bin" \
         "$test_file" "$EX2_DIR/uthreads.c" "$EX2_DIR/jump.c" 2>&1 | tee -a "$RESULTS_FILE"; then
        echo -e "${RED}[FAIL] Compilation failed${NC}" | tee -a "$RESULTS_FILE"
        return 1
    fi
    
    # Run with timeout
    if timeout "$timeout" "$TEST_DIR/${test_name}_bin" 2>&1 | tee -a "$RESULTS_FILE"; then
        echo -e "${GREEN}[PASS] $test_name completed successfully${NC}" | tee -a "$RESULTS_FILE"
        return 0
    else
        EXIT_CODE=$?
        if [ $EXIT_CODE -eq 124 ]; then
            echo -e "${RED}[FAIL] $test_name timed out${NC}" | tee -a "$RESULTS_FILE"
        else
            echo -e "${RED}[FAIL] $test_name exited with code $EXIT_CODE${NC}" | tee -a "$RESULTS_FILE"
        fi
        return 1
    fi
    echo "" | tee -a "$RESULTS_FILE"
}

# Run all tests
passed=0
failed=0

run_test "test_01_init_validation" "$TEST_DIR/test_01_init_validation.c" 5 && ((passed++)) || ((failed++))
echo "" | tee -a "$RESULTS_FILE"

run_test "test_02_spawn_basic" "$TEST_DIR/test_02_spawn_basic.c" 5 && ((passed++)) || ((failed++))
echo "" | tee -a "$RESULTS_FILE"

run_test "test_03_block_resume" "$TEST_DIR/test_03_block_resume.c" 5 && ((passed++)) || ((failed++))
echo "" | tee -a "$RESULTS_FILE"

run_test "test_04_sleep_errors" "$TEST_DIR/test_04_sleep_errors.c" 5 && ((passed++)) || ((failed++))
echo "" | tee -a "$RESULTS_FILE"

run_test "test_05_terminate" "$TEST_DIR/test_05_terminate.c" 5 && ((passed++)) || ((failed++))
echo "" | tee -a "$RESULTS_FILE"

run_test "debug_test" "$TEST_DIR/debug_test.c" 5 && ((passed++)) || ((failed++))
echo "" | tee -a "$RESULTS_FILE"

run_test "simple_test" "$TEST_DIR/simple_test.c" 5 && ((passed++)) || ((failed++))
echo "" | tee -a "$RESULTS_FILE"

# Summary
echo "========================================" | tee -a "$RESULTS_FILE"
echo "              FINAL SUMMARY" | tee -a "$RESULTS_FILE"
echo "========================================" | tee -a "$RESULTS_FILE"
echo "Tests Passed: $passed" | tee -a "$RESULTS_FILE"
echo "Tests Failed: $failed" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

if [ $failed -eq 0 ]; then
    echo -e "${GREEN}✓ ALL TESTS PASSED!${NC}" | tee -a "$RESULTS_FILE"
    exit 0
else
    echo -e "${RED}✗ SOME TESTS FAILED${NC}" | tee -a "$RESULTS_FILE"
    exit 1
fi
