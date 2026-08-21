# Recitation 2 — Processes, Threads and the Synchronization Problem

> Study notes converted from the recitation slide deck `Rec2_OS.pdf` (98 pages) — Operating Systems, Reichman University, Spring Semester 2026.
> Lecturer: Tsvi Cherny-Shahar · TAs: Daniel Karalnik, Liam Tal.
> Faithful conversion: every substantive item on the source pages appears here, and nothing has been added from outside them. Figures are rendered as italic *Figure:* descriptions.
> `<!-- source pages a-b -->` comments mark the original page ranges.

---

<!-- source pages 1-16 -->

## Recitation 2 — Multi-Threading and Synchronization

### Table of Contents

1. **Process and Thread Fundamentals**
   - Process Structure
   - Thread Basics
   - Lifecycle Management
2. **Process and Thread Communication**
   - Communication Models
   - Shared Memory in Practice
3. **The Synchronization Problem**
   - Overview
4. **Low-Level Synchronization**
   - Atomic Operations
   - Critical Sections
   - Spinlocks
   - C Atomics Library
   - Memory Ordering
5. **Synchronization Primitives**
   - Semaphores
   - Read-Write Locks
   - Condition Variables
6. **Classic Synchronization Problems**
   - Producer-Consumer Pattern

---

## Process and Thread Fundamentals

### Structure of a Process

> **Process Memory Layout**
>
> The memory of a **single-threaded process** is (usually) split into 5 sections:

- **Code** – Instructions to execute
- **Data** – Static data compiled into executable file (hard-coded strings...)
- **Heap** – Manually managed memory (malloc, new...)
- **Stack** – Automatically managed memory
  - Local variables
  - Function call stack
- **Metadata** – OS data about the process

#### Memory Layout and Stack

*Figure: The address space of Process $P_1$ drawn as a single column of stacked blocks, from top to bottom: Code, Data, Heap, Stack, OS Data. An arrow on the left labelled "Heap grows up" points upward alongside the Code/Data/Heap region, and an arrow labelled "Stack grows down" points downward alongside the Stack/OS Data region.*

### Thread Stack

- Each thread has its own stack for function calls and local variables
- The stack pointer (SP) and base pointer (BP) manage the stack
- The stack operates in a **Last In, First Out (LIFO)** manner
- Stores:
  - Local variables
  - Function call information (stack frames)
  - Return addresses
- Thread stacks are allocated separately to avoid conflicts

*Figure: Three separate, non-overlapping colored boxes side by side labelled "Thread 1 Stack", "Thread 2 Stack" and "Thread 3 Stack", captioned "Each thread has its own stack in memory."*

### Stack Growth

> **How the Stack Grows**
>
> The stack is **dynamic** – it grows and shrinks during execution:
> - **Grows** when a function is called (new stack frame pushed)
> - **Shrinks** when a function returns (stack frame popped)

- Stack typically grows **downward** (toward lower addresses)
- The OS sets a **maximum stack size** for each thread
- Stack can grow dynamically **up to this limit**
- Exceeding the limit causes **stack overflow**

### Process and Thread Control Blocks

- **Process Control Block (PCB)**: Stores process-level information like PID, memory allocation, and open files.
- **Thread Control Block (TCB)**: Contains thread-specific data such as stack pointer, registers, and thread state.
- Each process has **one PCB** but can have **multiple TCBs** (one per thread).

*Figure: Inside a dashed box labelled "Process $P_1$", a single PCB box at the top ("PID, Memory Pointers, Files") has three arrows pointing down to three TCB boxes — $TCB_1$, $TCB_2$, $TCB_3$ — each containing "SP, PC, Regs". Annotations at the right: "1 PCB per process" and "$n$ TCBs per process".*

### Context Switching

*Figure: A horizontal timeline. A blue segment labelled "Thread i" runs until a lightning-bolt "Interrupt", after which a dashed grey segment labelled "OS interrupt handler" runs, followed by a green segment labelled "Thread j". A red box below the handler shows the two operations performed: `TCB_i.context = CPU regs` then `CPU regs = TCB_j.context`. An arrow at the bottom marks the direction of "time".*

- **Context**: Stack pointer (SP), Program counter (PC), CPU registers
- **Requires**: Updating the PCB and TCB
- **Overhead**: Saving/loading states, managing memory mappings

### Thread States

*Figure: State machine with five boxes. New —"admitted"→ Ready —"scheduled"→ Running; Running —"preempted"→ Ready; Running —"exit"→ Terminated; Running —"I/O wait"→ Waiting; Waiting —"I/O done"→ Ready.*

- **New**: Being created
- **Ready**: Waiting for CPU
- **Running**: Executing
- **Waiting**: Blocked on I/O
- **Terminated**: Finished

---

## Lifecycle Management: Processes

### Process Creation with `fork()`

- `fork()` creates a **copy** of the calling process
- Parent and child continue execution from the same point
- **Return value** distinguishes parent from child:
  - **Parent**: receives child's PID (positive)
  - **Child**: receives 0
  - **Error**: returns -1

### What Does `fork()` Copy?

**Child receives a copy of:**

- Address space (code, data, heap, stack)
- Open file descriptors (more on that later)
- Signal handlers (more on that later)
- Environment variables
- Current working directory

**Not copied (unique to child):**

- PID (child gets new PID)
- Parent PID (set to original process)
- Pending signals (cleared, more on that later)
- **Other threads** – only the calling thread is copied

**Copy-on-Write (COW):** Pages (memory sections) are shared until one process writes, then copied. Efficient for `fork()` + `exec()` pattern.

### `fork()` Example in C

```c
#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
    } else if (pid == 0) {
        printf("Child PID: %d\n", getpid());
    } else {
        printf("Parent, child: %d\n", pid);
        wait(NULL);
    }
    return 0;
}
```

**Output:**

```
Parent, child: 1234
Child PID: 1234
```

*Order may vary*

### The `exec()` Family

- `exec()` **replaces** the current process memory and code with a new program
- The PID remains the same – only the code/data/stack change
- Does **not return** on success (the old program is gone)

**Common variants:**

- `execl(path, arg0, arg1, ..., NULL)` – args as list
- `execv(path, argv[])` – args as array
- `execlp`/`execvp` – searches PATH for executable

**fork() + exec() pattern:**

1. Parent calls `fork()` to create child
2. Child calls `exec()` to run a different program
3. Parent calls `wait()` to reap child

### `fork()` + `exec()` Example

```c
pid_t pid = fork();
if (pid == 0) {
    // Child: run "ls -l"
    execlp("ls", "ls", "-l", NULL);
    perror("exec failed");
    exit(1);
} else if (pid > 0) {
    wait(NULL);
    printf("Child finished\n");
}
```

**Output:**

```
-rw-r--r-- 1 user main.c
-rwxr-xr-x 1 user a.out
Child finished
```

*Child runs `ls`, parent prints after `wait()`*

---

<!-- source pages 17-35 -->

## Thread Creation

- Threads are created within a process to execute tasks concurrently
- Uses `pthread_create()` (POSIX threads)
- Thread creation involves:
  - Allocating a stack for the thread
  - Initializing the **thread control block (TCB)**
  - Setting the thread's entry point (function to execute)
- Threads share code, data, heap; each has own stack

### `pthread_create()` Example

```c
#include <pthread.h>
#include <stdio.h>

void* thread_func(void* arg) {
    int id = *(int*)arg;
    printf("Thread %d running\n", id);
    return NULL;
}

int main() {
    pthread_t tid;
    int arg = 1;
    pthread_create(&tid, NULL,
                   thread_func, &arg);
    pthread_join(tid, NULL); // wait for thread to finish
    printf("Thread joined\n");
    return 0;
}
```

Output:

```
Thread 1 running
Thread joined
```

Compile with:

```bash
gcc -pthread prog.c
```

### Multi-threaded Calculation Example

```c
#define NUM_THREADS 4
int partial_sum[NUM_THREADS];  // Each thread writes to its own index

void* compute_sum(void* arg) {
    int id = *(int*)arg;
    partial_sum[id] = 0;
    // Thread 0: 0-24, Thread 1: 25-49, Thread 2: 50-74, Thread 3: 75-99
    for (int i = id * 25; i < (id + 1) * 25; i++)
        partial_sum[id] += i;
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, compute_sum, &ids[i]);
    }
    int total = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        total += partial_sum[i];
    }
    printf("Sum 0-99: %d\n", total);  // Arithmetic series: n(n+1)/2 = 4950
}
```

---

## Thread Join and Detach

| Joinable Thread (default) | Detached Thread |
| --- | --- |
| Resources held until `pthread_join()` | Resources freed **automatically** on termination |
| Calling thread **blocks** until target finishes | Cannot join or get return value |
| Can retrieve return value | "Fire and forget" |
| Must join to avoid resource leak | |

```c
pthread_join(tid, &retval);
```

```c
pthread_detach(tid);
```

---

## Thread vs Process Termination

**Process Termination**

- `exit(status)` – terminates process
- `return` from `main()`
- Receiving fatal signal (e.g., SIGKILL)
- **All threads die** when process terminates
- OS cleans up: memory, file descriptors, locks

**Thread Termination**

- `pthread_exit(retval)`
- `return` from thread function
- `pthread_cancel(tid)`
- **Other threads continue** running
- Resources: freed by join or if detached

---

## Zombie Processes

A process enters the **zombie (defunct)** state after finishing execution but before being reaped by its parent.

Mechanism:

1. **Termination**: Process calls `exit()`, OS frees memory and files
2. **Retention**: Kernel keeps entry in process table (PID + exit status)
3. **Reaping**: Parent calls `wait()`/`waitpid()`, zombie is removed

Risk – PID Exhaustion:

- Zombies consume no CPU/RAM but hold onto their PID
- Mass accumulation $\rightarrow$ process table full $\rightarrow$ no new processes
- Visible in `ps` with status **Z** or `<defunct>`

### Orphans vs Zombies

**Orphan Process**

- A **living** process whose parent has **died**
- Adopted by `init` (PID 1)

**Zombie Process**

- A **dead** process waiting to be **reaped**
- Parent hasn't called `wait()`

**The Role of `init` (PID 1):**
`init` is the first process started by the kernel at boot – ancestor of all processes.

- Adopts orphans immediately when their parent dies
- If adopted child is a zombie $\rightarrow$ reaps it immediately
- If adopted child is running $\rightarrow$ waits and reaps upon termination

---

## Exercise – `exit()` vs `pthread_exit()`

**Key concept:** `exit()` terminates the **entire process**. `pthread_exit()` terminates only the calling thread.

```c
void* thread_A(void* arg) {
    printf("A1\n");
    printf("A2\n");
    return NULL;
}
void* thread_B(void* arg) {
    printf("B1\n");
    exit(0);        // intended to exit just this thread
    printf("B2\n");
    return NULL;
}
int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_A, NULL);
    pthread_create(&t2, NULL, thread_B, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Done\n");
    return 0;
}
```

### Solution

Possible outputs include:

- `B1`
- `A1, B1`
- `B1, A1`
- `A1, A2, B1`
- `A1, B1, A2`
- `B1, A1, A2`
- ... and other interleavings

Never printed:

- `B2` – after `exit()`, unreachable
- `Done` – process dies before main continues

**Why?** `exit(0)` terminates the **entire process**, killing all threads immediately.

**Fix:**

```c
void* thread_B(void* arg) {
    printf("B1\n");
    printf("B2\n");
    pthread_exit(NULL);
    // or: return NULL;
}
```

Now thread B exits properly; thread A and main continue. Output includes `Done`.

---

## Exercise – Fork Inside Thread

**Key concept:** A thread (not only the one running main) can call `fork()`. Only that thread survives in child.

```c
void* thread_func(void* arg) {
    printf("T1\n");
    if (fork() == 0) {
        printf("T-child\n");
        exit(0);
    }
    printf("T2\n");
    return NULL;
}

int main() {
    pthread_t t;
    pthread_create(&t, NULL, thread_func, NULL);
    printf("M\n");
    pthread_join(t, NULL);
    return 0;
}
```

**Question:** List all possible output orderings.

### Solution

Possible outputs (M can appear anywhere before join completes):

| Ordering 1 | Ordering 2 | Ordering 3 |
| --- | --- | --- |
| `M` | `T1` | `T1` |
| `T1` | `M` | `T-child` |
| `T-child` | `T2` | `T2` |
| `T2` | `T-child` | `M` |

Constraints:

- `T1` always before `T2` and `T-child` (sequential in thread)
- `T2` always after `T1` (parent continues after fork)
- `T-child` always after `T1` (child starts after fork)
- `M` can interleave with thread execution

**Key insight:** Child process has only the forking thread – main thread doesn't exist in child!

---

## Exercise – Multiple Threads + Fork

```c
void* print_A(void* arg) { printf("A\n"); return NULL; }
void* print_B(void* arg) { printf("B\n"); return NULL; }

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, print_A, NULL);
    if (fork() == 0) {
        pthread_create(&t2, NULL, print_B, NULL);
        printf("C\n");
        pthread_join(t2, NULL);
        exit(0);
    }
    printf("P\n");
    pthread_join(t1, NULL);
    return 0;
}
```

**Questions:**

1. Does the child process have thread `t1`?
2. What are the possible outputs?

### Solution

**Answer 1:** No! Child only has the "main" thread. Thread `t1` exists only in parent.

**Answer 2:** Possible outputs (many permutations):

| Ordering 1 | Ordering 2 | Ordering 3 |
| --- | --- | --- |
| `A` | `P` | `C` |
| `P` | `A` | `B` |
| `C` | `B` | `A` |
| `B` | `C` | `P` |

Constraints:

- `A` only in parent (`t1` doesn't survive fork to child)
- `B` only in child (`t2` created only in child)
- `P` only in parent, `C` only in child
- Parent: `A` and `P` can interleave
- Child: `B` and `C` can interleave
- Parent and Child run concurrently

---

## Process and Thread Communication

### Inter- and Intra-Process Communication

**Communication Models** — How do processes and threads exchange data?

- **Intra-process**: Threads within same process (shared memory by default)
- **Inter-process**: Between different processes (requires explicit mechanisms)
- Two main models:
  - **Shared Memory Model**
  - **Message Passing Model**

### Shared Memory Model

How it Works:

- Multiple processes or threads share a common memory space.
- They can read/write to the shared memory.
- Requires synchronization mechanisms (e.g., locks, semaphores).

*Figure: A central box labelled "Shared Memory" connected by three red arrows, each labelled R/W, to $t_i$ on the left, $t_j$ on the lower left, and $t_k$ on the right — every thread can both read and write the same memory region.*

### Message Passing Model

How it Works:

- Processes send messages to communicate.
- No shared memory is required.
- Concept is also used in distributed systems and computer networks.

*Figure: Circle $t_i$ — blue arrow labelled "Send" — box labelled $m$ (the message) — blue arrow labelled "Receive" — circle $t_j$; data moves as an explicit message rather than through common memory.*

*Only IPC model used in computer networks.*

### Shared Memory Details

**Memory Sharing Mechanisms**

- Threads in the same process share memory by default
- Different processes can share memory through **memory-mapped files**
- Used for fast communication, but requires synchronization

### Shared Memory Visualization: Threads Sharing Memory

*Figure: Process $P_1$ (Threads share memory) — a process box containing a vertical stack of segments Code, Data, Heap, Stack, OS Data; threads $t_1$, $t_2$ and $t_3$ sit outside on the left and all three arrows converge on the same point of the process's memory, showing that the threads share the one address space.*

---

<!-- source pages 36-48 -->

## The Synchronization Problem

### Threads Timing: Unpredictability

**Thread execution is non-deterministic.** Unless we explicitly synchronize threads, they can:

- Run in **any order**
- Start/Stop at **any time**
- Run/Stop for an **arbitrary amount of time**

*Figure: A time axis with five overlapping horizontal lines labelled Thread 1 … Thread 5, each starting and ending at a different point, some drawn as dashed segments (paused) that later resume as solid — threads execute at different times, may pause, resume, or stop unexpectedly.*

### Why Synchronization is Important?

Issues with uncontrolled thread execution:

- **Race Conditions**: Multiple threads modify shared data unpredictably
- **Deadlocks**: Threads waiting indefinitely for each other
- **Starvation**: Some threads may never get CPU time

### Thread Synchronization Mechanisms

Synchronization primitives:

- **Mutex (Mutual Exclusion)**: Ensures only one thread accesses a resource at a time
- **Semaphores**: Controls access to shared resources using counters
- **Locks**: Blocks other threads until the resource is free
- **Barriers**: Forces threads to wait at a specific point before proceeding
- **Atomic Operations**: Ensures single-step execution of critical updates

---

## Low-Level Synchronization

### What is an Atomic Operation?

**Atomic operation**: An operation that completes in a single step relative to other threads.

- Cannot be interrupted or left in an incomplete state (take one CPU cycle)
- Ensures consistency in concurrent environments
- Implemented at the hardware level using special CPU instructions
- Basis for implementing locks, semaphores, and other synchronization primitives.

### Non-Atomic vs Atomic: Pseudo-Assembly

Non-Atomic (3 steps):

```asm
LW   r1, counter   ; Load
ADD  r1, r1, 1
SW   r1, counter   ; Store
```

**Race Condition** — Thread can be interrupted between any of these steps $\rightarrow$ lost updates.

Atomic (1 step):

```asm
ATOMIC_ADD counter, 1
```

**Hardware Support** — Single indivisible instruction, cannot be interrupted.

Key: `LW` = Load Word, `SW` = Store Word

---

## Bank Account Interleaving

**What are the possible final balances?** Initial balance: **$100**. Two threads run concurrently (no synchronization):

Thread 1 – Deposit $100:

```asm
1a: LW   r1, balance
1b: ADD  r1, r1, 100
1c: SW   r1, balance
```

Thread 2 – Withdraw $50:

```asm
2a: LW   r2, balance
2b: SUB  r2, r2, 50
2c: SW   r2, balance
```

**Expected:** $100 + $100 - $50 = **$150**

**Question:** What values are actually possible? List all interleavings.

### Bank Account – Solution

**Possible final balances:** $50, $150, $200

| Interleaving | Result |
| --- | --- |
| 1a,1b,1c,2a,2b,2c | $150 (correct) |
| 2a,2b,2c,1a,1b,1c | $150 (correct) |
| 1a,2a,1b,2b,1c,2c | $50 (T2 overwrites T1) |
| 1a,2a,1b,2b,2c,1c | $200 (T1 overwrites T2) |

**Lost Update Problem**: Both threads load the **same** initial value, then one overwrites the other's result.

---

## Critical Sections

### Critical Section Requirements

**Critical section**: A section of code that accesses shared resources and must not be executed by more than one thread at a time.

Requirements for critical sections:

- **Mutual Exclusion**: No two processes are in their critical section at the same time
- **Deadlock-freedom**: If a process is trying to enter its critical section, then *some* process eventually enters a critical section
- **Starvation-freedom**: If a process is trying to enter its critical section, then *that* process must eventually enter its critical section

### Critical Section Structure

*Figure: A vertical flow of four boxes — remainder code → entry code → critical section → exit code — with an arrow curving from exit code back up to remainder code, forming a loop.*

- **Entry code**: Acquires lock, ensures only one thread enters
- **Critical section**: Access shared resources
- **Exit code**: Releases lock, allows others to enter

---

## Busy Wait / Spinlock / Wait

**Busy Wait** – repeatedly *polls* for a condition until met.

**Spinlock** – busy wait where CPU "spins" until lock is freed.

**Wait/Block** – thread sleeps until another thread wakes it (in practice we give an "hint" to the scheduler).

| | CPU | Overhead |
| --- | --- | --- |
| Busy wait | Wastes | No context switch |
| Block | Efficient | May context switch |

Busy Wait:

```c
while(locked) {
    // spin - wastes CPU
}
```

Wait/Block:

```c
while(locked) {
    sleep(1); // yield CPU
}
```

---

<!-- source pages 49-63 -->

## `stdatomic.h` — Overview

```c
#include <stdatomic.h>
```

(C11, compile with `-std=c11`)

**Types:** `atomic_int`, `atomic_bool`, `atomic_flag`

| Function | Returns | Description |
|---|---|---|
| `atomic_load(&x)` | value | Read atomically |
| `atomic_store(&x, val)` | void | Write atomically |
| `atomic_fetch_add(&x, val)` | old value | Add, return old |
| `atomic_fetch_sub(&x, val)` | old value | Subtract, return old |
| `atomic_exchange(&x, val)` | old value | Swap, return old |
| `atomic_compare_exchange_strong` | bool | CAS operation |
| `atomic_flag_test_and_set(&f)` | old (bool) | Set flag, return old |
| `atomic_flag_clear(&f)` | void | Clear flag |

### Atomic Operations — Basic Examples

```c
#include <stdatomic.h>

atomic_int counter = 0;

// Atomic read
int val = atomic_load(&counter);          // val = 0

// Atomic write
atomic_store(&counter , 10);              // counter = 10

// Atomic add (returns OLD value)
int old = atomic_fetch_add(&counter , 5); // old = 10,
//counter = 15

// Atomic subtract (returns OLD value)
old = atomic_fetch_sub(&counter , 3);     // old = 15,
//counter = 12
```

### Atomic Operations — Exchange

```c
atomic_int x = 5;

// Exchange: swap and return old value
int old = atomic_exchange(&x, 20);  // old = 5, x = 20
```

### Atomic Operations — CAS

```c
atomic_int x = 5;
int expected = 5;
int desired = 10;

// If x == expected: set x = desired, return true
// Else: set expected = x, return false
if (atomic_compare_exchange_strong(&x, &expected, desired))
    // Success: x was 5, now x = 10
} else {
    // Failed: x != 5, expected now contains
    // actual value of x
}
```

*Note: on the slide the opening brace of the `if` is missing (the line runs off the right edge of the slide); transcribed as printed.*

### `atomic_flag` — Spinlock Primitive

- Guaranteed lock-free boolean (only atomic type with this guarantee)
- Two operations: `test_and_set` and `clear`

**Spinlock Example:**

```c
#include <stdatomic.h>

atomic_flag lock = ATOMIC_FLAG_INIT;  // Initialize

void acquire() {
    while (atomic_flag_test_and_set(&lock)) // Returns old
        ;  // Spin until we get false (was unlocked)
}

void release() {
    atomic_flag_clear(&lock);
}
```

---

## Exercise — Bank Account: Safe Withdrawal

**Recall:** The bank account problem showed race conditions with concurrent deposits/withdrawals.

**Extension:** Now we add a balance check before withdrawal. Is this safe?

```c
int balance = 100;

bool withdraw(int amount) {
    if (balance >= amount) {   // CHECK
        balance -= amount;     // ACT
        return true;
    }
    return false;
}
// Thread 1: withdraw(80)
// Thread 2: withdraw(80)
```

**Questions:**

1. Can both withdrawals succeed? What's the final balance?
2. Fix using `atomic_compare_exchange_strong` (CAS loop)

### Solution — Bank Account: Safe Withdrawal

**The Bug:**

1. T1: checks `100 >= 80` $\rightarrow$ true
2. T2: checks `100 >= 80` $\rightarrow$ true
3. T1: `balance = 100 - 80 = 20`
4. T2: `balance = 20 - 80 = -60`

Both succeed, balance goes **negative**!

**Problem:** Check-then-act is not atomic. Another thread can modify balance between check and act.

**Fix with CAS:**

```c
#include <stdatomic.h>
atomic_int balance = 100;

bool withdraw(int amount) {
    int old = atomic_load(&balance);
    while (old >= amount) {
        // Try to update balance atomically
        if (atomic_compare_exchange_strong(
                &balance, &old, old - amount))
            return true;
        // CAS failed: &old automatically
        // updated to current balance
        // (CAS semantics), retry loop
    }
    return false;
}
```

Compare uses locals (safe). On CAS fail, `old` auto-updated to current value.

---

## The Optimization Problem

> **Why Optimizations Can Break Concurrent Code**
>
> Compilers and CPUs aggressively optimize for performance, but these optimizations assume single-threaded execution.

**Compiler Optimizations:**

- Caches variables in registers
- Removes "redundant" memory reads
- Assumes variables don't change externally

**CPU Optimizations:**

- Reorders instructions for pipeline efficiency
- Writes may not be immediately visible
- Out-of-order execution

### `volatile` — Preventing Compiler Caching

**Problem:**

```c
int flag = 0;

void wait_for_flag() {
    while (!flag);
    // Compiler may optimize to:
    // if (!flag) while(1);
    // (caches flag in register)
}

void signal() {
    flag = 1;  // Never seen!
}
```

**Solution:**

```c
volatile int flag = 0;

void wait_for_flag() {
    while (!flag);
    // Compiler MUST read flag
    // from memory each iteration
}

void signal() {
    flag = 1;  // Now visible!
}
```

> **Important**
>
> `volatile` does **NOT** guarantee atomicity or prevent CPU reordering!

### Memory Fences — Preventing Instruction Reordering

**Problem:**

```c
int data = 0;
int flag = 0;

void writer() {
    data = 42;
    flag = 1;
    // CPU may reorder:
    // flag = 1 BEFORE data = 42!
}

void reader() {
    while (!flag);
    print(data);   // May print 0!
}
```

**Solution:**

```c
int data = 0;
atomic_int flag = 0;

void writer() {
    data = 42;
    atomic_thread_fence(
        memory_order_release);
    atomic_store(&flag, 1);
}

void reader() {
    while (!atomic_load(&flag));
    atomic_thread_fence(
        memory_order_acquire);
    print(data);   // Always 42!
}
```

### Memory Fence Semantics

**Release (Writer Side)**
All writes **before** the fence are guaranteed to be visible **before** any write after the fence.
*"Release my changes to other threads"*

**Acquire (Reader Side)**
All reads **after** the fence see all writes that happened **before** a matching release fence.
*"Acquire changes from other threads"*

#### Memory Fence Semantics Example

*Figure: Two columns, Writer and Reader. Writer executes `data = 42`, then `RELEASE fence`, then `flag = 1`. Reader executes `wait for flag`, then `ACQUIRE fence`, then `read data`. A red arrow labelled "sync" runs from the writer's `flag = 1` box to the reader's `wait for flag` box, showing that the release/acquire pair is what makes the writer's earlier `data = 42` visible to the reader.*

---

## Exercise — Does This Code Work?

```c
typedef struct { int value; int id; } Object;

Object* obj = NULL;
int initialized = 0;

void init_thread() {
    obj = malloc(sizeof(Object));
    obj->value = 42;
    obj->id = 1;
    initialized = 1;
}

void use_thread() {
    while (!initialized);     // wait for init
    printf("value=%d\n", obj->value);
}
```

### Exercise — Solution

**Answer:** No! CPU may reorder `initialized = 1` before `obj` is fully set up. **Possible bugs:** `obj` is NULL, or `obj->value` is garbage.

**Fixed code:**

```c
Object* obj = NULL;
atomic_int initialized = 0;

void init_thread() {
    obj = malloc(sizeof(Object));
    obj->value = 42;
    obj->id = 1;
    atomic_thread_fence(memory_order_release);  // ensure writes complete
    atomic_store(&initialized , 1);
}

void use_thread() {
    while (!atomic_load(&initialized));
    atomic_thread_fence(memory_order_acquire);  // ensure we writes
    printf("value=%d\n", obj->value);           // always 42!
}
```

*Note: the comment `// ensure we writes` appears with that wording on the slide.*

---

## Synchronization Primitives

*Section divider slide.*

---

<!-- source pages 64-78 -->

## Semaphores

**Semaphores Overview**

- Semaphores control access to shared resources in concurrent systems
- Prevents race conditions and ensures safe access

**Binary Semaphore** — Only two states (locked/unlocked). Also known as a **Mutex**.

**Counting Semaphore** — Allows multiple accesses up to a specified limit.

### How Semaphores Work?

Operations:

- `wait()` (**P operation**): Decrements semaphore; blocks if value becomes negative.
- `signal()` (**V operation**): Increments semaphore; unblocks waiting process.

**Bounded Semaphore** — Has a maximum value limit.

- `signal()` fails/blocks if at max
- Prevents overflow
- Example: limited resource pool

**Unbounded Semaphore** — No maximum value limit.

- `signal()` always succeeds
- Can grow indefinitely
- Example: counting events

### Semaphore Mechanics

*Figure: A semaphore box holding `count=2`, with `wait()` entering from the left (if count=0 → block, else count−−) and `signal()` leaving to the right (count++, if waiters → wake one); below it a Waiting Queue containing T3, T4, … linked to the semaphore by a "block" arrow going down and a "wake" arrow coming back up.*

- **count > 0**: Resources available, `wait()` decrements and proceeds.
- **count = 0**: No resources, `wait()` blocks thread in queue.

### Semaphore Example in C

```c
sem_t sem;
int shared_resource = 0;

void* worker(void* arg) {
    long id = (long)arg;
    sem_wait(&sem);  // P: acquire
    printf("T%ld: enter (res=%d)\n", id, shared_resource);
    shared_resource++;
    sleep(1);
    printf("T%ld: leave (res=%d)\n", id, shared_resource);
    sem_post(&sem);  // V: release
    return NULL;
}

int main() {
    pthread_t t[3];
    // sem_init(sem, pshared, value): 0=local, 1=binary
    sem_init(&sem, 0, 1);
    for (long i = 0; i < 3; i++)
        pthread_create(&t[i], NULL, worker, (void*)i);
    for (int i = 0; i < 3; i++)
        pthread_join(t[i], NULL);
    sem_destroy(&sem);
}
```

Output:

```
T0: enter (res=0)
T0: leave (res=1)
T1: enter (res=1)
T1: leave (res=2)
T2: enter (res=2)
T2: leave (res=3)
```

Key points:

- Only one thread in critical section at a time
- `shared_resource` increments safely
- Threads execute sequentially

---

## Exercise – Connection Pool Synchronization

**Problem:** A server has a pool of **N=3** database connections. Multiple threads handle requests, each needing one connection.

```c
#define MAX_CONNECTIONS 3
int active_connections = 0;

void handle_request(int thread_id) {
    // Acquire connection
    active_connections++;
    printf("T%d: using connection (%d active)\n",
           thread_id, active_connections);
    sleep(1);  // Use connection
    // Release connection
    active_connections --;
}
```

Questions:

1. What's wrong with this code?
2. How would you fix it with a mutex? What's the limitation?
3. How would an unbounded semaphore help? What's still wrong?

### Solution – Attempt 1 (Mutex)

```c
pthread_mutex_t lock;
int active_connections = 0;

void handle_request(int id) {
    pthread_mutex_lock(&lock);
    active_connections++;
    printf("T%d: using (%d active)\n",
           id, active_connections);
    sleep(1);
    active_connections --;
    pthread_mutex_unlock(&lock);
}
```

Problem:

- Only **1 thread** can use a connection at a time
- We have 3 connections but only use 1!
- Throughput: 1 request/second

**Mutex is too restrictive** – we want up to N concurrent accesses, not just 1.

### Solution – Attempt 2 (Unbounded Semaphore)

```c
sem_t sem;
// Initialize with 3
sem_init(&sem, 0, 3);

void handle_request(int id) {
    sem_wait(&sem);
    // ... use connection ...
    sem_post(&sem);
}
// But what if buggy code does
// extra sem_post()?
```

Problem:

- Semaphore starts at 3 (✓)
- But `sem_post()` can increment **beyond 3**
- Bug or malicious code could call extra `post()`
- Result: More than 3 concurrent connections!

**Unbounded semaphore** – value can grow past the limit.

### Solution – Bounded Semaphore

Conceptual bounded semaphore:

```c
typedef struct {
    sem_t sem;
    sem_t limit;   // Prevents overflow
    int max;
} bounded_sem_t;

void bounded_wait(bounded_sem_t* bs) {
    sem_wait(&bs->sem);
}
void bounded_post(bounded_sem_t* bs) {
    // Only post if below max
    if (current < bs->max)
        sem_post(&bs->sem);
}
```

Correct behavior:

- Max 3 concurrent connections
- `wait()` blocks at 0
- `post()` **cannot exceed** max
- Extra `post()` calls ignored or error

**Bounded semaphore** – enforces both lower (0) and upper (N) bounds.

*Note: POSIX `sem_t` is unbounded. Some systems provide bounded variants.*

---

## Read-Write Locks

**Read-Write Locks** allow multiple threads to read concurrently while ensuring exclusive write access. Efficient for read-heavy workloads.

**Read Lock (rdlock)**

- Multiple readers simultaneously
- Blocks only if writer holds lock
- `pthread_rwlock_rdlock()`

**Write Lock (wrlock)**

- Only one writer at a time
- Blocks readers and writers
- `pthread_rwlock_wrlock()`

### Read-Write Lock Mechanics

*Figure: Readers R1 and R2 succeed with `rdlock()` into an "RW Lock" box showing `readers=2, writer=0`, while writer W1's `wrlock()` is blocked and is routed instead into a Wait Queue holding W, R, ….*

| State | Behavior |
|---|---|
| readers=0, writer=0 | `rdlock()` and `wrlock()` succeed |
| readers>0, writer=0 | `rdlock()` succeeds, `wrlock()` blocks |
| writer=1 | Both `rdlock()` and `wrlock()` block |

**Queue Priority Policies:** *Reader priority* (readers first, may starve writers), *Writer priority* (writers first, may starve readers), *Fair* (FIFO order).

### Read-Write Lock Example in C

```c
pthread_rwlock_t rwlock;
int data = 0;

void* reader(void* arg) {
    long id = (long)arg;
    pthread_rwlock_rdlock(&rwlock);
    printf("R%ld: read data=%d\n", id, data);
    sleep(1);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void* writer(void* arg) {
    long id = (long)arg;
    pthread_rwlock_wrlock(&rwlock);
    data++;
    printf("W%ld: wrote data=%d\n", id, data);
    sleep(1);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

int main() {
    pthread_t r1, r2, w1;
    pthread_rwlock_init(&rwlock, NULL);
    pthread_create(&r1, NULL, reader, (void*)1);
    pthread_create(&r2, NULL, reader, (void*)2);
    pthread_create(&w1, NULL, writer, (void*)1);
    ............
```

Possible output:

```
R1: read data=0
R2: read data=0
W1: wrote data=1
```

Key points:

- R1 and R2 can read concurrently
- W1 waits until all readers finish
- Writer has exclusive access

### Where Are Read-Write Locks Used?

- **Databases:** Allow multiple read queries while preventing write conflicts.
- **File Systems:** Multiple processes can read a file, but writing is exclusive.
- **Caching Mechanisms:** Multiple threads can read cached data, but updating requires exclusive access.

---

## Exercise – Writer Starvation

**Scenario:** A cache system with reader-priority RW lock. Readers arrive continuously.

```c
pthread_rwlock_t rwlock;
int cache_data = 0;

void* reader(void* arg) {
    while (1) {
        pthread_rwlock_rdlock(&rwlock);
        printf("Read: %d\n", cache_data);
        usleep(100000);  // 100ms hold time
        pthread_rwlock_unlock(&rwlock);
        usleep(50000);   // 50ms between reads
    }
}
void* writer(void* arg) {
    pthread_rwlock_wrlock(&rwlock);
    cache_data++;
    printf("Write: %d\n", cache_data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}
int main() {
    // Start 5 readers , then 1 writer
    for (int i = 0; i < 5; i++)
        pthread_create(&readers[i], NULL, reader, NULL);
    sleep(1);
    pthread_create(&writer_thread , NULL, writer, NULL);
    ...
}
```

### Writer Starvation – The Problem

What happens:

1. 5 readers continuously acquire `rdlock`
2. Short gap between unlock and next lock
3. Always $\geq 1$ active reader
4. Writer waits for readers=0
5. **Writer starves indefinitely!**

Timeline:

- R1, R2, R3 hold lock
- R1 releases, R4 acquires
- R2 releases, R5 acquires
- R3 releases, R1 acquires
- Writer never gets a chance...

*Figure: A time axis with a row of nine discrete, evenly spaced green reader blocks above it — R1, R2, R3, R4, R5, R1, R2, R3, R4, separated by small gaps — and beneath the axis one continuous red bar spanning its whole length labelled "Writer waiting...".*

### Writer Starvation – Solutions

**Actual Solutions:**

1. **Fair (FIFO) lock:** Requests served in arrival order – no starvation
2. **Application-level backoff:** All entities (readers **and** writers) back off periodically to avoid collision and give others a chance

**Shifts problem: Writer-priority lock**

Blocks new readers when writer waits.

*But:* Continuous writers → **readers starve instead!** Just moves starvation, doesn't solve it.

**NOT a solution: Timeout (`timedwrlock`)**

Releases writer from blocking.

*But:* Writer **still doesn't write** – just gives up! Operation fails, starvation not solved.

---

<!-- source pages 79-98 -->

## Exercise – Lock Upgrade Deadlock

**Scenario:** Thread reads data, decides it needs to modify, tries to "upgrade" to write lock.

```c
pthread_rwlock_t rwlock;
int data = 0;

void* read_then_write(void* arg) {
    // First, read the data
    pthread_rwlock_rdlock(&rwlock);
    int val = data;
    printf("Read: %d\n", val);

    if (val < 10) {
        // Need to update -- try to get write lock
        pthread_rwlock_wrlock(&rwlock);  // ???
        data = val + 1;
        printf("Wrote: %d\n", data);
        pthread_rwlock_unlock(&rwlock);
    }
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

int main() {
    pthread_create(&t1, NULL, read_then_write , NULL);
    pthread_create(&t2, NULL, read_then_write , NULL);
    ...
}
```

**Question:** What happens when `wrlock` is called? Is this code correct?

### Solution – Lock Upgrade Deadlock

**The Deadlock:**

1. T1: acquires `rdlock`
2. T2: acquires `rdlock` (allowed!)
3. T1: tries `wrlock` → **blocks** (T2 holds `rdlock`)
4. T2: tries `wrlock` → **blocks** (T1 holds `rdlock`)
5. **Both wait forever!**

**Key insight:** Cannot upgrade `rdlock` → `wrlock` while holding `rdlock`. Must release first, but then state may change.

**Fix – Release and Re-acquire:**

```c
void* read_then_write(void* arg) {
    pthread_rwlock_rdlock(&rwlock);
    int val = data;
    pthread_rwlock_unlock(&rwlock);
    // Lock released!

    if (val < 10) {
        pthread_rwlock_wrlock(&rwlock);
        // Re-check: data may have changed!
        if (data < 10) {
            data++;
        }
        pthread_rwlock_unlock(&rwlock);
    }
    return NULL;
}
```

**Note:** Must re-check condition after acquiring `wrlock` – another thread may have modified data.

### Alternative Solution – Use Mutex Instead

**Simple fix: Replace rwlock with mutex**

```c
pthread_mutex_t lock;
int data = 0;

void* read_then_write(void* arg) {
    pthread_mutex_lock(&lock);
    int val = data;

    if (val < 10) {
        data = val + 1;
        printf("Wrote: %d\n", data);
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}
```

No upgrade needed – we already have exclusive access. No deadlock possible.

### Mutex vs RWLock – Trade-offs

**Mutex Advantages**

- No upgrade deadlock
- Simpler code
- No re-check needed
- Lower overhead

**Mutex Disadvantage**

- **No concurrent reads!** Only 1 thread at a time, even for read-only operations.

**When to use Mutex:**

- Read-modify-write is common
- Critical section is short
- Simplicity over performance

**When to use RWLock:**

- Read-heavy workload
- Reads are long operations
- Writes are rare
- Need maximum read throughput

---

## Condition Variables

**Condition Variables** allow threads to wait for a specific condition efficiently. Used with a mutex to avoid busy-waiting.

**How it works:**

1. Lock mutex, check condition
2. If false: `wait()` releases mutex and blocks
3. Another thread signals the condition
4. Waiting thread wakes and reacquires mutex

**Key functions:**

- `pthread_cond_wait(&cv, &mtx)`
- `pthread_cond_signal(&cv)`
- `pthread_cond_broadcast(&cv)`

**Note:** Always check condition in a `while` loop (spurious wakeups).

### Condition Variable Mechanics

*Figure: Two parallel flowcharts. Waiter Thread: 1. lock(mutex) → 2. while(!cond) → 3. wait(&cv, &mtx) → BLOCKED → 5. continue... Signaler Thread: lock(mutex) → cond = true → 4. signal(&cv) → unlock(mutex). A dashed "releases mutex" arrow runs from the waiter's wait() step to the signaler's lock(mutex); a "wake + reacquire" arrow runs from the signaler's signal(&cv) back to the waiter's "5. continue...".*

**Key:** `wait()` atomically releases mutex and blocks. On wakeup, reacquires mutex before returning.

### Condition Variable Example in C

```c
pthread_mutex_t mtx;
pthread_cond_t cv;
int data_ready = 0;

void* waiter(void* arg) {
    long id = (long)arg;
    pthread_mutex_lock(&mtx);
    while (!data_ready) {          // check condition
        printf("W%ld: waiting...\n", id);
        pthread_cond_wait(&cv, &mtx);   // block + release
    }
    printf("W%ld: got signal, data_ready=%d\n", id, data_ready);
    pthread_mutex_unlock(&mtx);
    return NULL;
}

void* signaler(void* arg) {
    sleep(1);   // simulate work
    pthread_mutex_lock(&mtx);
    data_ready = 1;
    printf("S: signaling...\n");
    pthread_cond_signal(&cv);          // wake one waiter
    pthread_mutex_unlock(&mtx);
    return NULL;
}

int main() {
    // init mutex, condition varible and run the threads
}
```

**Output:**

```
W1: waiting...
S: signaling...
W1: got signal, data_ready=1
```

**Key points:**

- `wait()`: release → block → reacquire → return
- `signal()`: wake one
- `broadcast()`: wake all (compete for mutex one-by-one)
- `while` loop: re-check condition after wake

---

## Exercise – Two Conditions, One CV

**Scenario:** Counter with incrementing and decrementing threads sharing ONE condition variable.

```c
pthread_cond_t cv;   // ONE cv for both conditions!
pthread_mutex_t mtx;
int count = 0;
#define MAX 5

void* increment(void* arg) {
    pthread_mutex_lock(&mtx);
    while (count == MAX)        // wait until not full
        pthread_cond_wait(&cv, &mtx);
    count++;
    printf("Incremented: count=%d\n", count);
    pthread_cond_signal(&cv);   // signal ONE waiter
    pthread_mutex_unlock(&mtx);
    return NULL;
}

void* decrement(void* arg) {
    pthread_mutex_lock(&mtx);
    while (count == 0)          // wait until not empty
        pthread_cond_wait(&cv, &mtx);
    count--;
    printf("Decremented: count=%d\n", count);
    pthread_cond_signal(&cv);   // signal ONE waiter
    pthread_mutex_unlock(&mtx);
    return NULL;
}
```

**Question:** With multiple producers and consumers, can this deadlock?

### Solution – Two Conditions, One CV

**The Deadlock Scenario:**

1. `count=5` (max)
2. I1, I2 wait on CV (count at Max)
3. D1 decrements, signals CV
4. **I1 wakes**
5. I1 increments, count at max
6. I1 signals → **I2 wakes**
7. I2 sees count at max, waits again
8. D2 waiting, never signaled
9. **Deadlock!**

**Problem:** Signal wakes *any* waiter, not necessarily one whose condition is true.

**Solutions:**

1. **Use `broadcast()`:** Wake all, let them re-check
2. **Separate CVs:** `not_max` and `not_zero` (proper design)

**Rule:** Use separate CVs for different conditions, or use `broadcast()`.

---

## Exercise – Resource Pool with Cancellation

**Scenario:** Thread pool requests network connections.

```c
pthread_cond_t cv;
pthread_mutex_t mtx;
int available_connections = 0;

typedef struct { int id; int cancelled; } Request;

void* worker(void* arg) {
    Request* req = (Request*)arg;
    pthread_mutex_lock(&mtx);
    while (available_connections == 0 && !req->cancelled)
        pthread_cond_wait(&cv, &mtx);

    if (req->cancelled) {
        printf("W%d: request cancelled, giving up\n", req->id);
        pthread_mutex_unlock(&mtx);
        return NULL;   // Don't use connection
    }
    available_connections --;
    printf("W%d: got connection\n", req->id);
    pthread_mutex_unlock(&mtx);
    return NULL;
}

void release_connection() {
    pthread_mutex_lock(&mtx);
    available_connections++;
    pthread_cond_signal(&cv);   // Wake ONE waiter
    pthread_mutex_unlock(&mtx);
}
```

### Solution – Resource Pool with Cancellation

**The Starvation Scenario:**

1. W1, W2, W3 wait for connection
2. W1's request gets cancelled
3. Connection released, `signal()` called
4. **W1 wakes** (unlucky choice)
5. W1 sees cancelled, exits without using connection
6. W1 does **not** re-signal!
7. Connection available but W2, W3 still blocked
8. **Resource wasted, others starve**

**Problem:** Woken thread doesn't consume resource but doesn't notify others.

**Solutions:**

1. **Use `broadcast()`:** All waiters check their status
2. **Re-signal on exit:**

```c
if (req->cancelled) {
    // Pass the signal to others!
    pthread_cond_signal(&cv);
    pthread_mutex_unlock(&mtx);
    return NULL;
}
```

**Rule:** If woken but not consuming, either broadcast or re-signal.

---

## Classic Synchronization Problems

### Producer-Consumer Problem

**Producer-Consumer Problem** — classic synchronization problem: producers generate data, consumers retrieve data, both share a bounded buffer.

**Challenges**

- Buffer full → producer waits
- Buffer empty → consumer waits
- Concurrent access → race conditions

**Solutions**

- Mutex for mutual exclusion
- Condition variable: `not_full`
- Condition variable: `not_empty`

### Producer-Consumer: Step-by-Step Walkthrough

**Step 1 – Initial State**

*Figure: Producer box marked "Ready", a 5-cell Buffer (max=5) all empty with `count = 0 (EMPTY)`, and a Consumer box marked "Waiting..." with a dashed `wait()` arrow pointing to the `not_empty` condition variable.*

Buffer empty: Consumer calls `wait(not_empty)` and blocks.

**Step 2 – Producer Adds Data**

*Figure: Producer marked "Producing..." with a solid arrow into the buffer, whose first three cells hold 1, 2, 3 and `count = 3`; a dashed `signal()` arrow goes from Producer to `not_empty`, and a dashed `wake` arrow goes from `not_empty` to the Consumer (still "Waiting...").*

Producer: Adds items to buffer, then calls `signal(not_empty)` to wake consumer.

**Step 3 – Consumer Wakes & Consumes**

*Figure: Producer back to "Ready", buffer holds a single item 1 with `count = 1`, and an arrow runs from the buffer to the Consumer, now marked "Consuming...".*

Consumer: Wakes up, reacquires mutex, consumes items from buffer.

**Step 4 – Buffer Full**

*Figure: Producer marked "Waiting...", buffer completely filled with 1,2,3,4,5 and `count = 5 (FULL)` highlighted in red, Consumer marked "Consuming..." draining the buffer; a dashed `signal()` arrow goes from Consumer to `not_full`, and a dashed `wake` arrow goes from `not_full` back to the Producer.*

Buffer full: Producer calls `wait(not_full)` and blocks. Consumer signals `not_full`.

### Producer-Consumer Example in C

```c
#define MAX 5
int buffer[MAX], count = 0;
pthread_mutex_t mtx;
pthread_cond_t not_full, not_empty;

void* producer(void* arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mtx);
        while (count == MAX)            // buffer full?
            pthread_cond_wait(&not_full, &mtx);
        buffer[count++] = i;
        printf("P: produced %d (count=%d)\n", i, count);
        pthread_cond_signal(&not_empty);  // wake consumer
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mtx);
        while (count == 0)              // buffer empty?
            pthread_cond_wait(&not_empty, &mtx);
        int val = buffer[--count];
        printf("C: consumed %d (count=%d)\n", val, count);
        pthread_cond_signal(&not_full);   // wake producer
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}
```

**Sample output:**

```
P: produced 0 (count=1)
P: produced 1 (count=2)
C: consumed 1 (count=1)
C: consumed 0 (count=0)
P: produced 2 (count=1)
...
```

**Key points:**

- Two condition variables
- `while` loops for conditions
- Producer signals consumer
- Consumer signals producer

### Producer-Consumer: Correctness Analysis

**Race-Free**

Why:

- All access to `buffer`, `count` protected by mutex
- `wait()` atomically releases mutex and blocks
- `while` loop re-checks condition after wake

*Invariant:* Only one thread modifies shared state at any time.

**Deadlock-Free**

Why:

- Single mutex (no circular wait possible)
- `wait()` releases mutex while blocking
- No nested locking

*Key insight:* Blocked thread always releases mutex, allowing others to proceed and signal.

**Starvation-Free**

Why:

- Separate CVs: `not_full`, `not_empty`
- Producer signals `not_empty` (wakes consumer)
- Consumer signals `not_full` (wakes producer)

*Key insight:* Each signal targets the correct waiter.

---

## See You Next Week!

Questions?
