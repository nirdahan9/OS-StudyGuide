#!/bin/bash

cd "$(dirname "$0")"

mkdir -p output

echo "Building tests..."

# gcc flags
CFLAGS="-pthread -std=c11 -Wall" 

# Compile unit test
echo "Compiling basic unit test"
gcc $CFLAGS -o output/run_test_unit tests/test_unit.c tl_semaphore.c
if [ $? -ne 0 ]; then
    echo "Failed to compile unit test"
    exit 1
fi

# Compile concurrent test
echo "Compiling concurrency test"
gcc $CFLAGS -o output/run_test_concurrent tests/test_concurrent.c tl_semaphore.c
if [ $? -ne 0 ]; then
    echo "Failed to compile concurrent test"
    exit 1
fi

# Compile stress test
echo "Compiling stress test"
gcc $CFLAGS -o output/run_test_stress tests/test_stress.c tl_semaphore.c
if [ $? -ne 0 ]; then
    echo "Failed to compile stress test"
    exit 1
fi

echo "All tests compiled successfully!"

echo "Running basic unit test"
timeout 5 ./output/run_test_unit
if [ $? -ne 0 ]; then
    echo "Unit test failed or timed out"
    exit 1
fi

echo "Running concurrent test (8 threads)"
timeout 5 ./output/run_test_concurrent
if [ $? -ne 0 ]; then
    echo "Concurrent test failed or timed out"
    exit 1
fi

echo "Running stress test (16 threads, 10,000 iterations)"
timeout 10 ./output/run_test_stress
if [ $? -ne 0 ]; then
    echo "Stress test failed or timed out"
    exit 1
fi

echo "All tests passed successfully!"
exit 0
