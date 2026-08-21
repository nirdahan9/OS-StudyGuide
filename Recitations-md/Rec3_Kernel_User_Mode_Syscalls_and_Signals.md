# Recitation 3 — Kernel/User Mode, System Calls, Interrupts and Signals

> Study notes converted from the recitation slide deck `Rec3_OS.pdf` (107 pages) — Operating Systems, Reichman University, Spring Semester 2026.
> Lecturer: Tsvi Cherny-Shahar · TAs: Daniel Karalnik, Liam Tal.
> Faithful conversion: every substantive item on the source pages appears here, and nothing has been added from outside them. Figures are rendered as italic *Figure:* descriptions.
> `<!-- source pages a-b -->` comments mark the original page ranges.

---

<!-- source pages 1-10 -->

## Recitation 3 — Kernel and User Mode

### Table of Contents

1. OS Intro Continued
2. Kernel Mode and User Mode
3. System Calls
4. Interrupts and Exceptions
5. Signals
6. User Mode Threading

---

## OS Intro Continued

### Monolithic Operating Systems

> **Early System Design** — In early monolithic operating systems, there was minimal separation between system and application code.

- Single process post-boot, often a terminal
- Early systems (e.g., Atari) ran directly on hardware
- Crashes in any program could crash the system
- Direct memory access increased vulnerability to bugs

> **Security Risk** — Without isolation, any buggy or malicious code could access and corrupt any part of memory!

---

## Kernel Mode and User Mode

### Kernel Mode vs User Mode

> **Protection Rings** — Modern CPUs provide hardware-enforced privilege levels to isolate critical system code from applications.

*Figure: Two parallel columns. **Kernel Mode (Ring 0,1,2)** points down to a box "Drivers, Core OS Functions", captioned "Crashes affect entire system"; **User Mode (Ring 3)** points down to a box "Applications (e.g., Browsers, Games)", captioned "Crashes isolated to app". The point: privilege level determines both what kind of code runs there and how far a crash propagates — a fault in Ring 3 is contained to the application, while one in Ring 0,1,2 takes down the whole system.*

### Kernel Mode vs User Mode — Comparison

| Kernel Mode | User Mode |
| --- | --- |
| Can execute privileged instructions | Limited hardware access |
| Privileged memory access | Cannot execute privileged instructions |
| Manages all resources | Application memory only |
| | Must request resources via syscalls |

> **Key Insight** — The CPU hardware enforces these restrictions (with the CPL) — attempting privileged operations in user mode triggers an exception!

---

## What is a Trap?

> **Definition** — A **trap** is a **synchronous** event triggered by the currently executing instruction that transfers control to the kernel.

**Types of Traps**

- **System calls** — intentional trap to request kernel service
- **Exceptions** — errors during execution (div by zero, page fault)
- **Breakpoints** — debugging (`int 3`)

**Key Property**

**Synchronous** means:

- Caused by the instruction itself
- Happens at a predictable point
- Same instruction $=$ same trap

(Unlike interrupts, which are asynchronous)

### What is a Trap Handler?

> **Trap Handler** — The **trap handler** is kernel code that executes when a trap occurs. It determines what caused the trap and dispatches to the appropriate service routine.

*Figure: Three inputs on the left — `syscall`, `exception`, `breakpoint` — all arrow into a central "Trap Handler" box whose numbered steps are: 1. Save CPU state, 2. Identify trap cause, 3. Dispatch to service, 4. Restore state, 5. Return to user. From it, three arrows fan out on the right to `syscall_dispatch`, `exception_handler` and `debug_handler`. The point: one common entry path, then dispatch by cause.*

> **Key Insight** — The trap handler is the **entry point** into the kernel — the gatekeeper for all synchronous kernel requests.

---

<!-- source pages 11-21 -->

## System Calls

### Overview

*Figure: A two-band diagram. The upper band, **User Mode (Ring 3)**, contains an `App` box with the caption "Open file — creates a system call"; a side note reads "Any access to Kernel is done via System Call". An arrow labelled "`syscall` or `int 0x80` with code in RAX" crosses the dashed Ring 3 / Ring 0 boundary into the lower band, **Kernel Mode (Ring 0)**, where it reaches `trap_handler()`, which flows down to `syscall_dispatch(X)`, which in turn fans out to "Filesystem drivers", "Network stack" and "Disk Driver" — the last one connected to an HDD icon.*

- Any access to the kernel is done via a system call.
- The transition is made with the `syscall` instruction or `int 0x80`, with the syscall code in `RAX`.

### How It Works

**Step-by-step flow:**

1. Application calls library function (e.g., `open()`)
2. Library places syscall number in `RAX` register
3. Executes `syscall` instruction (or `int 0x80`)
4. CPU finds entry point:
   - `int 0x80`: looks up **IDT** entry 128
   - `syscall`: reads **MSR_LSTAR** directly (faster)
5. CPU switches from Ring 3 to Ring 0, jumps to handler
6. Trap handler saves state, calls syscall dispatch
7. Syscall dispatch looks up handler in syscall table
8. Handler executes, result returned in `RAX`
9. CPU returns to Ring 3, execution continues

**No context switch!** A system call is **not** a context switch — the same thread transitions from Ring 3 to Ring 0. The process remains the same; only the privilege level changes.

### Syscall Dispatch and the Syscall Table

**Routing to the right handler:** **Syscall dispatch** takes the syscall number (in `RAX`) and calls the corresponding kernel function from a **syscall table**.

*Figure: `RAX = 1` feeds an arrow labelled "index" into the array `sys_call_table[]`, whose entries are listed as `[0] sys_read`, `[1] sys_write`, `[2] sys_open`, `[3] sys_close`, `...`. Entry `[1]` is highlighted and an arrow leaves it to `sys_write()`.*

Common syscalls: 0=read, 1=write, 2=open, 3=close, 39=getpid, 57=fork, 60=exit, 62=kill

---

## Legacy Route: `int 0x80`

**Interrupt-based system call:** the legacy method uses a software interrupt — goes through the **general trap handler**.

- User program executes `int 0x80` instruction
- CPU looks up entry 0x80 in the **IDT** (Interrupt Descriptor Table)
- Jumps to the **general trap handler** (`system_call`)
- Trap handler identifies it as a syscall and calls `syscall_dispatch`
- Dispatch uses RAX to index into syscall table $\rightarrow$ `sys_*()`

**Warning — Overhead:** IDT memory lookup + general trap handler processing

### Flow

*Figure: `int 0x80` sits in the **User Mode (Ring 3)** band; an arrow (step 1) crosses the dashed boundary into **Kernel Mode (Ring 0)** to the box **IDT / Entry 0x80**, then arrow 2 to **Trap Handler** (`system_call`), then arrow 3 to `syscall_dispatch`.*

`int 0x80` $\rightarrow$ IDT $\rightarrow$ **Trap Handler** $\rightarrow$ `syscall_dispatch` $\rightarrow$ `sys_*()`

---

## Modern Route: `syscall` Instruction

**Fast system call entry:** modern x86-64 CPUs provide a dedicated `syscall` instruction that **bypasses both the IDT and the general trap handler**.

- User program executes `syscall` instruction
- CPU reads target address directly from **MSR_LSTAR** register
- Jumps to **dedicated syscall entry** (`entry_SYSCALL_64`)
- This entry point goes directly to `syscall_dispatch`
- Dispatch uses RAX to index into syscall table $\rightarrow$ `sys_*()`

**Key optimization:** Skips IDT lookup AND general trap handler

### Flow

*Figure: `syscall` sits in the **User Mode (Ring 3)** band; an arrow (step 1) crosses the dashed boundary into **Kernel Mode (Ring 0)** to the box **MSR_LSTAR**, then arrow 2 to **Dedicated Entry** (`entry_SYSCALL_64`), then arrow 3 to `syscall_dispatch`.*

`syscall` $\rightarrow$ MSR $\rightarrow$ **Dedicated Entry** $\rightarrow$ `syscall_dispatch` $\rightarrow$ `sys_*()`

---

## The Kernel Stack

**Why a separate stack?** Each thread has **two stacks**: a user stack and a kernel stack. The kernel cannot trust the user stack!

*Figure: Two stack boxes side by side. The **User Stack** (Ring 3, large, e.g., 8MB) holds "local variables", "function args", "return addresses", "…". The **Kernel Stack** (Ring 0, small, e.g., 8-16KB) holds "saved user RSP", "saved user RIP", "saved registers", "kernel locals", "…". An arrow labelled `syscall` goes from the user stack to the kernel stack and an arrow labelled `return` goes back. A side box notes: **TSS** (Task State Segment) stores kernel stack pointer (RSP0); CPU loads it on privilege level change.*

| | User Stack | Kernel Stack |
|---|---|---|
| Ring | Ring 3 | Ring 0 |
| Size | large, e.g., 8MB | small, e.g., 8-16KB |
| Contents | local variables, function args, return addresses, … | saved user RSP, saved user RIP, saved registers, kernel locals, … |

The **TSS** (Task State Segment) stores the kernel stack pointer (`RSP0`). The CPU loads it on a privilege level change.

### Kernel Stack: Security and Design

**Why not use the user stack?**

- User controls their stack pointer
- Could point to invalid memory
- Could point to kernel memory!
- Stack overflow attacks
- Kernel must have reliable storage

**Kernel stack properties:**

- Fixed small size (8-16KB)
- One per thread
- In kernel memory (protected)
- Stores syscall/interrupt state
- Enables nested interrupts

---

<!-- source pages 22-34 -->

## System Call Examples

**Common System Calls**, grouped by area:

| Process | File System | Network | Time |
|---|---|---|---|
| `fork()` | `open()` | `socket()` | `time()` |
| `exec()` | `close()` | `bind()` | `settime()` |
| `exit()` | `read()` | `listen()` | |
| `getpid()` | `write()` | | |
| `wait()` | `seek()` | | |
| | `mkdir()` | | |

---

## Exercise 1: System Call Overhead

### Problem

A program processes **1,200** data blocks in a tight loop. For each block:

- **User-mode computation:** 800 CPU cycles
- **System call (`write()`):**
  - User $\rightarrow$ kernel trap: 400 cycles
  - Kernel execution: 2,000 cycles
  - Kernel $\rightarrow$ user return: 300 cycles

### Questions — Calculate

1. Total CPU cycles in pure user-mode computation
2. Number of traps and total trap overhead
3. Total cycles executing `write()` in kernel
4. Grand total cycles and overhead percentage
5. Redo with batching (10 blocks per syscall)

### Solution 1: Total User-Mode Compute

Given — Per block: 800 cycles | Blocks: 1,200

$$\text{Total user-mode compute} = 800 \times 1{,}200 = \mathbf{960{,}000}\ \text{cycles}$$

### Solution 2: Mode-Switch Overhead

Given — Per syscall: User$\rightarrow$kernel = 400 cycles, Kernel$\rightarrow$user = 300 cycles

- Number of system calls: 1,200
- Overhead per syscall: $400 + 300 = 700$ cycles
- **Total trap overhead:**

$$700 \times 1{,}200 = \mathbf{840{,}000}\ \text{cycles}$$

### Solution 3: Kernel-Mode Work

Given — Kernel execution per `write()`: 2,000 cycles

$$\text{Total kernel-mode work} = 2{,}000 \times 1{,}200 = \mathbf{2{,}400{,}000}\ \text{cycles}$$

### Solution 4: Grand Total and Overhead

- User compute: 960,000 cycles
- Trap overhead: 840,000 cycles
- Kernel work: 2,400,000 cycles

$$\textbf{Grand total} = 960{,}000 + 840{,}000 + 2{,}400{,}000 = \mathbf{4{,}200{,}000}\ \text{cycles}$$

$$\textbf{Overhead fraction} = \frac{840{,}000}{4{,}200{,}000} \times 100\% = \mathbf{20\%}$$

### Solution 5: Batching Optimization

Batching 10 blocks per syscall:

$$\text{New syscall count} = \frac{1{,}200}{10} = 120$$
$$\text{Trap overhead} = 700 \times 120 = 84{,}000\ \text{cycles}$$
$$\text{Kernel work} = 2{,}000 \times 120 = 240{,}000\ \text{cycles}$$
$$\text{User compute} = 960{,}000\ \text{cycles (unchanged)}$$
$$\textbf{Grand total} = 960{,}000 + 84{,}000 + 240{,}000 = \mathbf{1{,}284{,}000}$$
$$\textbf{Overhead} = \frac{84{,}000}{1{,}284{,}000} \times 100\% \approx \mathbf{6.54\%}$$

### Key Takeaways

**Key Insight:** Batching reduced overhead from **20%** to **6.54%** and total cycles from **4.2M** to **1.28M** – a 3.3x improvement!

Real-World Implications:

- Minimize syscall frequency when possible
- Buffer/Batch I/O operations

---

## Interrupts and Exceptions

### Connecting to Traps

**Remember Traps?** Earlier we learned that **traps** are synchronous events caused by the executing instruction. Now let's see how interrupts and exceptions fit into this picture.

*Figure: A taxonomy tree rooted at "CPU Events (all go through IDT)". It splits into two branches — "Hardware Interrupts" (asynchronous, external to CPU; e.g. keyboard, disk, timer) and "Traps (Synchronous)" (caused by the executing instruction). The Traps branch splits further into "Software Interrupts" (int 0x80, syscalls) and "Exceptions" (page fault, div by 0).*

### Hardware Interrupts vs Software Interrupts

| Hardware Interrupts | Software Interrupts (Traps) |
|---|---|
| Triggered by external I/O devices | Triggered by `INT` instruction |
| **Asynchronous** – can occur at any time, between any instructions | **Synchronous** – caused by executing code |
| CPU was not expecting it | CPU executes `int 0x80` intentionally |
| Examples: keyboard, disk, network, timer | Used for legacy system calls |

### Exceptions: Types and Examples

**What are Exceptions?** Exceptions are **synchronous traps** triggered when the CPU encounters problematic conditions during instruction execution.

**Kernel-Recoverable:**

- **Page Fault** – kernel loads page from disk, process continues
- **Breakpoint (int 3)** – debugger handles it

**Signal to Process:**

- Invalid memory access $\rightarrow$ `SIGSEGV`
- Division by zero $\rightarrow$ `SIGFPE`
- Process dies unless it catches signal

---

<!-- source pages 35-49 -->

## Signals

### What are Signals?

**Signals** are a Unix mechanism for notifying a **process or thread** that an event has occurred. They can be sent by the kernel, by another process, or by the process/thread itself.

**From the kernel:**

- CPU exception (SIGSEGV, SIGFPE)
- Child terminated (SIGCHLD)
- Timer expired (SIGALRM)

**From another process:**

- `kill(pid, sig)`
- Ctrl+C (SIGINT)
- Custom (SIGUSR1/2)

**To specific thread:**

- `pthread_kill(tid, sig)`
- Faults (SIGSEGV goes to faulting thread)

### Signals: Connecting Exceptions to User Mode

**Key insight.** We just saw that CPU exceptions like invalid memory access result in SIGSEGV. Signals are how the kernel **communicates these events to user-mode** so the process/thread can respond.

*Figure: A left-to-right pipeline — "CPU Exception" (e.g., bad address) --trap--> "Kernel" --generate--> "Signal" (SIGSEGV) --deliver--> "User Handler" (your code); it shows the full path from a hardware fault to user-level code.*

### Signal Handler Table

In Linux, each process has a `task_struct` (its PCB), which includes a **signal handler table** mapping each signal number to a handler.

| Signal | Handler |
|---|---|
| SIGINT (2) | `my_handler` (custom) |
| SIGTERM (15) | SIG_DFL $\rightarrow$ terminate |
| SIGCHLD (17) | SIG_DFL $\rightarrow$ ignore |
| SIGKILL (9) | Always terminate (forced) |

Legend:

- **Custom** = your function
- **SIG_DFL** = kernel default
- **Forced** = cannot override

#### Signal Handler Table Caveats

**Key insight.** **SIG_DFL** means "use the default action" — each signal has a predefined behavior (terminate, ignore, stop, or core dump) that applies when no custom handler is set.

**Warning.** `SIGKILL` and `SIGSTOP` **cannot be caught or ignored** — ensures the system can always terminate/stop a process.

### Two Levels of Signal State

**Shared vs per-thread.** In multi-threaded programs, some signal state is **shared** (process-level) and some is **per-thread**.

*Figure: A box labelled "Process" containing a single "Handler Table (shared)" at the top, and below it three side-by-side boxes "Thread 1", "Thread 2", "Thread 3", each annotated "mask, pending" — one handler table for the whole process, but a private mask and pending set per thread.*

#### Process-Level vs Thread-Level

**Process-Level (Shared)**

- **Handler table** — all threads share the same signal handlers
- **Process pending queue** — signals sent to PID
- Set via `sigaction()`

**Thread-Level (Per-Thread)**

- **Signal mask** — each thread blocks different signals
- **Thread pending queue** — signals sent to TID
- Set via `pthread_sigmask()`

### Common Signals

| Signal | Default Action | Typical Cause |
|---|---|---|
| SIGINT (2) | Terminate | Ctrl+C from terminal |
| SIGKILL (9) | Terminate | `kill -9` (cannot catch!) |
| SIGSEGV (11) | Terminate + core | Invalid memory access |
| SIGTERM (15) | Terminate | Polite termination request |
| SIGCHLD (17) | Ignore | Child process stopped/terminated |
| SIGSTOP (19) | Stop | Pause process (cannot catch!) |
| SIGCONT (18) | Continue | Resume stopped process |

### Registering Signal Handlers

Two ways to register handlers:

- `signal(sig, handler)` — simple, but **unpredictable behavior**
- `sigaction(sig, &act, &oldact)` — preferred, consistent and configurable

**Warning — the problem with `signal()`:** On some systems (System V), after the handler runs, the signal disposition is **reset to SIG_DFL**. On other systems (BSD), the handler **remains installed**. This inconsistency makes `signal()` unreliable for portable code.

#### signal() vs sigaction() — Comparison

| | `signal()` | `sigaction()` |
|---|---|---|
| **Handler persistence** | May reset to SIG_DFL after handling | Stays installed |
| **Blocking during handler** | System-dependent | via `sa_mask` |
| **Syscall restart** | System-dependent | `SA_RESTART` flag |
| **Portability** | Behavior varies | Consistent |

#### Sigaction Flags

Useful `sigaction` flags:

- `SA_RESTART` — automatically restart interrupted syscalls (read, write, etc.)
- `SA_RESETHAND` — reset handler to SIG_DFL after one invocation
- `SA_NODEFER` — don't block this signal while handler runs

### Signals During Handler Execution

**What if a signal arrives while in a handler?** By default, when a handler runs, the signal that triggered it is **blocked**. But other signals can still interrupt the handler!

*Figure: A timeline running left to right — `main()` runs, then a SIGINT arrow drops in and control moves to `handler()`, then a SIGTERM arrow drops in and control moves to `handler2()`, then control returns to `main()`; it shows a second signal interrupting a running handler.*

**Warning.** This can cause **reentrancy problems** — the second handler interrupts the first, potentially corrupting shared state!

### What is Async-Signal-Safe?

**Definition.** A function is **async-signal-safe** if it can be safely called from within a signal handler, even if the signal interrupted that same function (or another non-safe function) in the main program.

A function is async-signal-safe if it:

- Uses no static/global data, OR
- Accesses shared data only atomically, OR
- Can be interrupted and re-entered without corruption

#### Dangers of not being Async-Signal-Safe

**Warning.** Most library functions are **NOT** async-signal-safe because they use internal locks, global buffers, or static variables. Calling them from a handler can cause **deadlocks** or **data corruption**.

#### Why printf() is NOT Async-Signal-Safe

The reentrancy problem — imagine this scenario:

1. `main()` calls `printf("Hello")` — acquires internal lock, starts filling buffer
2. Signal arrives, handler runs
3. Handler calls `printf("Signal!")` — tries to acquire same lock... **DEADLOCK!**

Or worse: buffer is in inconsistent state $\rightarrow$ **corruption**

---

<!-- source pages 50-64 -->

## Blocking Signals During Handler

**Using `sa_mask`:** `sa_mask` specifies signals to **add to the thread's signal mask** while the handler runs. These signals are blocked for the thread (not just the handler). The original mask is restored when the handler returns.

Block all signals during handler:

```c
struct sigaction sa;
sa.sa_handler = handler;
sigfillset(&sa.sa_mask); // block ALL
sa.sa_flags = 0;
sigaction(SIGINT, &sa, NULL);
```

**Key insight:** By default, only the triggering signal is added to the mask. Use `sa_mask` to block additional signals.

### Original Mask vs `sa_mask`

| | **Original Mask** | **`sa_mask`** |
|---|---|---|
| What it is | The thread's **existing signal mask** at the moment the kernel is about to run the handler. | A **static per-handler setting** chosen when you install the handler. |
| Details | Set via `sigprocmask()` or `pthread_sigmask()`. The **baseline** — kernel saves it and restores it on handler return. | Meaning: "while this handler runs, **also** block these signals". **Temporary** — only for duration of handler. |

### How They Combine

When signal `S` is delivered:

$$\texttt{handler\_mask} = \texttt{original\_mask} \cup \texttt{sa\_mask} \cup \{S\}$$

On normal handler return:

$$\texttt{current\_mask} := \texttt{original\_mask}$$

### `SA_NODEFER`

By default, the delivered signal `S` is **automatically blocked** during its own handler (prevents re-entry).

- **With `SA_NODEFER`**: Signal `S` is **not** added to the mask — handler can be interrupted by the same signal (re-entrant)
- Formula becomes: $\texttt{handler\_mask} = \texttt{original\_mask} \cup \texttt{sa\_mask}$

### Signal Mask Example

Setup:

- Thread's original mask blocks: `{SIGINT}`
- Handler installed with `sa_mask` blocking: `{SIGTERM}`
- Signal delivered: `SIGUSR1` (no `SA_NODEFER`)

| Phase | Blocked signals |
|---|---|
| While handler runs | `{SIGINT, SIGTERM, SIGUSR1}` (original $\cup$ `sa_mask` $\cup$ delivered) |
| After handler returns | `{SIGINT}` (back to original mask) |

---

## Signal Delivery Flow

Signal delivery happens in stages: **generation** $\rightarrow$ **pending** $\rightarrow$ **delivery** $\rightarrow$ **handling**.

*Figure: A four-box left-to-right pipeline — "Signal Generated" (kill(), kernel event, raise()) → "Pending Queue" (stored until deliverable) → "Kernel Checks (return to user)" (syscall/interrupt return, wakeup) → "Handler Invoked" (user handler or default).*

### Signals Can Interrupt Blocking Syscalls

A signal can wake a process blocked in a syscall (e.g., `read()`):

- Syscall returns `-1` with `errno = EINTR`
- **`SA_RESTART` flag**: kernel automatically restarts the syscall after handler returns

---

## Process-Directed vs Thread-Directed Signals

| | **Process-Directed** | **Thread-Directed** |
|---|---|---|
| Sent to | The **process as a whole** | A **specific thread** |
| Examples | `kill(pid, sig)`; `SIGTERM`, `SIGINT`; external events | `pthread_kill(tid, sig)`; `SIGSEGV`, `SIGFPE`; faults caused by that thread |
| Delivery | Any thread that doesn't have the signal blocked can receive it. | Only that specific thread can receive it. |

### Synchronous Signals are Thread-Directed

**Warning:** Synchronous signals (`SIGSEGV`, `SIGFPE`, `SIGBUS`) are **always thread-directed** — they're caused by the executing thread's own actions.

*Figure: A dashed "Process" box containing Thread 1, Thread 2 and Thread 3. A box `*NULL = 42` below points with a red arrow up to Thread 2, labelled SIGSEGV; Threads 1 and 3 are marked with ×. Caption: only Thread 2 (the faulting thread) receives SIGSEGV.*

### Multi-Threaded Signal Delivery — Which Thread Gets the Signal?

For **process-directed** signals, the kernel picks one thread that:

1. Does **not** have the signal blocked (via `pthread_sigmask`)
2. Is available to handle it

The choice is **arbitrary** — don't rely on which thread receives it!

*Figure: A `SIGUSR1` box outside a dashed "Process" box, with a "delivers to" arrow pointing past Thread 1 (marked blocked, struck through) to Thread 2 (unblocked); Thread 3 is also unblocked.*

---

## Signal Masks and Pending Signals

**Signal Mask (per thread):** Each thread has its own **signal mask** — a set of blocked signals.

- Set via `pthread_sigmask()`
- Inherited from parent on `fork()`
- Blocked signals stay pending

**Pending Signals:** Two levels of pending signals:

- **Process-level:** shared pending queue
- **Thread-level:** per-thread pending queue

Standard signals don't queue — only one pending per signal number.

### Standard Signals: Coalescing Behavior

**Bitmask, not a queue.** Standard signals (1–31) are stored as a **bitmask** in the kernel. Each signal number is a single bit: either pending (1) or not (0). Multiple instances of the same signal **collapse into one**.

*Figure: Timeline with the receiver having SIGUSR1 blocked. Sender emits SIGUSR1 three times; the first sets "bit 10 = 1" in the pending row, the second and third are struck out as "(already set)". After `unblock`, `handler()` runs — called ONCE.*

### Real-Time Signals: Queued Delivery

**`SIGRTMIN` to `SIGRTMAX`.** Real-time signals (typically 34–64) provide **reliable, queued** signal delivery — multiple instances are preserved, not coalesced.

*Figure: Timeline with the receiver having SIGRTMIN blocked. Sender emits SIGRTMIN three times; the queue grows `[1]` → `[1, 2]` → `[1, 2, 3]`. After `unblock`, `h()` runs three times — called 3 times!*

### Real-Time Signals: Features

**Key Properties:**

- **Range**: `SIGRTMIN` to `SIGRTMAX` (typically 34–64)
- **Delivery order**:
  - **Same signal**: FIFO (first sent = first delivered)
  - **Different signals**: Priority (lower number first)
- **Data**: Can carry payload via `sigqueue()`

**Use Cases:**

- Event counting
- IPC with data
- Timer notifications (`timer_create`)
- AIO completion

---

<!-- source pages 65-79 -->

## Sending Signals

**From the command line:**

```bash
kill -SIGTERM <pid>
kill -9 <pid>    (SIGKILL)
kill -STOP <pid>
killall -SIGTERM name
```

Use `ps aux` to find PIDs.

**From code:**

```c
kill(pid, SIGTERM);
raise(SIGALRM);    (to self)
pthread_kill(tid, sig);
```

Returns 0 on success, -1 on error.

> **Warning — Don't confuse with pthreads!** `pthread_cond_signal()` is **not** a Unix signal – it uses futexes. Same word, different mechanism!

---

## Signal Handler Example (using sigaction)

Proper signal handler setup:

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

void handle_sigint(int sig) {
    const char *msg = "\nCaught SIGINT. Exiting...\n";
    write(STDOUT_FILENO, msg, strlen(msg));
    _exit(0);  // async-signal-safe
}
```

> **Warning:** Use `write()` not `printf()` in handlers – printf is not async-signal-safe!

Main function with `sigaction()`:

```c
int main() {
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);  // don't block other signals
    sa.sa_flags = SA_RESTART;  // restart interrupted syscalls

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }
    printf("Running. Press Ctrl+C to exit.\n");
    while (1) { sleep(1); }
    return 0;
}
```

---

## Signal Masking as Synchronization

**Protecting critical sections.** Signal masking is a **synchronization mechanism** – like mutexes and semaphores, it prevents race conditions and deadlocks by controlling when handlers can interrupt your code.

### Deadlock scenario (without masking)

1. `main()` acquires `mutex_lock(&data_lock)`
2. Signal arrives, handler runs
3. Handler calls `mutex_lock(&data_lock)` → **DEADLOCK!** (In general behavior may be undefined, and not async-signal-safe)

Same thread holds the lock and waits for it – classic self-deadlock.

### Solution: mask during critical section

1. `sigprocmask(SIG_BLOCK, &mask, &old)` – block signals
2. `mutex_lock(&data_lock)` – safe, no handler can interrupt
3. Do critical work...
4. `mutex_unlock(&data_lock)`
5. `sigprocmask(SIG_SETMASK, &old, NULL)` – restore, pending signals now delivered

### Masking and unmasking in code

```c
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    sigset_t mask;
    sigemptyset(&mask);        // Initialize empty set
    sigaddset(&mask, SIGUSR1); // Add SIGUSR1 to mask

    // Block SIGUSR1
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        return 1;
    }
    // Critical section: SIGUSR1 is masked here
    printf("Entering critical section\n");
    do_critical_stuff();

    // Unblock SIGUSR1
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    // Pending SIGUSR1 delivered now
    printf("SIGUSR1 delivered after unmasking\n");
    return 0;
}
```

### Signal mask/unmask sequence

*Figure: A sequence diagram with two lifelines, "Process" and "Kernel". The process calls `sigprocmask(BLOCK)`, after which SIGUSR1 is masked; the kernel then sends SIGUSR1 to the process, which is added to the pending queue; the process calls `sigprocmask(UNBLOCK)` and only then does the handler execute.*

---

## Warning: Async-Signal-Safety

> **Warning:** The following exercises use `pthread_mutex_*` and `pthread_rwlock_*` in signal handlers. These functions are **NOT async-signal-safe**!

**For teaching purposes:** The following exercises assume signals arrive **between** function calls, not during. This lets us focus on signals/lock-related issues without undefined behavior.

---

## Exercise 2: Logging with Signals

Shared state and actors:

```c
char log_buf[4] = {0};   // Shared log buffer
int idx = 0;             // Next write position
pthread_rwlock_t lock;   // Read-write lock

void handler(int sig) {                 // SIGUSR1 Handler
    pthread_rwlock_wrlock(&lock);
    log_buf[idx++] = '!';
    pthread_rwlock_unlock(&lock);
}

void* writer(void* arg) {               // Thread A
    pthread_rwlock_wrlock(&lock);
    log_buf[idx++] = 'A';
    pthread_rwlock_unlock(&lock);
}

void* reader(void* arg) {               // Thread B
    pthread_rwlock_rdlock(&lock);
    printf("%s\n", log_buf);
    pthread_rwlock_unlock(&lock);
}
```

### The dangerous interleaving

Assume SIGUSR1 is sent to **Thread A** while it holds the write lock...

*Figure: A timeline for Thread A — `wrlock()` then `write 'A'` (annotated "holds lock"), then SIGUSR1 arrives (red arrow from above) and `handler: wrlock()` (annotated "waits for lock"), ending in a padlock labelled DEADLOCK.*

> **Warning:** Handler runs in **same thread** context! Thread A waits for a lock it already holds → **self-deadlock**.

### Your task

Given the code on the previous slides, and assuming **no signal masking**:

1. List all possible interleavings of Thread A, Thread B, and the signal handler
2. Mark which interleavings result in **deadlock**
3. For safe interleavings, write the **output** of Thread B's printf

**Assumptions:**

- SIGUSR1 is sent **once** during execution
- Signal can be delivered to **either** Thread A or Thread B
- Each thread runs its function exactly once

### Solution: interleaving table

| # | Order | Signal To | Result |
|---|---|---|---|
| 1 | A(lock) → SIG → handler(lock) | A (holding lock) | Deadlock |
| 2 | B(rdlock) → SIG → handler(wrlock) | B (holding rdlock) | Deadlock |
| 3 | A completes → SIG → handler → B | A (after unlock) | prints "A!" |
| 4 | A completes → B → SIG → handler | B (after unlock) | prints "A", then "!" added |
| 5 | B → A → SIG → handler | A (after unlock) | prints "", then "A!" added |
| 6 | SIG → handler → A → B | before any lock | prints "!A" |

- **Deadlock cases:** signal delivered while **any thread** holds the lock → handler blocks forever
- **Safe cases:** signal delivered when **no lock held** → handler completes normally

### Solution: the fix (signal masking)

Mask signals during the critical section:

```c
void* writer(void* arg) {
    sigset_t mask, old;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    sigprocmask(SIG_BLOCK, &mask, &old);   // Block signal
    pthread_rwlock_wrlock(&lock);
    log_buf[idx++] = 'A';
    pthread_rwlock_unlock(&lock);
    sigprocmask(SIG_SETMASK, &old, NULL);  // Unblock, pending delivered
    return NULL;
}

void* reader(void* arg) {
    sigset_t mask, old;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    sigprocmask(SIG_BLOCK, &mask, &old);   // Block signal
    pthread_rwlock_rdlock(&lock);
    printf("%s\n", log_buf);
    pthread_rwlock_unlock(&lock);
    sigprocmask(SIG_SETMASK, &old, NULL);  // Unblock
    return NULL;
}
```

### Solution: safe outputs only

**With signal masking – no deadlocks possible.** Signal delivery is **deferred** until the lock is released. Handler always runs when the lock is free.

Possible outputs (all safe):

| Order | Thread B prints |
|---|---|
| handler → A → B | "!A" |
| A → handler → B | "A!" |
| A → B → handler | "A" |
| B → handler → A | "" |
| B → A → handler | "" |
| handler → B → A | "!" |

---

<!-- source pages 80-94 -->

## Exercise 3: Sensor Monitor with Alerts

### Shared State

```c
int current_reading = 0;        // Latest sensor value
int thresholds[2] = {10, 90};   // {low, high}
int alert_status = 0;           // 0=normal, 1=alert
pthread_mutex_t sensor_lock;    // Protects current_reading
pthread_mutex_t config_lock;    // Protects thresholds
```

*Figure: Inside the box labelled "Process" sits the shared state (`current_reading`, `thresholds[]`, `alert_status`). An external "Sensor" box feeds Thread A (Sensor) via a `read` arrow; Thread A `write`s the shared state, and Thread B (Monitor) `read`s it. Two signal arrows enter from outside the process: SIGUSR1 (red) points at the Thread B (Monitor) box, while SIGHUP (orange) stops inside the dashed process boundary at the level of the shared state, arriving at the process as a whole rather than at a specific thread — showing that signals arrive from outside while the two threads are contending for the same shared state.*

### The two threads

**Thread A: Sensor Reader**

```c
void* sensor_thread(void* arg) {
    while (1) {
        int val = read_sensor();
        pthread_mutex_lock(&sensor_lock);
        current_reading = val;
        pthread_mutex_unlock(&sensor_lock);
    }
}
```

**Thread B: Monitor**

```c
void* monitor_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&config_lock);
        int lo = thresholds[0];
        int hi = thresholds[1];
        pthread_mutex_unlock(&config_lock);
        // check and update alert_status
    }
}
```

### Signal Handlers

**SIGHUP: Reconfigure Thresholds**

```c
void sighup_handler(int sig) {
    // Update thresholds from config
    pthread_mutex_lock(&config_lock);
    thresholds[0] = new_low;    // Step 1
    // <-- SIGUSR1 could arrive here!
    thresholds[1] = new_high;   // Step 2
    pthread_mutex_unlock(&config_lock);
}
```

**SIGUSR1: Force Alert Check**

```c
void sigusr1_handler(int sig) {
    pthread_mutex_lock(&config_lock);
    int lo = thresholds[0];
    int hi = thresholds[1];
    pthread_mutex_unlock(&config_lock);

    pthread_mutex_lock(&sensor_lock);
    int val = current_reading;
    pthread_mutex_unlock(&sensor_lock);

    alert_status = (val < lo || val > hi);
}
```

#### The problem

> **Warning.** Both handlers try to acquire locks. Signals can be delivered to **either thread**. What could go wrong?

---

## Danger 1: Nested Handler (SIGUSR1 during SIGHUP)

> **Warning — SIGUSR1 During SIGHUP Handler.** If SIGUSR1 arrives while the SIGHUP handler is between updating `thresholds[0]` and `thresholds[1]`...

*Figure: A timeline for SIGHUP. Step 1 box "lock config"; step 2 box "thresh[0]=5" (annotated `new_low=5`); then a red SIGUSR1 arrow strikes down onto step 3, a box "lock config" marked DEADLOCK!, ending at a padlock icon. A callout below the timeline reads `thresholds = {5, 90}` (half old, half new!).*

Step-by-step:

1. SIGHUP handler locks `config_lock`.
2. It sets `thresholds[0] = new_low` (= 5), so the array is now `{5, 90}` — half old, half new.
3. SIGUSR1 arrives at this exact point and its handler tries `lock config` → **DEADLOCK**, before `thresholds[1]` is ever written.

**Problem 1: Deadlock** — the SIGUSR1 handler tries to acquire `config_lock` already held by the SIGHUP handler (same thread).

**Problem 2: Inconsistent State** — even without deadlock, SIGUSR1 might see `{new_low, old_high}` → wrong alert!

---

## Danger 2: Signal to a Lock-Holding Thread

*Figure: A SIGUSR1 arrow points down into the "Process" box, landing on Thread A ("holds sensor_lock, updating reading"). Below Thread A a red box reads "Handler needs sensor_lock — DEADLOCK". To the right, Thread B ("holds config_lock, checking thresholds") is annotated "If signal went here: needs config_lock — DEADLOCK". Either target deadlocks.*

> **Warning.** The SIGUSR1 handler needs **both locks**. If delivered to a thread holding **either** lock → deadlock!

---

## Exercise: Interleaving Analysis

**Your task:** complete the table — for each scenario, determine the **outcome**.

| # | Signal | Delivered To | State | Outcome? |
|---|--------|--------------|-------|----------|
| 1 | SIGHUP | Thread A | holds sensor_lock | |
| 2 | SIGHUP | Thread B | holds config_lock | |
| 3 | SIGUSR1 | Thread A | holds sensor_lock | |
| 4 | SIGUSR1 | Thread B | holds config_lock | |
| 5 | SIGUSR1 | Thread A | in SIGHUP handler | |
| 6 | SIGUSR1 | Thread B | no lock held | |

### Solution

| # | Signal | To | State | Outcome |
|---|--------|----|-------|---------|
| 1 | SIGHUP | A | holds sensor_lock | Safe (SIGHUP needs `config_lock` only) |
| 2 | SIGHUP | B | holds config_lock | Deadlock (SIGHUP needs `config_lock`) |
| 3 | SIGUSR1 | A | holds sensor_lock | Deadlock (SIGUSR1 needs `sensor_lock`) |
| 4 | SIGUSR1 | B | holds config_lock | Deadlock (SIGUSR1 needs `config_lock`) |
| 5 | SIGUSR1 | A | in SIGHUP handler | Deadlock (`config_lock` held) |
| 6 | SIGUSR1 | B | no lock held | Safe |

---

## Exercise 3: The Fix

### Fix 1: Block SIGUSR1 During SIGHUP

```c
struct sigaction sa_hup;
sa_hup.sa_handler = sighup_handler;
sigemptyset(&sa_hup.sa_mask);
sigaddset(&sa_hup.sa_mask, SIGUSR1);  // Block SIGUSR1 during SIGHUP handler
sigaction(SIGHUP, &sa_hup, NULL);
```

### Fix 2: Mask Signals While Holding Locks

```c
void* sensor_thread(void* arg) {
    sigset_t mask, old;
    sigemptyset(&mask);
    sigaddset(&mask, SIGHUP);
    sigaddset(&mask, SIGUSR1);

    while (1) {
        int val = read_sensor();
        sigprocmask(SIG_BLOCK, &mask, &old);   // Block both signals
        pthread_mutex_lock(&sensor_lock);
        current_reading = val;
        pthread_mutex_unlock(&sensor_lock);
        sigprocmask(SIG_SETMASK, &old, NULL);  // Unblock
    }
}
```

---

## User Mode Threading

### setjmp and longjmp

**Non-Local Jumps**

- `setjmp`: Saves thread state in `jmp_buf`, returns 0
- `longjmp`: Restores thread state, returns non-zero
- Used for non-linear code execution and exception handling

**Key Insight.** Think of `setjmp`/`longjmp` as a "checkpoint and restore" mechanism — save your position, and jump back to it later from anywhere in the call stack!

### Error Recovery with setjmp/longjmp

**Division with Error Recovery**

```c
#include <stdio.h>
#include <setjmp.h>

jmp_buf jump_buffer;

void divide(int numerator, int denominator) {
    if (denominator == 0) {
        printf("Error: Division by zero!\n");
        longjmp(jump_buffer, 1);  // Jump back
    }
    printf("Result: %d\n", numerator / denominator);
}
```

**Main with Checkpoint**

```c
int main() {
    int num = 10, denom = 0;
    if (setjmp(jump_buffer) == 0) {
        // First time: setjmp returns 0
        printf("Attempting division...\n");
        divide(num, denom);
        printf("Division succeeded!\n");
    } else {
        // After longjmp: execution resumes here
        printf("Recovered from error.\n");
    }
    printf("Program continues.\n");
    return 0;
}
```

---

<!-- source pages 95-107 -->

## Gotcha: `sigsetjmp` and `siglongjmp`

**Warning:** If you use `longjmp()` to exit a signal handler, the signal mask **might not be restored** — behavior is platform-dependent!

**POSIX Solution**

- `sigsetjmp(env, savemask)` — if `savemask` $\neq 0$, saves current signal mask
- `siglongjmp(env, val)` — restores the mask saved by `sigsetjmp`

*Note: a further callout, titled approximately "Safe Jump from Handler", overflows past the bottom edge of this slide in the original; only the tops of the title letters are visible and its body is unreadable.*

---

## User-Mode Threading

**Threading Without Kernel:** Use `setjmp`/`longjmp` to manage threads entirely in user space.

| Pros | Cons |
| --- | --- |
| No system call overhead | OS unaware of threads |
| Faster context switching | No multi-core support |
| Smaller state to save | Complex to manage |

---

## Exercise 4: Database Transaction Rollback

### The SQL Analogy

In SQL, a transaction can be aborted with `ROLLBACK`:

```sql
BEGIN; UPDATE accounts SET balance=balance-100 WHERE id='A';
                -- error occurs here → ROLLBACK
UPDATE accounts SET balance=balance+100 WHERE id='B';
COMMIT;
```

*Figure: a timeline of the transaction — `BEGIN` → `Debit A` → `ROLLBACK`, with a red `SIGUSR1` arrow striking the `ROLLBACK` box, and a dashed, greyed-out `Credit B` box that is never reached. Account-state boxes below the timeline show `A: $1000, B: $100` at `BEGIN` and `A: $900, B: $100` after `Debit A`.*

### Shared State

```c
int account[2] = {1000, 100};        // Account A=1000, B=100
int total_deposited = 1100;          // Bank's record of total money in system
pthread_mutex_t db_lock;             // Protects all account data
sigjmp_buf checkpoint;               // Rollback point
volatile sig_atomic_t in_tx = 0;     // In transaction flag
```

*Figure: a red `SIGUSR1 = ROLLBACK` arrow points down into a "Database" box holding `Account A: $1000`, `Account B: $100` and `db_lock`; Thread A (Transaction) on the left and Thread B (Auditor) on the right both have arrows into the database.*

### The Threads

Thread A: Transaction Worker

```c
void* transaction(void* arg) {
    if (sigsetjmp(checkpoint, 1) != 0) {
        // ROLLED BACK - jumped here!
        printf("Transaction aborted\n");
        in_tx = 0;
        return NULL;
    }

    in_tx = 1;
    pthread_mutex_lock(&db_lock);   // BEGIN
    account[0] -= 100;              // Debit A
    // <-- SIGUSR1 danger zone!
    account[1] += 100;              // Credit B
    pthread_mutex_unlock(&db_lock); // COMMIT
    in_tx = 0;
    printf("Transaction committed\n");
}
```

Thread B: Auditor

```c
void* auditor(void* arg) {
    while (1) {
        pthread_mutex_lock(&db_lock);
        int sum = account[0] + account[1];
        int expected = total_deposited;
        pthread_mutex_unlock(&db_lock);

        if (sum != expected) {
            printf("INCONSISTENT!\n");
        }
        sleep(1);
    }
}
```

**Key Insight:** Bank's internal transfers: money moves between accounts but `total_deposited` stays constant.

### The Dangerous Handler

```c
void rollback_handler(int sig) {
    if (in_tx) {
        siglongjmp(checkpoint, 1);  // DANGER: lock still held!
    }
}
```

- **Problem 1: Lock Abandoned** — `siglongjmp` jumps out while `db_lock` is held $\rightarrow$ lock **never released** $\rightarrow$ Auditor thread **deadlocks forever**
- **Problem 2: Inconsistent State** — If signal after debit but before credit: A=\$900, B=\$100 $\rightarrow$ sum $\neq$ `total_deposited`!

### Danger Timeline

*Figure: two-thread timeline. Thread A: `lock()` → `A -= 100` → (red `SIGUSR1` arrow) → `handler` → a red dashed `longjmp!` curve arcing backwards to the checkpoint. State labels under the axis: `lock: held, A=900, B=100` early, then `lock: still held! (orphaned)` at the handler. Thread B: `lock()` after that point, then `blocked...` with a padlock icon.*

**Result**

- Thread A: continues from checkpoint, thinks rollback succeeded
- Thread B: blocked forever waiting for lock that will never be released
- Database: inconsistent state (A=900, B=100, sum=1000)

### Your Task: Interleaving Analysis

For each scenario, determine what happens when `SIGUSR1` arrives:

| # | Signal Arrives | Lock State | Outcome? |
| --- | --- | --- | --- |
| 1 | Before `sigsetjmp` | not held | |
| 2 | After `sigsetjmp`, before `lock()` | not held | |
| 3 | After `lock()`, before debit | held | |
| 4 | After debit, before credit | held | |
| 5 | After credit, before `unlock()` | held | |
| 6 | After `unlock()` (committed) | not held | |

### Solution — Analysis

| # | Signal Arrives | Lock | Outcome |
| --- | --- | --- | --- |
| 1 | Before `sigsetjmp` | none | Handler jumps to unset checkpoint = **undefined behavior**! |
| 2 | After `sigsetjmp`, before `lock()` | none | Safe rollback, no cleanup needed |
| 3 | After `lock()`, before debit | held | Deadlock (lock orphaned), data OK |
| 4 | After debit, before credit | held | Deadlock + Inconsistent (A=900, B=100) |
| 5 | After credit, before `unlock()` | held | Deadlock (lock orphaned), data consistent |
| 6 | After `unlock()` | none | Too late, transaction already committed |

### Solution — The Fix, Part 1: Fixed Handler (Cleanup Before Jump)

```c
volatile sig_atomic_t saved_state = 0;  // Track saved state
int saved_A, saved_B;                   // For actual rollback

void rollback_handler(int sig) {
    if (in_tx) {
        // Restore original values (actual rollback!)
        if (saved_state) {
            account[0] = saved_A;
            account[1] = saved_B;
            saved_state = 0;
        }
        pthread_mutex_unlock(&db_lock);  // Release lock!
        siglongjmp(checkpoint, 1);
    }
}
```

### Solution — The Fix, Part 2: Fixed Transaction (Save State)

```c
void* transaction(void* arg) {
    if (sigsetjmp(checkpoint, 1) != 0) { printf("Aborted\n");
        return NULL; }

    in_tx = 1;
    pthread_mutex_lock(&db_lock);

    saved_A = account[0]; saved_B = account[1];  // Save for rollback
    saved_state = 1;

    account[0] -= 100;
    account[1] += 100;

    saved_state = 0;
    pthread_mutex_unlock(&db_lock);
    in_tx = 0;
}
```

---

## See You Next Week!

Questions?
