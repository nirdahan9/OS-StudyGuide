#!/bin/bash

cd "$(dirname "$0")"

mkdir -p output

echo "Building tests for PQ2"

# gcc flags
CFLAGS="-pthread -std=c11 -Wall" 

# Compile unit test
echo "Compiling CV unit test"
gcc $CFLAGS -I. -o output/test_cv_unit tests/test_cv_unit.c cond_var.c tl_semaphore.c
if [ $? -ne 0 ]; then
    echo "Failed to compile CV unit test"
    exit 1
fi

# Compile signal test
echo "Compiling CV signal test"
gcc $CFLAGS -I. -o output/test_cv_signal tests/test_cv_signal.c cond_var.c tl_semaphore.c
if [ $? -ne 0 ]; then
    echo "Failed to compile CV signal test"
    exit 1
fi

# Compile broadcast stress test
echo "Compiling CV broadcast stress test"
gcc $CFLAGS -I. -o output/test_cv_broadcast_stress tests/test_cv_broadcast_stress.c cond_var.c tl_semaphore.c
if [ $? -ne 0 ]; then
    echo "Failed to compile CV broadcast stress test"
    exit 1
fi

echo "All tests compiled successfully!"

echo "Running CV unit test"
timeout 5 ./output/test_cv_unit
if [ $? -ne 0 ]; then
    echo "CV unit test failed or timed out"
    exit 1
fi

echo "Running CV signal test"
timeout 10 ./output/test_cv_signal
if [ $? -ne 0 ]; then
    echo "CV signal test failed or timed out"
    exit 1
fi

echo "Running CV broadcast stress test"
timeout 30 ./output/test_cv_broadcast_stress
if [ $? -ne 0 ]; then
    echo "CV broadcast stress test failed or timed out"
    exit 1
fi

echo "All tests passed successfully!"
exit 0
