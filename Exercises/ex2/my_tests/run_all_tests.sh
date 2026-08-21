#!/bin/bash
# Simple test runner - runs each test individually

TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EX2_DIR="$(dirname "$TEST_DIR")"

echo "=========================================="
echo "  RUNNING INDIVIDUAL UTHREADS TESTS"
echo "=========================================="
echo ""

passed=0
failed=0

run_test() {
    local num=$1
    local name=$2
    
    echo "TEST $num: $name"
    echo "---"
    
    # Compile
    gcc-13 -std=gnu17 -I"$EX2_DIR" \
        -o "$TEST_DIR/test_bin_$num" \
        "$TEST_DIR/test_${num}${name}.c" \
        "$EX2_DIR/uthreads.c" \
        "$EX2_DIR/uthread_queue.c" \
        "$EX2_DIR/jump.c" 2>/dev/null
    
    # Run with timeout
    if timeout 5 "$TEST_DIR/test_bin_$num" 2>&1 | tail -5; then
        echo "✓ PASSED"
        ((passed++))
    else
        echo "✗ FAILED (exit code $?)"
        ((failed++))
    fi
    
    echo ""
}

# Run all individual tests
run_test "01" "_init_validation"
run_test "02" "_spawn_basic"
run_test "03" "_block_resume"
run_test "04" "_sleep_errors"
run_test "05" "_terminate"

# Also run the simple test suites
echo "TEST 6: debug_test"
echo "---"
gcc-13 -std=gnu17 -I"$EX2_DIR" \
    -o "$TEST_DIR/test_bin_debug" \
    "$TEST_DIR/debug_test.c" \
    "$EX2_DIR/uthreads.c" \
    "$EX2_DIR/uthread_queue.c" \
    "$EX2_DIR/jump.c" 2>/dev/null

if timeout 5 "$TEST_DIR/test_bin_debug" 2>&1; then
    echo "✓ PASSED"
    ((passed++))
else
    echo "✗ FAILED"
    ((failed++))
fi
echo ""

# Summary
echo "=========================================="
echo "              TEST SUMMARY"
echo "=========================================="
echo "Passed: $passed"
echo "Failed: $failed"
echo "Total:  $((passed + failed))"
echo ""

if [ $failed -eq 0 ]; then
    echo "✓ ALL TESTS PASSED!"
    exit 0
else
    echo "✗ SOME TESTS FAILED"
    exit 1
fi
