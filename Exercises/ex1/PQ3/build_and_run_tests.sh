#!/bin/bash

cd "$(dirname "$0")"

mkdir -p output

echo "Building tests for PQ3"

# gcc flags
CFLAGS="-pthread -std=c11 -Wall" 

# Compile readers parallel test
echo "Compiling RW readers parallel test"
gcc $CFLAGS -I. -o output/test_rw_readers_parallel tests/test_rw_readers_parallel.c rw_lock.c cond_var.c tl_semaphore.c
if [ $? -ne 0 ]; then
    echo "Failed to compile RW readers parallel test"
    exit 1
fi

# Compile writer exclusive test
echo "Compiling RW writer exclusive test"
gcc $CFLAGS -I. -o output/test_rw_writer_exclusive tests/test_rw_writer_exclusive.c rw_lock.c cond_var.c tl_semaphore.c
if [ $? -ne 0 ]; then
    echo "Failed to compile RW writer exclusive test"
    exit 1
fi

# Compile writer preference stress test
echo "Compiling RW writer preference stress test"
gcc $CFLAGS -I. -o output/test_rw_writer_preference_stress tests/test_rw_writer_preference_stress.c rw_lock.c cond_var.c tl_semaphore.c
if [ $? -ne 0 ]; then
    echo "Failed to compile RW writer preference stress test"
    exit 1
fi

echo "All tests compiled successfully!"

echo "Running RW readers parallel test"
timeout 5 ./output/test_rw_readers_parallel
if [ $? -ne 0 ]; then
    echo "RW readers parallel test failed or timed out"
    exit 1
fi

echo "Running RW writer exclusive test"
timeout 10 ./output/test_rw_writer_exclusive
if [ $? -ne 0 ]; then
    echo "RW writer exclusive test failed or timed out"
    exit 1
fi

echo "Running RW writer preference stress test"
timeout 30 ./output/test_rw_writer_preference_stress
if [ $? -ne 0 ]; then
    echo "RW writer preference stress test failed or timed out"
    exit 1
fi

echo "All tests passed successfully!"
exit 0
