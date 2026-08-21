# Test Suite Function Coverage Map

## Quick Reference: Which Tests Cover Which Functions

### uthread_init(int quantum_usecs)
| Test | File | Description |
|------|------|-------------|
| 1.1 | extended | Basic initialization returns 0 |
| 1.2 | extended | Main thread ID is 0 after init |
| 1.3 | extended | Total quantums at least 1 |
| 1.4 | extended | Main thread quantums >= 1 |
| 1.5 | extended | Invalid TID returns -1 |
| 1.6 | extended | Total quantums counter is positive |
| Init | balanced | Library initialization |

### uthread_spawn(thread_entry_point entry_point)
| Test | File | Description |
|------|------|-------------|
| 2.1 | extended | First spawn returns TID 1 |
| 2.2 | extended | Second spawn returns TID 2 |
| 2.3 | extended | Third spawn returns TID 3 |
| 2.4 | extended | New thread has 0 quantums |
| 2.5 | extended | spawn(NULL) returns -1 |
| 2.6 | extended | Second spawn(NULL) returns -1 |
| 2.7 | extended | Spawn returns incrementing TIDs |
| 2.8 | extended | Multiple sequential spawns work |
| 2.10 | extended | Spawn beyond MAX_THREAD_NUM returns -1 |
| Spawn 1-6 | balanced | Basic spawn, error cases, limits |

### uthread_block(int tid)
| Test | File | Description |
|------|------|-------------|
| 3.1 | extended | Cannot block main thread (returns -1) |
| 3.2 | extended | Cannot block invalid TID (returns -1) |
| 3.3 | extended | Can block valid thread (returns 0) |
| 3.7 | extended | Double block is no-op (returns 0) |
| 3.9 | extended | Block-Resume-Block sequence works |
| 3.10 | extended | Multiple threads blocking works |
| Block 1-2 | balanced | Basic block operations |

### uthread_resume(int tid)
| Test | File | Description |
|------|------|-------------|
| 3.4 | extended | Resume on main thread has defined behavior |
| 3.5 | extended | Resume blocked thread returns 0 |
| 3.6 | extended | Resume invalid TID returns -1 |
| 3.8 | extended | Resume non-blocked thread returns 0 |
| 3.9 | extended | Block-Resume-Block sequence works |
| 3.10 | extended | Multiple block/resume works |
| Resume 3-5 | balanced | Resume operations |

### uthread_sleep(int num_quantums)
| Test | File | Description |
|------|------|-------------|
| 4.1 | extended | Main thread cannot sleep (returns -1) |
| 4.4 | extended | Spawned thread for sleep test |
| 4.5 | extended | Sleep with various durations |
| 4.6 | extended | Sleep should block for specified quantums |
| Sleep 1-2 | balanced | Sleep error cases and timing |

### uthread_terminate(int tid)
| Test | File | Description |
|------|------|-------------|
| 6.1 | extended | Terminate invalid TID returns -1 |
| 6.2 | extended | Terminate valid TID returns 0 |
| 6.3 | extended | Terminate already-terminated returns -1 |
| 6.4 | extended | Self-termination works |
| 6.5 | extended | Terminated thread resources cleaned up |
| 6.6 | extended | TID reused after termination |
| Terminate 1-3 | balanced | Termination and cleanup |

### uthread_get_tid()
| Test | File | Description |
|------|------|-------------|
| 11.1 | extended | get_tid() from main returns 0 |
| 11.7 | extended | get_tid returns current thread id |
| Get TID 1-2 | balanced | Get current thread ID |

### uthread_get_quantums(int tid)
| Test | File | Description |
|------|------|-------------|
| 1.5 | extended | Invalid TID returns -1 |
| 2.4 | extended | Newly spawned thread has 0 quantums |
| 5.3 | extended | New thread starts with 0 quantums |
| 5.5 | extended | Terminated thread quantum return -1 |
| 11.3 | extended | get_quantums on main >= 1 |
| 11.4 | extended | get_quantums on spawned thread = 0 |
| 11.5 | extended | get_quantums on invalid tid = -1 |
| Quantums 1-4 | balanced | Get per-thread quantum count |

### uthread_get_total_quantums()
| Test | File | Description |
|------|------|-------------|
| 1.3 | extended | Total quantums >= 1 after init |
| 1.6 | extended | Total quantums counter positive |
| 5.1 | extended | Total quantums monotonically increases |
| 11.2 | extended | get_total_quantums() > 0 |
| 15.3 | extended | Total quantums monotonically increases |
| 15.4b | extended | Total quantums never negative |
| Total Quantums | balanced | Get total quantum count |

---

## Test Distribution

### By Function
```
uthread_init:           6 tests
uthread_spawn:          9 tests  ⭐ (Most tested - complex behavior)
uthread_block:          6 tests
uthread_resume:         6 tests
uthread_sleep:          6 tests
uthread_terminate:      6 tests
uthread_get_tid:        2 tests
uthread_get_quantums:   7 tests
uthread_get_total_quantums: 6 tests
```

### By Category
```
Error Cases:           30+ tests
State Transitions:     15+ tests
Edge Cases:            25+ tests
Stress Tests:          10+ tests
Concurrency:           10+ tests
Resource Management:   10+ tests
```

---

## How to Use This Map

### If a test fails...
1. Find the test name in the "Quick Reference" above
2. See which function it tests
3. Focus on that function's implementation
4. Check the error conditions for that function

### If implementing a function...
1. Find the function in the map
2. Run just those tests
3. Verify all error cases are handled
4. Check state transitions work correctly

### For debugging...
1. Run the balanced version first
2. Find which test failed
3. Use this map to understand what's being tested
4. Check your implementation of that function
5. Re-run to verify the fix

---

## Test Execution Flow

```
START
  |
  +-> Initialize library (tests 1.1-1.6)
  |
  +-> Test spawning (tests 2.1-2.10)
  |
  +-> Test blocking (tests 3.1-3.10)
  |
  +-> Test resuming (tests 3.4-3.10)
  |
  +-> Test sleeping (tests 4.1-4.6)
  |
  +-> Test quantum counting (tests 5.1-5.5)
  |
  +-> Test termination (tests 6.1-6.6)
  |
  +-> Test TID allocation (tests 7.1-7.5)
  |
  +-> Test scheduling (tests 8.1-8.2)
  |
  +-> Test complex scenarios (tests 9.1-9.4)
  |
  +-> Stress tests (tests 10.1-10.4)
  |
  +-> Test getter functions (tests 11.1-11.5)
  |
  +-> Test edge cases (tests 12.1-12.7)
  |
  +-> Test return values (tests 13.1-13.10)
  |
  +-> Test error recovery (tests 14.1-14.4)
  |
  +-> Test consistency (tests 15.1-15.6)
  |
END
```

---

## Success Criteria

### All tests should PASS:
- ✓ 45+ tests in balanced version
- ✓ 80+ tests in extended version
- ✓ All error cases handled correctly
- ✓ All state transitions work
- ✓ No resource leaks
- ✓ Concurrent execution works

### Tests should SKIP:
- ⊘ Tests that require separate processes
- ⊘ Tests for undefined behavior

### Tests should FAIL initially:
- ✗ Any tests that expose bugs in your implementation
- ✗ Use these to guide your fixes!

---

## Compilation Commands

### Balanced version only
```bash
cd /workspaces/Exercises/ex2
gcc -std=gnu17 -Wall -Wextra -g \
    uthreads.c uthread_queue.c jump.c \
    my_tests/comprehensive_test_balanced.c \
    -o my_tests/test_comprehensive_balanced
```

### Extended version only
```bash
gcc -std=gnu17 -Wall -Wextra -g \
    uthreads.c uthread_queue.c jump.c \
    my_tests/comprehensive_test_extended.c \
    -o my_tests/test_comprehensive_extended
```

### Both versions
```bash
# Balanced
gcc -std=gnu17 -Wall -Wextra -g \
    uthreads.c uthread_queue.c jump.c \
    my_tests/comprehensive_test_balanced.c \
    -o my_tests/test_comprehensive_balanced

# Extended
gcc -std=gnu17 -Wall -Wextra -g \
    uthreads.c uthread_queue.c jump.c \
    my_tests/comprehensive_test_extended.c \
    -o my_tests/test_comprehensive_extended
```

---

## Running Tests

### Run balanced (recommended for development)
```bash
cd /workspaces/Exercises/ex2/my_tests
./test_comprehensive_balanced
```

### Run extended (for final validation)
```bash
timeout 30 ./test_comprehensive_extended
```

### Run with output to file
```bash
./test_comprehensive_balanced > test_results.txt 2>&1
cat test_results.txt
```

---

## Interpreting Results

### Balanced Version Target
- Aim for: 45-50 tests passing
- Some failures ok initially (helps debug)
- 0 skipped tests
- Typical runtime: 10-15 seconds

### Extended Version Target
- Aim for: 80+ tests passing
- May need more resources
- Some skips are ok
- Typical runtime: 20-30 seconds

---

## Common Issues and Solutions

### Test Failures
**Cause:** Bug in implementation  
**Solution:** Check which function failed, review that function's code

### Segmentation Fault
**Cause:** Memory access error in implementation  
**Solution:** Use gdb to debug: `gdb ./test_comprehensive_balanced`

### Test Timeout
**Cause:** Infinite loop or blocked progress  
**Solution:** Check blocking/resuming logic

### Out of Memory
**Cause:** Too many threads created  
**Solution:** Use balanced version instead of extended

---

**Document Version:** 1.0  
**Last Updated:** 2026-06-07  
**Status:** Reference Guide Complete ✓
