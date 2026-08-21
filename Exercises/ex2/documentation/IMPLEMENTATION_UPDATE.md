# IMPLEMENTATION UPDATES - JUNE 4, 2026

## ✅ COMPLETED: Full rewrite of uthreads.c with all requirements

### Major Features Implemented

#### 1. **READY Queue Data Structure**
- Static array-based queue with front/back pointers and count
- Functions: `enqueue_ready()`, `dequeue_ready()`, `remove_from_ready()`
- Maintains threads in READY state as per round-robin scheduling requirements

#### 2. **Signal Masking (Critical Section Protection)**
- Added `mask_signal()` and `unmask_signal()` helper functions
- Protects all critical state updates with signal masking
- Prevents SIGVTALRM interrupts during critical operations
- Exits with error code 1 if masking fails: `"system error: masking failed"`

#### 3. **Comprehensive Error Handling**
- **System errors** (printed to stderr, exit code 1):
  - `system error: sigaction failed`
  - `system error: setitimer failed`
  - `system error: masking failed`
  
- **Thread library errors** (printed to stderr, return -1):
  - `thread library error: quantum_usecs must be positive`
  - `thread library error: entry_point cannot be NULL`
  - `thread library error: no available thread slots`
  - `thread library error: invalid thread ID`
  - `thread library error: cannot block main thread`
  - `thread library error: thread does not exist`
  - `thread library error: main thread cannot sleep`
  - `thread library error: num_quantums must be positive`

#### 4. **Proper Quantum Counting**
- Quantum increments occur in `context_switch()` BEFORE context switch
- Both `total_quantums` and per-thread `quantums` incremented on EVERY schedule
- **NOT** just on timer expiration - on any scheduling event
- Matches requirement: "A new quantum begins whenever a uthread becomes RUNNING"

#### 5. **Separate Block + Sleep Tracking**
- New global array: `is_blocked[MAX_THREAD_NUM]` for explicit blocking
- `threads[tid].sleep_until` for sleep tracking
- Thread becomes READY only when:
  - NOT explicitly blocked (`!is_blocked[tid]`), AND
  - Sleep has expired (`total_quantums >= sleep_until`) OR sleep_until == 0

#### 6. **Signal Handler Improvements**
- Replaced `signal()` with `sigaction()` for better control
- Properly initializes mask and flags
- More robust signal handling

#### 7. **Queue Management in All Operations**
- `uthread_spawn()`: Enqueues new READY threads
- `uthread_terminate()`: Removes thread from queue
- `uthread_block()`: Removes READY thread from queue, marks BLOCKED
- `uthread_resume()`: Re-enqueues thread if conditions met
- `context_switch()`: Enqueues current thread when moving to READY
- `schedule_next()`: Dequeues from front for FIFO ordering

---

## ✅ TEST RESULTS

### My Test Suite (6/6 PASSED)
```
✓ test_01_init_validation
✓ test_02_spawn_basic
✓ test_03_block_resume
✓ test_04_sleep_errors
✓ test_05_terminate
✓ debug_test
```

### Provided Test Suite (testuthreads.c)
- All GROUP 1-6 tests passing ✓
- Error messages printing to stderr correctly ✓
- No crashes or hangs ✓
- Proper error handling on all edge cases ✓

---

## 📋 REQUIREMENTS CHECKLIST

From **ex2.md**:

✅ **Scheduler**
- Round-robin with quantum-based preemption
- READY queue data structure implemented
- Queue management: enqueue on READY, dequeue from front

✅ **Quantum Counting**
- Starts at 1 immediately after init
- Increments for every RUNNING state entry
- Both global and per-thread counts tracked

✅ **Time Management**
- Virtual timer (ITIMER_VIRTUAL) with SIGVTALRM
- Timer resets on each new quantum
- Proper time slice allocation

✅ **Blocking and Sleeping**
- Separate tracking for explicit block vs sleep
- Thread only READY when both conditions clear
- Resume doesn't wake sleeping threads early

✅ **Signal Masking**
- Critical sections protected with sigprocmask
- Exits with code 1 if masking fails
- Prevents timer interrupts during state updates

✅ **Error Handling**
- System call errors with "system error:" prefix to stderr
- Thread library errors with "thread library error:" prefix to stderr
- All error cases handled and return appropriate values

✅ **Code Quality**
- No dynamic allocation (uses static arrays)
- Proper signal handling with sigaction
- Clean separation of concerns
- No output to stdout from library

---

## 🔧 IMPLEMENTATION DETAILS

### Global State
```c
static thread_t threads[MAX_THREAD_NUM];        // TCB array
static char stacks[MAX_THREAD_NUM][STACK_SIZE]; // Stack array
static int ready_queue[MAX_THREAD_NUM];         // READY queue
static int ready_front, ready_back, ready_count; // Queue pointers
static bool is_blocked[MAX_THREAD_NUM];         // Explicit block tracking
```

### Critical Functions Modified
1. `uthread_init()` - Added queue init, sigaction, error handling
2. `uthread_spawn()` - Added signal masking, queue management
3. `uthread_terminate()` - Added signal masking, queue removal
4. `uthread_block()` - Separate blocking tracking, queue removal
5. `uthread_resume()` - Check both block and sleep conditions
6. `uthread_sleep()` - Added signal masking, proper block tracking
7. `schedule_next()` - Uses READY queue, proper wake logic
8. `context_switch()` - Increments quantums, manages queue
9. `timer_handler()` - Simplified to just call schedule_next()

### Helper Functions Added
- `mask_signal()` / `unmask_signal()` - Signal masking
- `enqueue_ready()` - Add to queue
- `dequeue_ready()` - Remove from queue
- `remove_from_ready()` - Remove specific thread

---

## 🚀 READY FOR SUBMISSION

All requirements from ex2.md implemented ✓
All error handling implemented ✓
All tests passing ✓
Code compiles with gcc-13 -std=gnu17 ✓
No warnings (except unused parameter suppressed) ✓
