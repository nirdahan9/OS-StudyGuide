# Comprehensive uthreads Test Suite

This folder contains all the test files I created during development of the uthreads library.

## Files

### Individual Test Files (Each tests one specific requirement)
- **test_01_init_validation.c** - Tests uthread_init validation
- **test_02_spawn_basic.c** - Tests uthread_spawn basic functionality
- **test_03_block_resume.c** - Tests uthread_block/resume error cases
- **test_04_sleep_errors.c** - Tests uthread_sleep error handling
- **test_05_terminate.c** - Tests uthread_terminate error handling
- **debug_test.c** - Simple debug test for basic functionality
- **simple_test.c** - Basic functionality test (from tests/test.c)

### Test Runners
- **run_all_tests.sh** - Runs all individual tests and shows summary (RECOMMENDED)
- **run_tests.sh** - Legacy test runner
- **comprehensive_test.c** - All tests in one file (can cause issues if too many threads)

## Test Coverage Summary

Each test file focuses on a single aspect of the uthreads API:

| Test | Focus | Verifies |
|------|-------|----------|
| test_01_init_validation.c | Initialization | Init returns 0, total_quantums=1, main tid=0 |
| test_02_spawn_basic.c | Thread spawning | Spawn returns sequential tids, new threads have 0 quantums |
| test_03_block_resume.c | Block/Resume | Can't block main, errors on invalid tids, state transitions work |
| test_04_sleep_errors.c | Sleep errors | Main thread can't sleep |
| test_05_terminate.c | Termination | Terminate errors, get_quantums on terminated returns -1 |
| debug_test.c | Basic execution | Spawning, tidretrieval, quantum counting |
| simple_test.c | Scheduling | Round-robin scheduling with multiple threads |

### Requirements Covered

**From uthreads.h:**
- ✓ uthread_init validation and quantum counting
- ✓ uthread_spawn returns sequential tids
- ✓ uthread_spawn with NULL returns -1
- ✓ uthread_block/resume error handling
- ✓ uthread_block main thread fails
- ✓ uthread_sleep from main fails
- ✓ uthread_terminate error handling
- ✓ get_quantums error handling
- ✓ All getter functions work correctly

**From ex2.md:**
- ✓ "total_quantums starts at 1 immediately after uthread_init"
- ✓ "main thread tid == 0"
- ✓ "main thread quantums == 1 after init"
- ✓ "newly spawned threads have 0 quantums initially"
- ✓ "errors for invalid operations"

---

## How to Run

### Recommended: Run All Tests at Once
```bash
cd /workspaces/Exercises/ex2/my_tests
chmod +x run_all_tests.sh
./run_all_tests.sh
```

This will compile and run each test individually, showing:
- Individual test output (last 5 lines)
- Summary of passes/failures
- Overall result

### Run Individual Tests Manually
```bash
cd /workspaces/Exercises/ex2/my_tests

# Test 1: Init Validation
gcc-13 -std=gnu17 -I.. -o t1 test_01_init_validation.c ../uthreads.c ../jump.c
./t1

# Test 2: Spawn Basic
gcc-13 -std=gnu17 -I.. -o t2 test_02_spawn_basic.c ../uthreads.c ../jump.c
./t2

# Test 3: Block/Resume
gcc-13 -std=gnu17 -I.. -o t3 test_03_block_resume.c ../uthreads.c ../jump.c
./t3

# Test 4: Sleep Errors
gcc-13 -std=gnu17 -I.. -o t4 test_04_sleep_errors.c ../uthreads.c ../jump.c
./t4

# Test 5: Terminate
gcc-13 -std=gnu17 -I.. -o t5 test_05_terminate.c ../uthreads.c ../jump.c
./t5

# Debug test
gcc-13 -std=gnu17 -I.. -o debug debug_test.c ../uthreads.c ../jump.c
./debug
```

### Expected Output

Each test should output:
```
========================================
   TEST X: Description
========================================

[PASS] test description 1
[PASS] test description 2
...

========================================
        ALL TESTS PASSED!
========================================
Exit code: 0
```

## Test Results

All tests pass on x86_64 with GCC 13 and GNU C17:
```
✓ test_01_init_validation - PASSED
✓ test_02_spawn_basic - PASSED
✓ test_03_block_resume - PASSED
✓ test_04_sleep_errors - PASSED
✓ test_05_terminate - PASSED
✓ debug_test - PASSED
✓ simple_test - PASSED

Total: 7 tests, 0 failures
```

---

## Integration with Your Project

These test files are provided for verification purposes only. 
**Do not include them in your INGInious submission.**

Your submission should only contain:
- `uthreads.c` (your implementation)
- `theory_answers.pdf` (for the theory part)

See `../ex2.md` for submission requirements.
