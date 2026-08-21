# Recitation 1 — Intro and the Modern Operating System

> Study notes converted from the recitation slide deck `Rec1_OS.pdf` (64 pages) — Operating Systems, Reichman University, Spring Semester 2026.
> Lecturer: Tsvi Cherny-Shahar · TAs: Daniel Karalnik, Liam Tal.
> Faithful conversion: every substantive item on the source pages appears here, and nothing has been added from outside them. Figures are rendered as italic *Figure:* descriptions.
> `<!-- source pages a-b -->` comments mark the original page ranges.

---

<!-- source pages 1-14 -->

## Recitation 1 — Introduction to Operating Systems

### Table of Contents

1. Intro
   - Course Logistics
2. The Modern Operating System
   - Basic Concepts
   - Threads and Processes
   - Time Accounting

---

## Intro

### The Way of the OS

*Figure: A stylized illustration of a cat sensei ("Cat-Su Sensei") in a traditional dojo lecturing a class of seated cats. The board behind him reads "THE WAY OF OS: CAT-SU SENSEI'S TEACHINGS" and shows code alongside a KERNEL block diagram with arrows to device/data boxes, captioned "OPERATING SYSTEMS: KERNEL | PROCESS | MEMORY". A hanging scroll reads 猫王流 OS 真理.*

### Contact Information

**Teaching Assistants:**

- Daniel Karalnik: daniel.karalnik@post.runi.ac.il
- Liam Tal: liam.tal@post.runi.ac.il

**Grader:**

- Maor Mizrachi: maor.mizrachi@post.runi.ac.il

**Office Hours:** By appointment only

### Homework Overview

**3 hybrid assignments** (theoretical + programming) — **40% of final grade**

- All assignments are **mandatory**
- Submit **before the deadline** — follow all submission instructions
- **Theoretical answers:** Must be fully explained; partial answers may lose marks
- **Code requirements:** Correct, readable, well-organized, and efficient
- Deviations from instructions may result in point deductions

**Extension requests:** Email both the lecturer and grader (CC both)

### Homework Appeal Policy

- Send appeals **directly to the grader**
- Non-administrative appeals trigger a **full re-check** of the assignment
  - This may result in grade increases *or decreases*

**Note:** TAs and lecturer will not pre-evaluate appeals. If you believe your case is valid, submit it directly to the grader.

---

## The Modern Operating System

### Terminology

**Key Terminology**

- **Latency** (time unit) — The time to complete a job.
- **Throughput** (amount/time) — The amount of data processed in a time unit.
- **CPU Utilization** — The amount of time the CPU is executing non-OS code.
- **Overhead** — Time spent on non-program tasks (memory access, OS computation, etc.).
- **CPU Usage** — Utilization + Overhead.

### Exercise 1: Terminology

**Problem**

A program takes 10 seconds to complete a task. During this time:

- The CPU spends 6 seconds executing program code.
- It spends 2 seconds accessing memory.
- It spends 2 seconds performing OS-related tasks.
- The system processes 500GB in these 10 seconds.

**Calculate**

1. The CPU utilization (time executing non-OS code)
2. The overhead (time not running program code)
3. The total CPU usage (utilization + overhead)
4. The throughput (amount of data per time unit)
5. The latency (time to complete a job)

**Hint**

Use the terminology definitions from the previous slide. Remember: Utilization $\neq$ Usage!

#### Exercise 1: Solution

**Given:** Total: 10s | CPU exec: 6s | Memory: 2s | OS tasks: 2s | Data processed: 500GB

1. **CPU Utilization** (time executing non-OS code):
   $$\text{CPU Utilization} = 6\,\text{s}$$
2. **Overhead:**
   $$\text{Overhead} = \text{Memory} + \text{OS tasks} = 2 + 2 = 4 \text{ seconds}$$
3. **CPU Usage** (utilization + overhead):
   $$\text{CPU Usage} = \text{Utilization} + \text{Overhead} = 6 + 4 = 10\,\text{s}$$
4. **Throughput:**
   $$\text{Throughput} = \frac{\text{Data processed}}{\text{Total time}} = \frac{500}{10} = 50\,\text{GB/s}$$
5. **Latency:** Latency is the time taken to complete the task:
   $$\text{Latency} = \text{Total time} = 10 \text{ seconds/task}$$

---

<!-- source pages 15-27 -->

## Exercise 1: Key Takeaways

**Key Insight:** **CPU Utilization** measures only program code execution, while **CPU Usage** includes all CPU activity (utilization + overhead).

Summary:

- **CPU Utilization:** 6s (time executing non-OS code)
- **Overhead:** 4s (time not running program code)
- **CPU Usage:** 10s (utilization + overhead)
- **Throughput:** 50 GB/s (amount per time unit)
- **Latency:** 10s/task (time to complete a job)

### Percentage Breakdown

In this example:

- **60%** of CPU time $\rightarrow$ program code execution
- **40%** of CPU time $\rightarrow$ overhead (memory access, OS tasks)

**Real-World Implication:** In practice, high overhead percentages can significantly impact application performance. Optimizing memory access patterns and reducing OS calls can improve CPU utilization.

---

## Exercise 1b: Idle CPU Time

**Problem.** A job runs on the system for 12 seconds. During this time:

- The CPU spends 5 seconds executing non-OS program code
- The CPU spends 2 seconds performing OS computations
- The CPU spends 1 second accessing memory
- For the remaining time, the CPU is idle (not executing the job and waiting for IO, i.e network request, user input etc)
- During the execution of the job, the system processes 360 GB of data

**Calculate:**

1. CPU Utilization (time executing non-OS code)
2. Overhead (time not running program code)
3. CPU Usage (utilization + overhead)
4. CPU Idle Time:
   - In seconds
   - As a percentage of total time
5. Throughput (amount of data processed per time unit)
6. Latency (time to complete a job)

**Hint:** CPU Idle Time = Total Time $-$ CPU Usage. The CPU is idle when waiting for I/O!

### Solution

**Given:** Total: 12s | Non-OS execution: 5s | OS computation: 2s | Memory access: 1s

1. **CPU Utilization** (time executing non-OS code):
   $$\text{CPU Utilization} = 5\,\text{s}$$
2. **Overhead** (time not running program code):
   $$\text{Overhead} = \text{OS} + \text{Memory} = 2 + 1 = 3\,\text{s}$$

**Given:** Total: 12s | Utilization: 5s | Overhead: 3s | Data: 360 GB

3. **CPU Usage** (utilization + overhead):
   $$\text{CPU Usage} = 5 + 3 = 8\,\text{s}$$
4. **CPU Idle Time:**
   $$\text{Idle} = \text{Total} - \text{Usage} = 12 - 8 = 4\,\text{s} \approx 33.3\%$$
5. **Throughput** (amount per time unit):
   $$\text{Throughput} = \frac{360\,\text{GB}}{12\,\text{s}} = 30\,\text{GB/s}$$
6. **Latency** (time to complete a job):
   $$\text{Latency} = 12\,\text{s/job}$$

### Exercise 1b: Key Takeaways

**Key Insight:** **CPU Usage does not have to be 100%!** Idle CPU time is normal and occurs when the CPU is waiting or has no work to do.

**When does CPU idle?**

- Waiting for I/O operations (disk, network)
- Waiting for user input
- Waiting for external events or signals
- No ready tasks in the scheduler queue

Summary:

- **CPU Utilization:** 5s (time executing non-OS code)
- **Overhead:** 3s (OS computation + memory access)
- **CPU Usage:** 8s (utilization + overhead)
- **CPU Idle:** 4s
- **Throughput:** 30 GB/s
- **Latency:** 12s per job

Percentage Breakdown:

- **Utilization:** $\frac{5}{12} \approx 41.7\%$ (program code)
- **Overhead:** $\frac{3}{12} = 25\%$ (OS + memory)
- **Usage:** $\frac{8}{12} \approx 66.7\%$ (active CPU)
- **Idle:** $\frac{4}{12} \approx 33.3\%$ (waiting)

**Visual Summary:** Utilization (41.7%) + Overhead (25%) + Idle (33.3%) = 100%

---

## PC Structure

**Hardware Architecture.** The **bus** connects all hardware components, allowing CPU, memory, and controllers to communicate.

*Figure: A classic PC hardware block diagram — a wide orange "Bus" bar at the bottom, with CPU, Memory, Video controller, Keyboard controller, Floppy disk controller and Hard disk controller all attached to it; each controller in turn connects upward to its device (Monitor, Keyboard, Floppy disk drive, Hard disk drive). The point: every component talks over the single shared bus.*

---

## OS Abstractions

**Core OS Abstractions:**

- **Thread** – Abstract the CPU execution of the program
- **Address Space** – Abstracts the memory
- **File** – Abstract the inner structure of storage
- **Process** – Ties all entities of a running program together

**Why abstractions?** They hide hardware complexity and provide a consistent interface for programs to interact with system resources.

### OS Abstractions - Address Space

**Virtual Memory.** Each process has its own **virtual address space** – an illusion of having the entire memory to itself.

*Figure: A horizontal "Address Space" bar running from `0x00000000` on the left to `0x80000000` on the right, containing three labelled coloured regions separated by empty gaps — Code (green, at the low end), Heap (blue, in the middle), and Stack (red, at the high end).*

- **Code:** Program instructions
- **Heap:** Dynamic allocations
- **Stack:** Function calls & locals

---

<!-- source pages 28-46 -->

## OS Abstractions - Files

**File Abstraction:** A **file** abstracts storage details — programs see a simple path, not disk sectors or blocks.

*Figure: A single document-shaped box labelled "File" containing the path `/home/os-user/topsecret/exam.pdf` — the program's whole view of the stored data is the path.*

**Benefits:** Uniform interface for different storage types (HDD, SSD, network drives), access control, and organization via directories.

## OS Abstractions - Process

*Figure: A large box labelled "Process" ties the previous abstractions together. Inside it, three arrows across the top labelled Thread 1, Thread 2, Thread 3 (each drawn as a dashed arrow pointing right, i.e. an execution flow). Below them sits the "Address Space" bar, running from `0x00000000` on the left to `0x80000000` on the right, with coloured regions Code (green, at the low end), Heap (blue, in the middle) and Stack (pink, at the high end). Below the address space is a box labelled "File Handles", whose arrow points out of the process box down to an external file `/home/os-user/topsecret/exam.pdf`. The point: a process bundles threads, an address space and file handles into one running program.*

---

## Threads

**What is a Thread?** A **thread** is the basic unit of CPU execution within a process.

- **Old single-process OS:** The thread running the OS is also used to run applications.
- **Modern multi-process OS:** For each new process, a thread is spawned (the "main thread"). The process executes as long as its main thread runs.
- Multiple processes with threads running concurrently $\Rightarrow$ multiple threads running concurrently.

### Threads - Memory Caveats

**Memory Sharing:** Each process can have multiple threads which **partially share memory** (e.g., the heap). More on this in later lectures.

| Shared | Not Shared |
| --- | --- |
| Code section | Stack (each thread has its own) |
| Heap memory | Registers |
| Global variables | Program counter |
| File handles | |

### Threads - Creation Context

**Note:** In our context, all processes and threads are created and managed by the OS.

---

## Concurrency vs. Parallelism

- **Concurrency:** Run multiple tasks in *overlapping* time periods (interleaved execution).
- **Parallelism:** Run multiple tasks *simultaneously* (true parallel execution).

*Figure: Two timelines side by side. Left, labelled "1 core": blocks alternate along a single time axis — A, then B, then A, then B (interleaved). Right, labelled "2 cores": two long blocks, A and B, stacked one above the other spanning the same stretch of the time axis (running at once).*

**Key Difference:** Concurrency is about *dealing* with multiple things at once. Parallelism is about *doing* multiple things at once.

### Threads - Parallelism

**Parallel Execution:**

- If we have enough CPU cores to serve all our processes we can run two processes in parallel.
- However in most cases there are more processes running on our PCs than we have cores.

**The Scheduling Problem:** To solve this problem, the Operating System will try to schedule each thread to run on the CPU on its own turn.

### Threads - Concurrency

**OS Scheduling:** To solve the problem of limited cores vs. number of processes/threads, the OS will schedule each thread to run on some core according to a certain policy (more on this later).

**Context Switch:** The process where the OS interrupts the execution of a thread to execute another is called **Context Switch**.

### Context Switch

*Figure: A single timeline labelled "1 CPU core" showing blocks A, B, A, B in sequence, with a small red "cs" block inserted between each pair of consecutive task blocks (three cs blocks in total) — every switch between tasks costs extra CPU time.*

**Performance Warning:** Context switching is a **"heavy" action**. Over time, it can consume a significant portion of CPU usage, especially in scenarios with many short-lived tasks.

---

## Exercise 2: Context Switching

**Problem:** A system is running three processes (P1, P2, P3) concurrently over 15 seconds:

- CPU execution: 4s (P1) + 3s (P2) + 5s (P3)
- Context switches: 1s, Memory access: 1s, OS tasks: 1s
- Total data processed: 600GB

**Calculate:**

1. The CPU utilization (time executing non-OS code)
2. The overhead (time spent on other things, e.g. memory access, OS computation)
3. The CPU usage (utilization + overhead)
4. The throughput (amount of data processed per time unit)
5. The latency (time to complete a job)

**Hint:** Context switch time is part of the overhead — it's time the CPU spends **not** running your program code.

### Exercise 2: Solution

**Given:** Total: 15s | Execution (non-OS code): P1(4s) + P2(3s) + P3(5s) = 12s

1. **CPU Utilization** (time executing non-OS code):

$$\text{CPU Utilization} = 4 + 3 + 5 = 12\,\text{s}$$

*Note: CPU Utilization sums the execution time of **all three processes** — the actual time spent running program code (not OS operations).*

**Given:** Total: 15s | Context switch: 1s | Memory access: 1s | OS computation: 1s

2. **Overhead** (time not running program code):

$$\text{Overhead} = \text{CS} + \text{Memory} + \text{OS} = 1 + 1 + 1 = 3\,\text{s}$$

3. **CPU Usage** (utilization + overhead):

$$\text{CPU Usage} = \text{Utilization} + \text{Overhead} = 12 + 3 = 15\,\text{s}$$

**Given:** Total: 15s | 3 jobs (processes) | Data processed: 600GB

4. **Throughput** (amount per time unit):

$$\text{Throughput} = \frac{\text{Data}}{\text{Time}} = \frac{600\,\text{GB}}{15\,\text{s}} = 40\,\text{GB/s}$$

5. **Latency** (time to complete a job):

$$\text{Latency} = \frac{\text{Total Time}}{\text{Jobs}} = \frac{15}{3} = 5\,\text{s/job}$$

*Note: In this context, a job is essentially a process.*

### Exercise 2: Key Takeaways

**Key Insight:** Context switching adds overhead but enables concurrency. The trade-off is essential for multi-tasking systems.

**Summary:**

- **CPU Utilization:** 12s (time executing non-OS code)
- **Overhead:** 3s (time not running program code)
- **CPU Usage:** 15s (utilization + overhead)
- **Throughput:** 40 GB/s (amount per time unit)
- **Latency:** 5s per job (time to complete a job)

---

## Time Accounting: Time Metrics

- **Wall Time** — Total elapsed time measured by a stopwatch. *"How long did it take?"*
- **User Time** — CPU time executing the program's code in **user mode** (non-OS).
- **System Time** — CPU time executing **kernel code** (OS code) on behalf of the program (scheduling, context switching).
- **Wait Time** — Time a thread is alive but **not executing** (preempted, blocked, sleeping).

### Time Accounting: Formulas

**Key Formulas:**

- **CPU Time** = User Time + System Time
- **Wait Time (per thread)** = Wall Time $-$ Execution Time
- **Total Wait Time** = $\sum$ (Wait Time per thread)

| Single Core | Multi-Core |
| --- | --- |
| User + System $\leq$ Wall Time | User + System **may exceed** Wall Time |
| *CPU does one thing at a time* | *Multiple cores work in parallel* |

### Wait Time: Single vs Multi-Threaded

**Single Thread** — Wait time occurs when:

- Blocked on I/O (disk, network)
- Sleeping (e.g., `sleep()`)
- Waiting for user input

*No competition for CPU — only external waits*

**Multi-Threaded** — Wait time **also** includes:

- Preempted by scheduler
- Waiting for a CPU core
- Other threads running

*Threads compete for limited CPU time*

### Wait Time in Multi-Threading

**Key Insight:** In multi-threaded programs, **total wait time grows** because threads must share CPU resources. Even without I/O, threads wait while others execute!

---

<!-- source pages 47-64 -->

## Exercise 3: Time Accounting

### Scenario

A program creates **5 threads**. Each thread:

- Needs **40 ms** of pure user-mode CPU execution
- Performs no I/O operations
- Does not voluntarily sleep
- Terminates immediately after finishing
- All threads terminate at approximately the same time.

**Total Work:** 5 threads $\times$ 40 ms = **200 ms** of user-mode work

---

### Part A – No Overhead

**Question.** Assume fair time slicing and **negligible context-switch overhead**.

For both **single-core** and **4-core** CPUs, calculate:

1. Minimum wall time
2. Total user time
3. Total system time
4. Total wait time (summed over all threads)
5. Is this concurrency, parallelism, or both?

**Hint:** Wait Time (per thread) = Wall Time $-$ Execution Time of that thread

#### Part A – Solution (Single Core)

*Figure: A single-core timeline from 0 to 200 ms filled with many small alternating colored slices, followed by "...", with a legend listing T1–T5 marked "(interleaved)" — the five threads share the one core in small round-robin slices.*

- **Wall Time:** $5 \times 40 = 200$ ms
- **User Time:** $200$ ms   **System Time:** $0$ ms
- **Wait Time:** Each thread waits while others run: $(200 - 40) \times 5 = 800$ ms

**Concurrency without parallelism** – threads overlap in time but not in execution

#### Wait Time Calculation (Single Core)

**How Wait Time is Calculated.** With **fair time slicing** (round-robin) under our naive assumption:

- All threads start at time 0
- Threads take turns in small time slices
- Each thread **terminates** when it completes its 40 ms of work
- With equal progress, all finish **approximately** around time 200 ms

**Note:** This is an **estimate** – threads cannot all finish at the exact same instant. In practice, they finish in quick succession as each completes its final time slice.

**Per-Thread Breakdown (Estimated)**

| Thread | Lifetime | Exec Time | Wait Time |
|---|---|---|---|
| T1 | 0 → ~200 ms | 40 ms | ~160 ms |
| T2 | 0 → ~200 ms | 40 ms | ~160 ms |
| T3 | 0 → ~200 ms | 40 ms | ~160 ms |
| T4 | 0 → ~200 ms | 40 ms | ~160 ms |
| T5 | 0 → ~200 ms | 40 ms | ~160 ms |
| **Total** | – | **200 ms** | **~800 ms** |

**Formula:**

$$\text{Wait Time} \approx \text{Lifetime} - \text{Execution Time} = 200 - 40 = 160 \text{ ms/thread}$$

#### Part A – (4 Cores)

- Assume that we now have 4 cores.
- Scheduling is now by FIFO order and once scheduled, threads run to completion.

#### Part A – Solution (4 Cores)

*Figure: Four core timelines — Core 1 runs T1 from 0 to 40 ms then T5 from 40 to 80 ms; Cores 2, 3 and 4 each run T2, T3 and T4 respectively from 0 to 40 ms and are then idle. Time axis marked 0, 40 ms, 80 ms.*

- **Wall Time:** $\lceil 5/4 \rceil \times 40 = 80$ ms
- **User Time:** $200$ ms   **System Time:** $0$ ms
- **Wait Time:** Only T5 waits 40 ms $\Rightarrow$ total $= 40$ ms

**Concurrency with parallelism** – User time (200 ms) > Wall time (80 ms)!

---

### Part B – Context Switching

**Additional Assumptions (Single Core)**

- Context switch every **5 ms** of execution
- Each context switch costs **0.2 ms**
- Context switch also occurs after the final chunk

**Calculate**

1. How many execution chunks per thread?
2. How many context switches total?
3. Total system time?
4. New wall time?
5. Total user time?
6. Total wait time?

#### Part B – Solution (Step 1)

**Basic Calculations**

- **Chunks per thread:** $40/5 = 8$ chunks
- **Total chunks:** $5 \times 8 = 40$ chunks
- **Context switches:** 40 (one after each chunk, <u>including final</u>)
- **System time:** $40 \times 0.2 = 8$ ms
- **User time:** $200$ ms (unchanged)

**Round Duration & Per-Thread Overhead**

- One full round = all 5 threads run once: $5 \times (5 + 0.2) = \mathbf{26}$ **ms**
- CPU burst per thread (user + CS): $40 + 8 \times 0.2 = \mathbf{41.6}$ **ms**

#### Part B – Solution (Important Note!)

**Assumption:** A context switch occurs **after every chunk**, including the final chunk of each thread. The thread is not considered complete until its final context switch finishes.

#### Part B – Solution (Step 2)

**Staggered Finish & Wait Times**

After **7 full rounds** ($7 \times 26 = 182$ ms), each thread has 1 chunk left.

In round 8, threads finish **one by one**, staggered by $5.2$ ms. Each turnaround **includes** the thread's own final context switch:

| Thread | Exec done | + final CS | Turnaround | Wait ($T_i - 41.6$) |
|---|---|---|---|---|
| $T_1$ | 187.0 ms | 187.2 ms | 187.2 ms | 145.6 ms |
| $T_2$ | 192.2 ms | 192.4 ms | 192.4 ms | 150.8 ms |
| $T_3$ | 197.4 ms | 197.6 ms | 197.6 ms | 156.0 ms |
| $T_4$ | 202.6 ms | 202.8 ms | 202.8 ms | 161.2 ms |
| $T_5$ | 207.8 ms | 208.0 ms | 208.0 ms | 166.4 ms |

#### Part B – Solution (Step 3)

**Results**

- **Wall time:** **208** ms
- **Total wait:** $145.6 + 150.8 + 156.0 + 161.2 + 166.4 = \mathbf{780}$ ms

#### Part B – Insight

- Context switching adds **system time** ($8$ ms). User work stays unchanged.
- Not all threads finish at the same time – in the final round, each thread waits for the ones scheduled before it.
- $T_5$ waits **20.8** ms **more** than $T_1$ ($4$ threads $\times$ $5.2$ ms gap).
- **Wait time** = Turnaround $-$ CPU burst (per thread, not a single multiplication).
- CPU burst includes **both** user time and the thread's own CS overhead: $40 + 1.6 = 41.6$ ms.

---

### Exercise 3: Key Takeaways

**Comparison Table**

| Metric | 1 Core | 4 Cores | 1 Core + CS |
|---|---|---|---|
| Wall Time | 200 ms | 80 ms | 208 ms |
| User Time | 200 ms | 200 ms | 200 ms |
| System Time | 0 ms | 0 ms | 8 ms |
| Wait Time | 800 ms | 40 ms | **780 ms** |

**Key Insights**

- User time stays constant – it's the **actual work**
- Parallelism reduces wall time and wait time dramatically
- Context switching adds system time **and** staggers thread completion, but total wait (780 ms) is actually *less* than sequential (800 ms) because threads overlap their execution via time-sharing

**One-Line Intuition**

> **Wall time** = how long it took
> **User + system time** = how much CPU you used
> **Wait time** = how long you existed but weren't running

**Remember**

- Wait time $\neq$ system time (common misconception!)
- Preemption without I/O still creates wait time
- With parallelism: User time > Wall time is normal

---

## See You Next Week!

Questions?
