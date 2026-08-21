# Recitation 4 — Scheduling

> Study notes converted from the recitation slide deck `Rec4_OS.pdf` (89 pages) — Operating Systems, Reichman University, Spring Semester 2026.
> Lecturer: Tsvi Cherny-Shahar · TAs: Daniel Karalnik, Liam Tal.
> Faithful conversion: every substantive item on the source pages appears here, and nothing has been added from outside them. Figures are rendered as italic *Figure:* descriptions.
> `<!-- source pages a-b -->` comments mark the original page ranges.

---

<!-- source pages 1-16 -->

## Table of Contents

1. Scheduling Intro
2. First Come First Serve
3. Shortest Job First
4. Round Robin
5. Multi Level Feedback Queues
6. Rotating Staircase Deadline Scheduler
7. Concluding Questions

---

## Scheduling Intro

### CPU Scheduler Overview

**The CPU Scheduler**

The **CPU Scheduler** is responsible for allocating CPU time among threads.

- Thread states: **New**, **Running**, **Ready**, **Wait**, **Terminated**
- Context switching **may** occur when:
  - A thread calls `yield()` — *a hint to the scheduler, not a guarantee*
  - A syscall blocks on a synchronization primitive (thread enters Wait state)
  - A timer interrupt fires (preemption)
- **Note**: `yield()` only suggests the scheduler consider switching; if no other thread is ready, the same thread may continue running

### Scheduling Mechanism vs Policy

**Policy**

Algorithm that determines **which** threads run **when**.

**Mechanism**

Maintains thread states, handles context switches, and tracks performance metrics.

**Why It Matters**

- **Responsiveness**: Timely reaction to user inputs
- **Fairness**: Ensuring all threads get CPU time, preventing starvation
- **CPU Utilization**: Keeping the CPU busy and efficient

### Preemptive Scheduling & Quantum

**Preemptive Scheduling**

The scheduler can **interrupt and switch out** a running thread.

- **Quantum**: The maximum time a thread can run before being preempted
- Considerations:
  - Long quantum $\Rightarrow$ Poor responsiveness
  - Short quantum $\Rightarrow$ High context-switch overhead

**Typical Values**

Quantum typically ranges from **10ms to 100ms** — balancing responsiveness with efficiency.

### Types of Scheduling Algorithms

**Offline Scheduling**

Job count and length are **known in advance**.

- First Come First Serve (FCFS)
- Shortest Job First (SJF)

**Online Scheduling**

**No prior knowledge** of job arrival or length.

- Round Robin
- Random Scheduling

---

## First Come First Serve

### First Come First Serve (FCFS)

**FCFS Overview**

Threads are executed **in the order they arrive**. Simple but can lead to the **convoy effect**.

- Simple to implement — just a FIFO queue
- Short threads get stuck behind long ones (convoy effect)
- Suitable for batch systems but not interactive systems

**Convoy Effect**

A single long-running thread can delay all subsequent threads, even if they are very short!

### FCFS Scheduling Example

**FCFS Scheduling** — Threads run in arrival order.

*Figure: Gantt chart on a time axis with ticks at 0, 2, 5, 7, 12 — four coloured blocks run back-to-back in arrival order: T1 from 0 to 2, T2 from 2 to 5, T3 from 5 to 7, T4 from 7 to 12.*

**Scenario:**
Threads arrive at times 0, 1, 2, and 3.
They are scheduled strictly in arrival order.

---

## Shortest Job First

### Shortest Job First (SJF)

**SJF Overview**

Executes the **shortest thread available** first. Optimal for minimizing average waiting time.

- **Preemptive SJF** (Shortest Remaining Time First) improves responsiveness
- Challenges:
  - Requires accurate prediction of thread burst lengths
  - Risk of **starvation** for longer threads

**Optimality**

SJF is **provably optimal** for minimizing average waiting time when all threads are known in advance.

### SJF Scheduling Example

**SJF (Shortest Job First) Scheduling** — Threads run in order of increasing CPU burst time.

*Figure: Gantt chart with four coloured blocks in the order T3, T2, T1, T4, each block wider than the previous one, on a time axis running from 0 to 10 with printed tick labels 0, 2, 5, 9, 10.*

*Note: on the slide the drawn block boundaries (blocks of widths 1, 2, 3 and 4 summing to 10) do not line up with the printed tick labels 0, 2, 5, 9, 10.*

**Scenario:**
All threads arrive at time 0.

---

## Round Robin

### Round Robin Scheduling

**Round Robin Overview**

Each thread gets an **equal share of CPU time** in a cyclic order.

- Good for time-sharing systems
- Performance depends on the chosen quantum
- Too small quantum $\Rightarrow$ High overhead
- Too large quantum $\Rightarrow$ Poor responsiveness

**Fairness**

Round Robin guarantees that no thread waits more than $(n - 1) \times q$ time units, where $n$ is the number of threads and $q$ is the quantum.

### Round Robin Scheduling Example

**Round Robin Scheduling** — Time-sliced execution with preemption.

*Figure: Gantt chart on a time axis with ticks every 2 units from 0 to 14, split into seven equal slices; a brace under the interval 0–2 is labelled "Time Quantum". The slices run T1, T3, T2, T1, T3, T2, T1.*

| Interval | Thread |
|---|---|
| 0–2 | T1 |
| 2–4 | T3 |
| 4–6 | T2 |
| 6–8 | T1 |
| 8–10 | T3 |
| 10–12 | T2 |
| 12–14 | T1 |

**Scenario:**
Each thread gets a fixed time slice.
If unfinished, it re-enters the back of the queue. (all threads arrive at time 0, order: T1, T3, T2)

---

<!-- source pages 17-28 -->

## Multi Level Feedback Queues

### MLFQ Overview

Threads are assigned to **different priority queues** (0, 1, 2, … where 0 is highest). Each queue has a **constant quantum**. Higher priority queues have smaller quanta.

- **Higher priority** for interactive (I/O-bound) tasks (smaller quantum)
- **Lower priority** for CPU-bound tasks (larger quantum)
- Rules:
  - New threads start at queue 0 (highest priority)
  - Threads are **promoted** if they yield or block before using full quantum
  - Threads are **demoted** if they use their entire quantum (preempted)

**Starvation Risk:** Without periodic priority boosting, low-priority threads may starve indefinitely!

### Queue Structure

*Figure: Three stacked queue boxes — Queue 0 – quantum = 2ms (top), Queue 1 – quantum = 4ms, Queue 2 – quantum = 8ms (bottom) — with a downward arrow beside them labelled "Priority", showing that priority decreases as the quantum grows.*

- New threads enter at queue 0 (highest priority)
- Each queue has a **constant quantum** (increases with lower priority)
- Threads using full quantum are demoted to lower-priority queues

---

### MLFQ Example

#### Setup

**Parameters**

- **Queue 0**: quantum = 2ms (highest priority)
- **Queue 1**: quantum = 4ms
- **Queue 2**: quantum = 8ms (lowest priority)
- All threads start in Queue 0
- $t_1$: CPU-bound, $t_2$: I/O-bound (blocks after 1ms)

*Figure: Initial state — an idle CPU icon on the left; queue rows 0, 1, 2 and a Wait row on the right. Queue 0 holds $t_1$ then $t_2$; queues 1, 2 and Wait are empty.*

#### Step 1 — $t_1$ Runs

*Figure: CPU box holds $t_1$ labelled "Runs 2"; the execution timeline shows $t_1$ occupying 0–2. Queue 0 now holds only $t_2$; queues 1, 2 and Wait are empty.*

- $t_1$ uses full quantum (2ms) → **preempted**
- Preempted at end of quantum → **demote to Queue 1**

#### Step 2 — $t_1$ Demoted

*Figure: CPU idle; a red arrow labelled "demoted" points from Queue 0 down to Queue 1, where $t_1$ now sits. Queue 0 holds $t_2$. Timeline still shows $t_1$ from 0–2.*

- $t_1$ moved to Queue 1 (lower priority)
- Scheduler picks from Queue 0 first → $t_2$ runs next

#### Step 3 — $t_2$ Blocks for I/O

*Figure: CPU box holds $t_2$ labelled "Runs 1"; timeline shows $t_1$ 0–2 followed by $t_2$ 2–3. Queue 0 is empty, Queue 1 holds $t_1$, and the Wait row holds $t_2$ marked "zzz".*

- $t_2$ runs 1ms then **blocks for I/O** (before quantum expires)
- Block/yield → **promoted to Queue 0** (stays at highest)
- $t_2$ moves to Wait area while I/O completes

#### Step 4 — $t_1$ Runs Again

*Figure: CPU box holds $t_1$ labelled "Runs 4"; timeline shows the earlier 0–3 segments followed by $t_1$ running 3–7. Queues 0, 1, 2 are empty; Wait holds $t_2$ "zzz".*

- Queue 0 empty → scheduler picks $t_1$ from Queue 1
- $t_1$ uses full quantum (4ms) → **demote to Queue 2**

#### Step 5 — $t_2$ Wakes Up

*Figure: CPU box labelled "Woke up!"; timeline runs to 7. A blue arrow labelled "promoted" curves from the Wait row up to Queue 0, where $t_2$ now sits; $t_1$ now sits in Queue 2.*

- $t_2$ I/O completes → returns to **Queue 0** (was promoted for blocking)
- $t_1$ demoted to Queue 2 (used full quantum in Queue 1)
- I/O-bound $t_2$ gets priority over CPU-bound $t_1$

#### Step 6 — Final State

*Figure: CPU box holds $t_2$; timeline shows the added $t_2$ segment 7–8. Queues 0 and 1 are empty, Queue 2 holds $t_1$, Wait is empty.*

**MLFQ Behavior**

- I/O-bound threads ($t_2$) stay at high priority – responsive!
- CPU-bound threads ($t_1$) sink to lower queues – fair sharing
- Block/yield → promote; preempted → demote

---

## Rotating Staircase Deadline Scheduler

---

<!-- source pages 29-42 -->

## Rotating Staircase Deadline Scheduler (RSDL)

### How RSDL Works

**RSDL Overview** — Threads are scheduled in a **round-robin** manner within each priority level. Uses **quotas** and **epochs** to ensure fairness and avoid starvation.

- **RR Interval**: Time slice for round-robin within a queue
- **Thread Quota**: Total CPU time a thread can use per epoch
- **Queue Quota**: Sum of all thread quotas in that queue
- **Epoch**: Period after which all quotas reset and threads return to starting priority

### RSDL Demotion Rules

**Thread Demotion Rules**

1. **Thread quota exhaustion**: Thread demoted to next lower queue, **quota refreshed**
2. **Queue quota exhaustion**: **ALL threads** in queue demoted together, **quotas refreshed**
3. **I/O blocking**: Thread **stays at same priority level** (no demotion)

**Note**: Queue quotas are fixed per epoch – NOT increased when threads are demoted into a queue.

### RSDL Epoch End Rules

**Epoch End and Reset**

**Epoch ends when**:

- All queue quotas depleted (lowest queue exhausted), OR
- No runnable threads (all waiting on I/O)

**On reset**:

- All thread quotas reset to initial values
- All queue quotas reset
- All threads return to their **original starting priority**

---

## RSDL Example: Full Walkthrough

### Setup

**Parameters**

- **RR Interval**: 3ms, **Thread Quota**: 6ms each
- 5 threads: $t_1, t_2, t_3$ in Queue 0; $t_4$ in Queue 1; $t_5$ in Queue 2
- Queue quota = sum of thread quotas (fixed per epoch, NOT changed by demotion)

*Figure: Initial state — CPU idle. Queue 0 (Q: 18) holds `t₁|6`, `t₂|6`, `t₃|6`; Queue 1 (Q: 6) holds `t₄|6`; Queue 2 (Q: 6) holds `t₅|6`; the Wait area is empty.*

### Step 1: $t_1$ Runs

- $t_1$ runs for RR interval (3ms), quota: 6→3
- Queue 0 quota: 18→15 (decremented by time used)
- $t_1$ moves to end of Queue 0 (round-robin)

*Figure: CPU shows $t_1$ "Runs 3". Queue 0 (Q: 15) now `t₂|6`, `t₃|6`, `t₁|3`; Queue 1 (Q: 6) `t₄|6`; Queue 2 (Q: 6) `t₅|6`. Timeline bar covers 0–3 with $t_1$.*

### Step 2: $t_2$ Blocks for I/O

- $t_2$ runs 2ms then **blocks for I/O**, quota: 6→4
- Queue 0 quota: 15→13
- $t_2$ **stays in Queue 0** (I/O block = no demotion)

*Figure: CPU shows $t_2$ "Runs 2". Queue 0 (Q: 13) now `t₃|6`, `t₁|3`; Queue 1 (Q: 6) `t₄|6`; Queue 2 (Q: 6) `t₅|6`; Wait area holds `t₂|4 zzz`. Timeline: $t_1$ 0–3, $t_2$ 3–5.*

### Step 3: $t_1$ Exhausts Quota

- After $t_3$ runs 3ms, $t_1$ runs 3ms (quota: 3→0)
- $t_1$ **quota exhausted → demoted to Q1, quota refreshed to 6**
- Q1 quota stays 6 (fixed per epoch, not increased by demotion)

*Figure: CPU shows $t_1$ "Runs 3". Queue 0 (Q: 7) holds `t₃|3`; a red "demoted" arrow points from Queue 0 down into Queue 1 (Q: 6), which now holds `t₄|6`, `t₁|6`; Queue 2 (Q: 6) `t₅|6`; Wait holds `t₂|4 zzz`. Timeline marks 0, 5, 8, 11.*

### Step 4: $t_3$ Exhausts Quota

- $t_3$ runs 3ms (quota: 3→0) → demoted to Q1
- Q0 quota: 7→4 (only $t_2$'s 4ms remaining, sleeping)
- Q1 quota stays 6 (fixed per epoch)

*Figure: CPU shows $t_3$ "Runs 3". Queue 0 (Q: 4) is empty; "demoted" arrow into Queue 1 (Q: 6), now `t₄|6`, `t₁|6`, `t₃|6`; Queue 2 (Q: 6) `t₅|6`; Wait holds `t₂|4 zzz`. Timeline marks 11, 14.*

### Step 5: $t_2$ Wakes Up

- $t_2$ I/O completes, returns to **Queue 0** (same level)
- $t_2$ has 4ms quota, Q0 quota is 4ms
- Both will exhaust together!

*Figure: CPU labelled "Woke!". Queue 0 (Q: 4) holds `t₂|4`; Queue 1 (Q: 6) holds `t₄|6`, `t₁|6`, `t₃|6`; Queue 2 (Q: 6) `t₅|6`; Wait area empty. Timeline reaches 14.*

### Step 6: $t_2$ Runs, Q0 Exhausts

- $t_2$ runs 4ms, quota: 4→0, **Q0 quota: 4→0**
- Thread quota exhausted → $t_2$ demoted, quota refreshed to 6
- Q1 quota stays 6 (NOT increased by demotion)

*Figure: CPU shows $t_2$ "Runs 4". Queue 0 is empty with a red "Q: 0!"; Queue 1 (Q: 6) now holds `t₄|6`, `t₁|6`, `t₃|6`, `t₂|6`; Queue 2 (Q: 6) `t₅|6`. Timeline runs 14–18.*

### Step 7: Q1 Runs, Q1 Exhausts

- Q1 has 4 threads (24ms quota) but only 6ms queue quota!
- After $t_4$ runs 3ms, $t_1$ runs 3ms → **Q1 quota exhausted**
- **All Q1 threads mass-demoted to Q2, quotas refreshed**

*Figure: CPU shows $t_4$, labelled "Q1 runs". Queue 0 empty (Q: 0 in red); Queue 1 (Q: 6) holds `t₄|6`, `t₁|6`, `t₃|6`, `t₂|6`; Queue 2 (Q: 6) `t₅|6`. Timeline reaches 18.*

### Step 8: Epoch Ends

*Figure: CPU labelled "Epoch Ends!". Queue 0 (Q: 0) and Queue 1 (Q: 0) are empty; Queue 2 (Q: 0) holds $t_1$, $t_2$, $t_3$, $t_4$, $t_5$; Wait area empty.*

**Epoch End Conditions** — Epoch ends when:

- All queue quotas exhausted (no more runnable time), OR
- No runnable threads (all waiting on I/O)

### Step 9: Epoch Reset

*Figure: CPU labelled "New Epoch!", with a blue "reset" arrow curving back up. Queue 0 (Q: 18) holds `t₁|6`, `t₂|6`, `t₃|6`; Queue 1 (Q: 6) `t₄|6`; Queue 2 (Q: 6) `t₅|6`; Wait area empty — identical to the initial setup.*

**Epoch Reset**

- All thread quotas reset to 6ms
- All threads return to **original starting priorities**
- Queue quotas reset: Q0=18, Q1=6, Q2=6

---

## Concluding Questions

---

<!-- source pages 43-60 -->

## General Scheduling Concepts (1/2)

**True or False**

- The CPU scheduler is responsible for determining which thread gets to use the CPU next.
  **Answer: True**
- A context switch occurs only when a thread voluntarily yields the CPU.
  **Answer: False**

---

## General Scheduling Concepts (2/2)

**True or False**

- The goal of scheduling policies is to maximize fairness, responsiveness, and CPU utilization.
  **Answer: True**
- Preemptive scheduling allows the scheduler to interrupt a running thread before it completes execution.
  **Answer: True**
- The quantum in preemptive scheduling is the maximum amount of time a thread can run before being preempted.
  **Answer: True**

---

## Scheduling Algorithms (1/3)

**True or False**

- FCFS scheduling is optimal for minimizing the average waiting time in all scenarios.
  **Answer: False**
- FCFS scheduling may lead to the convoy effect, where short threads are delayed by long threads.
  **Answer: True**
- SJF scheduling always ensures fair CPU time distribution among all threads.
  **Answer: False**
- Preemptive SJF (SRTF) can prevent long threads from experiencing starvation.
  **Answer: False**

---

<!-- source pages 61-76 -->

## Scheduling Algorithms (2/3) — True or False

- Round Robin scheduling ensures that each thread gets an equal share of CPU time in a cyclic order.
  **Answer: True**
- In Round Robin scheduling, using an extremely small quantum reduces context switch overhead.
  **Answer: False**
- MLFQ prioritizes interactive (I/O-bound) tasks by assigning them higher priority queues.
  **Answer: True**
- In MLFQ, each queue has a constant quantum, with higher priority queues having smaller quanta.
  **Answer: True**

---

## Scheduling Algorithms (3/3) — True or False

- MLFQ scheduling guarantees that all threads will eventually receive CPU time, preventing starvation completely.
  **Answer: False**
- In RSDL, when a thread blocks for I/O, it is demoted to the next lower queue.
  **Answer: False** (stays at same level)
- In RSDL, when a queue's quota is exhausted, all threads in that queue are demoted together.
  **Answer: True**
- In RSDL, at the end of an epoch, all thread quotas reset and threads return to their starting priority.
  **Answer: True**

---

<!-- source pages 77-89 -->

## Open Question: Autonomous Drone Scheduling

### Scenario

An autonomous delivery drone must schedule the following tasks efficiently:

- **Obstacle Detection (OD)** – Must process data within 50ms to avoid collisions
- **Route Calculation (RC)** – Runs every 2 seconds but can tolerate slight delays
- **Battery Monitoring (BM)** – Runs every 5 seconds and is the least time-sensitive

### Questions

- Which scheduling algorithm would you use, and why?
- How do you ensure that OD always meets its deadline?
- What happens if RC and BM take too long?
- Would you use preemptive or non-preemptive scheduling?

### Answer

- **MLFQ is the best choice:** OD gets the highest-priority queue, ensuring it runs first. RC and BM are in lower queues.
- **Preemption ensures OD meets its 50ms deadline:** Lower-priority threads are preempted if OD needs CPU time.
- **RC and BM are scheduled fairly:** RC runs in a mid-priority queue and BM in the lowest queue, with priority boosting to prevent starvation.
- **Why not RSDL?** RSDL enforces fairness through epoch resets, but doesn't guarantee OD always runs first, making it unsuitable for real-time constraints.
- **Dynamic priority adjustments:** If RC or BM is delayed too often, priority boosting ensures they eventually execute without delaying OD.

---

## Scheduling Performance Question

### Problem Setup

Three threads with arrival times, CPU bursts, and I/O:

| Thread | Arrival | CPU$_1$ | I/O | CPU$_2$ |
|---|---|---|---|---|
| $T_1$ | 0 | 3 | 2 | 2 |
| $T_2$ | 1 | 3 | – | – |
| $T_3$ | 2 | 4 | 3 | 4 |

($T_2$ has no I/O; times in ms; total burst: $T_1$=5, $T_2$=3, $T_3$=8)

### Parameters

- **Round Robin**: quantum = 2ms
- **Modified MLFQ**:
  - Q1 quantum = 2ms, Q2 quantum = 4ms, Q3 FCFS policy
  - New threads start in Q1; demote on full-quantum; stay on I/O block
- **Modified RSDL**:
  - Thread quotas: $T_1$=5, $T_2$=3, $T_3$=8
  - Queue quotas = sum of thread quotas; RR Interval = 2ms
  - Demote on quota exhaustion; stay on I/O block; reset at epoch end

### Tasks

Compute the following for each algorithm:

1. Completion time
2. Turnaround time (completion – arrival)
3. Waiting time (turnaround – burst)

For each of these algorithms:

1. First-Come, First-Serve (FCFS)
2. Round Robin
3. Modified Multi-Level Feedback Queue
4. Modified RSDL

---

## FCFS Solution (with I/O)

### Gantt Chart

*Figure: A Gantt chart of five CPU segments in order — 0–3 running $T_1$, 3–6 running $T_2$, 6–10 running $T_3$, 10–12 running $T_1$, 12–16 running $T_3$ — each interval brace-labelled with its thread.*

$T_1$ I/O: 3–5, $T_3$ I/O: 10–13

### Solution

- **Completion times**: $C_1 = 12$, $C_2 = 6$, $C_3 = 16$
- **Turnaround times** $T_i = C_i - A_i$:

$$T_1 = 12 - 0 = 12, \quad T_2 = 6 - 1 = 5, \quad T_3 = 16 - 2 = 14$$

- **Waiting times** $W_i = T_i - B_i - IO_i$:

$$W_1 = 12 - 5 - 2 = 5, \quad W_2 = 5 - 3 = 2, \quad W_3 = 14 - 8 - 3 = 3$$

---

## Round Robin (Quantum = 2ms, with I/O)

### Execution Trace

- 0–2: $T_1$ (rem 1), 2–4: $T_2$ (rem 1), 4–6: $T_3$ (rem 2)
- 6–7: $T_1$ finishes CPU$_1$ $\rightarrow$ I/O (7–9)
- 7–8: $T_2$ finishes, 8–10: $T_3$ (rem 0) $\rightarrow$ I/O (10–13)
- 10–12: $T_1$ returns, finishes CPU$_2$
- 13–17: $T_3$ returns, runs CPU$_2$ (4ms)

### Solution

- **Completion:** $C_1 = 12$, $C_2 = 8$, $C_3 = 17$
- **Turnaround:** $T_1 = 12$, $T_2 = 7$, $T_3 = 15$
- **Waiting:** $W_1 = 12 - 5 - 2 = 5$, $W_2 = 7 - 3 = 4$, $W_3 = 15 - 8 - 3 = 4$

---

## Multi-Level Feedback Queue (with I/O)

### Execution Steps

1. **Q1 (2ms each):** $T_1$ 0–2 (rem 1) $\downarrow$Q2, $T_2$ 2–4 (rem 1) $\downarrow$Q2, $T_3$ 4–6 (rem 2) $\downarrow$Q2
2. **Q2:** $T_1$ 6–7 $\rightarrow$ I/O (7–9), stays Q2; $T_2$ 7–8 fin; $T_3$ 8–10 $\rightarrow$ I/O (10–13), stays Q2
3. **Q2 cont:** $T_1$ returns 9, runs 9–11 fin; $T_3$ returns 13, runs 13–17 fin

### Solution

- **Completion:** $C_1 = 11$, $C_2 = 8$, $C_3 = 17$
- **Turnaround:** $T_1 = 11$, $T_2 = 7$, $T_3 = 15$
- **Waiting:** $W_1 = 11 - 5 - 2 = 4$, $W_2 = 7 - 3 = 4$, $W_3 = 15 - 8 - 3 = 4$

---

## RSDL (Modified, with I/O)

### Parameters

Thread quotas: $T_1$=5, $T_2$=3, $T_3$=8. Queue quota = sum of thread quotas = 16.
RR Interval = 2ms. Three queues: High, Medium, Low.

### Execution Trace

1. **High queue, RR:** $T_1$ 0–2 (quota 5$\rightarrow$3), $T_2$ 2–4 (quota 3$\rightarrow$1), $T_3$ 4–6 (quota 8$\rightarrow$6)
2. $T_1$ 6–7 $\rightarrow$ I/O (7–9), **stays High** (I/O block)
3. $T_2$ 7–8 finishes (quota=0)
4. $T_3$ 8–10 $\rightarrow$ I/O (10–13), **stays High**
5. $T_1$ returns at 9, runs 9–11 (finishes CPU$_2$, quota exhausted)
6. $T_3$ returns at 13, runs 13–17 (finishes CPU$_2$)

Note: $T_1$ and $T_3$ stayed in High queue due to I/O blocking rule.

### Solution

$$C_1 = 11, \quad \text{Turnaround}_1 = 11, \quad W_1 = 11 - 5 - 2 = 4$$
$$C_2 = 8, \quad \text{Turnaround}_2 = 7, \quad W_2 = 7 - 3 = 4$$
$$C_3 = 17, \quad \text{Turnaround}_3 = 15, \quad W_3 = 15 - 8 - 3 = 4$$
