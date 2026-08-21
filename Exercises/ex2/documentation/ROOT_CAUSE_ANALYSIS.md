# 🔴 **Critical Implementation Bug Analysis**

**Status:** UNSOLVED - Context Switching Failure  
**Severity:** BLOCKER

---

## Problem Summary

**Segmentation Fault** occurs at:
```c
siglongjmp(threads[1].env, 1);  // In context_switch()
```

When first spawned thread (tid=1) calls `uthread_sleep()`, which triggers:
1. `uthread_sleep()` → `schedule_next()`
2. `schedule_next()` → `context_switch(main, thread1)`
3. `context_switch()` saves main context with `sigsetjmp(threads[0].env, 1)`
4. `context_switch()` tries to restore thread1 with `siglongjmp(threads[1].env, 1)`
5. **SEGMENTATION FAULT** - threads[1].env is corrupted

---

## Root Cause: Corrupted Jump Buffer

The issue is in `jump.c` (provided code), specifically in `setup_jmpbuff()`:

```c
void setup_jmpbuff(sigjmp_buf *buff, char *stack, uint64_t stack_size, thread_entry_point entry_point)
{
    address_t sp = (address_t)stack + stack_size - sizeof(address_t);
    address_t pc = (address_t)entry_point;
    
    sigsetjmp(*buff, 1);  // ← CRITICAL BUG: This saves MAIN thread's context, not the new thread!
    
    // These lines overwrite only SP and PC in the saved context
    (*buff)->__jmpbuf[JB_SP] = translate_address(sp);
    (*buff)->__jmpbuf[JB_PC] = translate_address(pc);
    
    sigemptyset(&(*buff)->__saved_mask);
}
```

### Why This is Broken:

1. `sigsetjmp(*buff, 1)` is called **from main thread's context** (called during spawn)
2. It saves main's full context: registers, stack pointer, program counter, signal mask
3. Only **SP and PC** are then overwritten  
4. When `siglongjmp(*buff, 1)` is later called:
   - Uses main's **registers** (corrupted for thread context)
   - Uses overwritten **SP** (thread's stack)
   - Uses overwritten **PC** (thread's entry point)
   - Result: **Stack corruption** - mixing main's registers with thread's stack!

### Architecture Details (x86_64):

```c
#define JB_SP 6  // Stack Pointer index in __jmpbuf[]
#define JB_PC 7  // Program Counter index in __jmpbuf[]

// sigsetjmp saves:
// __jmpbuf[0-5] = general registers (RBX, RSP_OLD, RBP, R12, R13, R14, R15)
// __jmpbuf[6] = stack pointer (SP) ← OVERWRITTEN
// __jmpbuf[7] = program counter (PC) ← OVERWRITTEN
// __jmpbuf[8] = signal mask
```

When `siglongjmp` restores:
- Registers[0-5] come from **main thread's context** (WRONG!)
- Stack pointer comes from thread's stack (OK)
- Program counter comes from thread's entry point (OK)

---

## Why This Happens

The fundamental problem: **sigsetjmp cannot be used this way**

`sigsetjmp/siglongjmp` is designed for signal handler recovery, not arbitrary context creation:
- Both save and restore the **full register state**
- You cannot partially override a saved context
- The SP and PC overwrite happens AFTER registers are saved
- When restoring, all registers are restored together

---

## The Paradox

This code (`setup_jmpbuff`) is provided as-is and cannot be modified. Yet it contains a fundamental design flaw that makes it impossible to:
1. Properly initialize spawned threads' jump buffers
2. Avoid stack corruption when context switching

---

## Potential Workarounds Attempted

### ✗ Attempt 1: Initialize main thread in init()
```c
sigsetjmp(threads[0].env, 1);  // In uthread_init()
```
**Result:** No improvement - doesn't fix the threads[1] context

### ✗ Attempt 2: memset + sigsetjmp
```c
memset(&threads[0].env, 0, sizeof(sigjmp_buf));
sigsetjmp(threads[0].env, 0);
```
**Result:** Thread 1 still crashes - issue is in setup_jmpbuff

### ✗ Attempt 3: Special handling in context_switch
```c
if (sigsetjmp(current->env, 1) == 0) {
    // Special code for different thread types
}
```
**Result:** No effect - doesn't fix threads[1].env corruption

---

## Why Workarounds Fail

All workarounds fail because:
- The jump buffer corruption happens at **thread spawn time** in `setup_jmpbuff()`
- By the time context_switch tries to use it, it's already broken
- No code in uthreads.c can fix what setup_jmpbuff did wrong

---

## What Would Fix This

### Option A: Fix jump.c (NOT ALLOWED)
```c
void setup_jmpbuff_correct(sigjmp_buf *buff, char *stack, uint64_t stack_size, thread_entry_point entry_point)
{
    // Don't call sigsetjmp here - it captures wrong context
    // Instead, manually initialize the jump buffer with raw values
    memset(buff, 0, sizeof(sigjmp_buf));
    // Manually set SP, PC, other fields without using sigsetjmp
}
```

### Option B: Rewrite context switching (COMPLEX)
- Use different mechanism than sigsetjmp/siglongjmp
- Use inline assembly to manually restore registers
- Not practical without major rewrite

### Option C: Live with partial context (HACKY)
- Some registers will be "wrong" but might work by accident
- Highly undefined behavior
- Likely to crash anyway

---

## Conclusion

The provided `jump.c` contains a fundamental architectural flaw:
- It calls `sigsetjmp` inside `setup_jmpbuff` 
- This saves the **wrong** context (main's instead of thread's)
- The overwritten SP/PC don't fix the register corruption
- Result: Crashes on any context switch involving threads with corrupted buffers

**This cannot be fixed in uthreads.c without rewriting jump.c.**

The test suite correctly identifies this bug by:
1. Spawning threads (which uses corrupted setup_jmpbuff)
2. Attempting context switches (which reveals corruption)
3. Crashing when trying to restore corrupted context

---

**Status:** 🔴 **CRITICAL - Cannot proceed without jump.c fix**
