# CPU Scheduling

> Study notes converted from the lecture slide deck `OS Presentation 4 - 1 Slide-2.pdf` (65 pages) — Operating Systems, Reichman University, Spring Semester 2026.
> Lecturer: Tsvi Cherny-Shahar · TAs: Daniel Karalnik, Liam Tal.
> Faithful conversion: every substantive item on the source pages appears here, and nothing has been added from outside them. Figures are rendered as italic *Figure:* descriptions.
> `<!-- source pages a-b -->` comments mark the original page ranges.

---

<!-- source pages 1-15 -->

## Presentation 4 — How to schedule all those threads

*Figure: Title slide over a macOS kernel-panic screen ("You need to restart your computer. Hold down the Power button for several seconds or press the Restart button.", repeated in French, German and Polish) — captioned "Operating Systems - Presentation 4 / How to schedule all those threads".*

### Acknowledgments

Thanks to:

- Prof. Anat Bremler-Barr
- Prof. Gadi Taubenfeld
- Dr. Guy Katz and Prof. David Hay

Also, resources from:

- OS/161 Instructional Operating System (os161.org)
- ops-class.org

---

## When do we context switch? To whom do we context switch?

- The **CPU Scheduler** is responsible for **scheduling** CPU-time between the threads

*Figure: Thread state diagram — New → Ready; Ready → Running labelled "Scheduled to run"; Running → Ready labelled "yield()" / "out of time"; Running → Wait labelled "syscall" / "blocked by sync primitive"; Wait → Ready labelled "Event has occurred (syscall returned)"; Running → Terminated labelled "Exit" / "OS termination".*

---

## Scheduling Mechanism vs Policy

- **Policy** — algorithm that decides the **which and when** the threads are executed
- **Mechanism** — maintain threads states, context switch, measurements

---

## IO-bound & CPU-bound

**I/O-bound** threads may leave the CPU idle.

**CPU-bound** threads can "take over" the CPU.

*Figure: Three example processes — $P_1$ is a "Dynamic Buttons" GUI window with three buttons and a mouse cursor, $P_2$ is a "Downloading" progress bar, and $P_3$ is a box reading "Calculate the $n_{th}$ digit of $\pi$". $P_1$ and $P_2$ illustrate I/O-bound work; $P_3$ illustrates CPU-bound work.*

---

## Why does it matter?

- **Responsiveness** — click a button, press a key, move the mouse
  - If a process doesn't get execution time, it won't respond
- **Fairness** — *no starvation*. Each process should, eventually, run and end
- **CPU utilization** — maximum utilization (minimum idle time)
- **Bad scheduling** makes the computer feels "slow" and "laggy", even though the hardware is fit for the task

---

## Preemptive Scheduling & Quantum

Assuming **we don't know anything** about the running threads **we cannot tell when they stop**.

**Preemptive scheduling** means the scheduler **can stop** a thread **mid-work**, to allow others to run.

**Scheduling Quantum** is the *maximum* **time** a thread runs continuously before the OS preempts the thread.

In other words, after *quantum* **time**, threads moves from *running* → *ready*.

**Quantum is too long** → affects responsiveness
**Quantum is too short** → context switch overhead
(usually between 10ms to 100ms)

---

## Types of scheduling algorithms

- **Off-Line** — Jobs count and their length is known
- **On-Line** — No information about the jobs and their length

---

## First Come First Serve (FCFS - offline)

- Assume we know all the existing jobs (threads) and their required CPU time

*Figure: CPU timeline bar — a green block of length 6 (ends at time 6), then an orange block of length 3 (ends at 9), then a purple block of length 1 (ends at 10), along an arrow labelled "time".*

$$\text{Average wait time: } \frac{0+6+9}{3} = 5$$

$$\text{Average response time: } \frac{6+9+10}{3} = 8.33$$

$$\text{Throughput: } \frac{10}{3} = 3.33$$

Short jobs get stuck after long ones…

---

## Shortest Job First (SJF - offline)

*Figure: CPU timeline bar — a purple block of length 1 (ends at time 1), then an orange block of length 3 (ends at 4), then a green block of length 6 (ends at 10), along an arrow labelled "time". Same three jobs as FCFS, reordered shortest-first.*

$$\text{Average wait time: } \frac{0+1+4}{3} = 1.67$$

$$\text{Average response time: } \frac{1+4+1}{3} = 5$$

$$\text{Throughput: } \frac{10}{3} = 3.33$$

Short jobs get stuck after long ones…

*Note: the slide repeats the caption "Short jobs get stuck after long ones…" from the FCFS slide, and the average response time numerator is written as $1+4+1$ although the completion times shown are 1, 4 and 10 — transcribed as printed.*

### SJF properties

- SJF is optimal!
- Preemptive version of SJF can even do better!
- Intuitive "proof":
  - If you switch any two jobs, the numerator of the "average response" and "wait" increases
  - Can be proven by contradiction
- **Conclusion:**
  **Short CPU bursts, common in interactive (I/O bound) threads, should scheduled first! (while not starving others…)**

### But we don't

All we need is to know how many threads and their length…

**But we don't**

---

## Estimate as we go…

- **Assuming** threads "behavior" stays the same, the scheduler can "measure" the CPU bursts over time and estimate the future:

$$\tau_{n+1} = \alpha\, t_n + (1 - \alpha)\, \tau_n$$

- $t_n$ = actual length of the $n_{th}$ CPU burst
- $\tau_{n+1}$ = predicted value for the next CPU burst
- $\alpha,\ 0 \le \alpha \le 1$

If $\alpha = 0$ → $\tau_{n+1} = \tau_n$

If $\alpha = 1$ → $\tau_{n+1} = t_n$

We can modify $\alpha$ to decide the "affect" of previous CPU rounds on the next round

Limitations:

- It takes time to improve estimation
- Evaluation is only for the CPU bursts
- **Assumes the threads behave the same over time**

---

<!-- source pages 16-30 -->

## On-line scheduling

- Do not assume anything about the incoming jobs:
  - Not their arrival time
  - Not their length
  - Not their behavior

---

## Random (online)

*Figure: the standard illustration used throughout this section — a CPU icon at the left with a horizontal "time" arrow to its right, on which executed slices are drawn as coloured blocks left to right; below it a yellow **Ready** box and a red **Wait** box. Four threads $t_1$ (orange), $t_2$ (purple), $t_3$ (green) and $t_4$ (blue) move between the boxes and the CPU along arrows.*

The trace runs as follows (each step is one animation frame):

| Step | CPU timeline so far | Ready | Wait | Transition shown |
|---|---|---|---|---|
| 1 | (empty) | $t_1, t_2, t_3, t_4$ | — | An arrow goes from the Ready box up to the CPU: a thread is picked at random. |
| 2 | $t_1$ | $t_2, t_3, t_4$ | — | $t_1$ has finished its slice and an arrow takes it back down into Ready; another arrow takes $t_3$ up to the CPU. |
| 3 | $t_1, t_3$ | $t_1, t_2, t_4$ | — | $t_3$'s block is shorter than a full slice, and its arrow goes from the CPU into the **Wait** box; an arrow takes $t_4$ up to the CPU. |
| 4 | $t_1, t_3, t_4$ | $t_1, t_2$ | $t_3$ | $t_4$ returns from the CPU into Ready; another arrow goes from Ready up to the CPU; a third arrow moves $t_3$ from Wait back into Ready. |
| 5 | $t_1, t_3, t_4, t_1$ | $t_2, t_3, t_4$ | — | $t_1$ returns from the CPU into Ready; an arrow takes $t_3$ up to the CPU. |
| 6 | $t_1, t_3, t_4, t_1, t_3$ | $t_1, t_2, t_4$ | — | Final frame: $t_2$ is drawn with a crying face next to it — it has still not been chosen once. |

*Figure: the last frame — the CPU timeline reads $t_1 \mid t_3 \mid t_4 \mid t_1 \mid t_3$, and $t_2$, still sitting in Ready, is marked with a crying-face emoji.*

---

## Round Robin (online)

The trace runs as follows (each step is one animation frame):

| Step | CPU timeline so far | Ready | Wait |
|---|---|---|---|
| 1 | (empty) | $t_1, t_2, t_3, t_4$ | — |
| 2 | $t_1$ | $t_2, t_3, t_4$ | — |
| 3 | $t_1, t_2$ | $t_1, t_3, t_4$ | — |
| 4 | $t_1, t_2, t_3$ | $t_1, t_2, t_4$ | — |
| 5 | $t_1, t_2, t_3, t_4$ | $t_1, t_2$ | $t_3$ |
| 6 | $t_1, t_2, t_3, t_4, t_1$ | $t_2, t_4$ | $t_3$ |
| 7 | $t_1, t_2, t_3, t_4, t_1, t_2$ | $t_1, t_4$ | $t_3$ |
| 8 | $t_1, t_2, t_3, t_4, t_1, t_2, t_4$ | $t_1, t_2$ | $t_3$ |

At step 4 the block drawn for $t_3$ on the timeline is much narrower than the others, and from step 5 onwards $t_3$ sits in the **Wait** box; the following rounds cycle over $t_4, t_1, t_2, t_4$.

*Figure: the last frame — CPU timeline $t_1 \mid t_2 \mid t_3 \mid t_4 \mid t_1 \mid t_2 \mid t_4$ with $t_3$'s block visibly shorter than the rest; Ready holds $t_1$ and $t_2$, Wait holds $t_3$.*

---

<!-- source pages 31-47 -->

## Rewarding I/O-bound threads

That's great,

But we want to "reward" I/O bound thread

(remember SJF – that's why…)

---

## Multi-level Feedback Priority Queues

We want to differentiate the threads,

- Some should be more responsive (e.g. game, video)
- Some can be less responsive (e.g. background task)
- Some in the middle… (e.g. word, powerpoint)

### The algorithm

I/O bound threads get priority!

- Choose *quantum*
- Set priority queues, each has a different level from 0 (highest) to N (lowest)
- All/Arriving threads start at level 0

<!-- -->

- Pick a thread from the highest non-empty queue
- If a thread blocks/yields $\rightarrow$ promote
- If a thread preempted at the end of the quantum $\rightarrow$ demote

### Worked trace

*Figure: the trace uses a CPU timeline (blocks laid out left to right along a "time" arrow), three priority queues drawn as yellow boxes labelled 0, 1 and 2, and a pink "Wait" box for blocked threads. Four threads $t_1$ (orange), $t_2$ (purple), $t_3$ (green) and $t_4$ (blue) move between them. Wide CPU blocks mean the thread used its whole quantum; narrow blocks mean it blocked or yielded early.*

**Initial state** — queue 0: $t_1, t_2, t_3, t_4$; queue 1: empty; queue 2: empty; Wait: empty. CPU idle.

| # | Runs on CPU | Queue 0 | Queue 1 | Queue 2 | Wait | What changed |
|---|---|---|---|---|---|---|
| 1 | $t_1$ (full quantum) | $t_2, t_3, t_4$ | — | — | — | $t_1$ taken from queue 0 |
| 2 | $t_3$ (short) | $t_2, t_4$ | $t_1$ | — | — | $t_1$ preempted at end of quantum $\rightarrow$ demoted to level 1 |
| 3 | $t_2$ | $t_4$ | $t_1$ | — | $t_3$ | $t_3$ blocked $\rightarrow$ moved to Wait |
| 4 | $t_4$ (full quantum) | — | $t_1, t_2$ | — | $t_3$ | $t_2$ preempted $\rightarrow$ demoted to level 1 |
| 5 | $t_1$ (full quantum) | — | $t_2, t_4$ | — | $t_3$ | $t_4$ preempted $\rightarrow$ demoted to level 1; $t_1$ taken from head of level 1 |
| 6 | $t_2$ (short) | $t_3$ | $t_4$ | $t_1$ | — | $t_1$ preempted $\rightarrow$ demoted to level 2; $t_3$'s wait ended $\rightarrow$ back in level 0 |
| 7 | $t_3$ (short) | $t_2$ | $t_4$ | $t_1$ | — | $t_2$ blocked/yielded $\rightarrow$ promoted to level 0 |
| 8 | $t_2$ (short) | — | $t_4$ | $t_1$ | $t_3$ | $t_3$ blocked $\rightarrow$ Wait |
| 9 | $t_4$ (short) | $t_2$ | — | $t_1$ | $t_3$ | $t_2$ blocked/yielded $\rightarrow$ level 0 |
| 10 | $t_2$ | $t_3$ | — | $t_1$ | $t_4$ | $t_4$ blocked $\rightarrow$ Wait; $t_3$'s wait ended $\rightarrow$ level 0 |
| 11 | $t_3$ | $t_4, t_2$ | — | $t_1$ | — | $t_4$'s wait ended $\rightarrow$ level 0; $t_2$ blocked/yielded $\rightarrow$ level 0 |
| 12 | $t_4$ | $t_3, t_2$ | — | $t_1$ | — | $t_3$ blocked/yielded $\rightarrow$ level 0 |

Final CPU timeline: $t_1, t_3, t_2, t_4, t_1, t_2, t_3, t_2, t_4, t_2, t_3, t_4$.

At the last step $t_1$ is still sitting alone at level 2, with the caption:

> $t_1$ starts to feel left out…

*Figure: at step 12 a crying-face emoji is drawn next to $t_1$ in queue 2, while queue 0 holds $t_3$ and $t_2$ and queue 1 is empty.*

### The problem

"Just" prioritizing I/O bound threads can lower their priority too low… and cause **starvation**

*Figure: a decorative photo of a champagne-glass tower.*

---

<!-- source pages 48-65 -->

## Completely Fair Scheduler (CFS)

- Before 2.6, Linux scheduler algorithm was $O(n)$ ($n$ – running threads)
- Between Linux 2.6.21 and 2.6.24 **Ingo Molnár** worked on a "completely fair" scheduler, inspired by a previous work done by **Con Kolivas**.
- **Ingo Molnár** CFS algorithm improved the algorithm to $O(1)$ scheduling decision while keeping the scheduling "fair"

*Figure: photograph of Ingo Molnár at a Linux symposium, captioned "Ingo Molnár".*

---

## Rotating Staircase Deadline Scheduler (RSDL)

- Later, **Con Kolivas**, inspired by Ingo Molnár CFS, created RSDL, which its algorithm and code is much simpler compared to CFS

*Figure: photograph captioned "Con Kolivas".*

### Structure

- Each thread has a **pre-assigned** priority (usually derived from the process)
- Each thread assigned with a pre-configured **quota** (=`RR_INTERVAL`)
  - Default is 6ms
- There are $n$ priority queues
  - Each queue assigned with **queue quota = sum(quotas of threads in queue)**
  - $n$ is the highest, 0 is the lowest
- The algorithm execution is based on repeatedly run **epochs**
  - Once an **epoch** finishes, it **resets** and starts all over again

### Epoch rules

- Epoch round-robin the threads by their priority and quota
- Once the **quotas exhausted** or **no threads can run**, the epoch is complete

**Epoch:**

- Run the next thread from the highest available queue (minding the quota)
- If a thread **blocks** or **yields** → remain in the same level
- If a thread runs out of quota (used all its CPU) → **demote** a level (to the **end of the queue**)
  - **Thread** quota refreshed (NOT QUEUE)
- If the **queue** runs out of quota (max CPU for the queue) → **demote** all threads
  - **Threads** quota refreshed (NOT QUEUE)

---

## RSDL worked example (epoch trace)

*Figure: each frame shows a CPU box at the top holding the currently running thread, a red "Runs k" caption saying how many units it actually ran, and three priority queues drawn as horizontal bars labelled 2 (highest), 1 and 0 (lowest). Each thread is a box labelled $t_i \mid q$ where $q$ is its remaining thread quota; the number in the right-hand cell of each bar is the queue quota. A "zZz" icon on a thread box means it blocked / is sleeping.*

Initial state (5 threads, all with quota 6):

| Level | Queue contents | Queue quota |
|---|---|---|
| 2 | $t_3\mid 6$ | 6 |
| 1 | $t_1\mid 6$, $t_2\mid 6$, $t_4\mid 6$ | 18 |
| 0 | $t_5\mid 6$ | 6 |

CPU is idle at this point.

The epoch then proceeds step by step:

**Step 1 — CPU: $t_3\mid 6$, Runs 3**

| Level | Queue contents | Queue quota |
|---|---|---|
| 2 | (empty) | 6 |
| 1 | $t_1\mid 6$, $t_2\mid 6$, $t_4\mid 6$ | 18 |
| 0 | $t_5\mid 6$ | 6 |

**Step 2 — CPU: $t_1\mid 6$, Runs 6**

$t_3$ ran only 3 and blocked (zZz), so it stays at level 2 with $t_3\mid 3$.

| Level | Queue contents | Queue quota |
|---|---|---|
| 2 | $t_3\mid 3$ (sleeping) | 3 |
| 1 | $t_2\mid 6$, $t_4\mid 6$ | 18 |
| 0 | $t_5\mid 6$ | 6 |

**Step 3 — CPU: $t_2\mid 6$, Runs 6**

$t_1$ used all its quota → demoted to level 0, appended at the end, quota refreshed to 6. $t_3$ is marked "Woke up".

| Level | Queue contents | Queue quota |
|---|---|---|
| 2 | $t_3\mid 3$ (Woke up) | 3 |
| 1 | $t_4\mid 6$ | 12 |
| 0 | $t_5\mid 6$, $t_1\mid 6$ | 6 |

**Step 4 — CPU: $t_3\mid 3$, Runs 3**

$t_2$ used all its quota → demoted to level 0 with quota refreshed to 6.

| Level | Queue contents | Queue quota |
|---|---|---|
| 2 | (empty) | 3 |
| 1 | $t_4\mid 6$ | 6 |
| 0 | $t_5\mid 6$, $t_1\mid 6$, $t_2\mid 6$ | 6 |

**Step 5 — CPU: $t_4\mid 6$, Runs 4**

$t_3$ used the remaining 3 of its quota → demoted to level 1, quota refreshed to 6.

| Level | Queue contents | Queue quota |
|---|---|---|
| 2 | (empty) | 0 |
| 1 | $t_3\mid 6$ | 6 |
| 0 | $t_5\mid 6$, $t_1\mid 6$, $t_2\mid 6$ | 6 |

**Step 6 — CPU: $t_3\mid 6$, Runs 2 (queue exhausts)**

$t_4$ ran 4 and blocked (zZz), so it stays at level 1 with $t_4\mid 2$; the level-1 queue quota drops to 2.

| Level | Queue contents | Queue quota |
|---|---|---|
| 2 | (empty) | 0 |
| 1 | $t_4\mid 2$ (sleeping) | 2 |
| 0 | $t_5\mid 6$, $t_1\mid 6$, $t_2\mid 6$ | 6 |

**Step 7 — CPU: $t_5\mid 6$, Runs 1**

$t_3$ ran 2, which exhausts the level-1 **queue** quota → all threads of level 1 ($t_4$ and $t_3$) are demoted to level 0.

| Level | Queue contents | Queue quota |
|---|---|---|
| 2 | (empty) | 0 |
| 1 | (empty) | 0 |
| 0 | $t_1\mid 6$, $t_2\mid 6$, $t_4\mid 2$ (sleeping), $t_3\mid 4$ | 6 |

*Note: slide 51 states that on queue exhaustion the threads' quotas are refreshed, but the diagram shows the demoted threads keeping their remaining quotas ($t_4\mid 2$, $t_3\mid 4$); transcribed as drawn.*

**Step 8 — CPU: $t_1\mid 6$, Runs 1**

$t_5$ ran 1 and blocked, staying at level 0 as $t_5\mid 5$.

| Level | Queue contents | Queue quota |
|---|---|---|
| 2 | (empty) | 0 |
| 1 | (empty) | 0 |
| 0 | $t_2\mid 6$, $t_4\mid 2$ (sleeping), $t_3\mid 4$, $t_5\mid 5$ (sleeping) | 5 |

**Step 9 — CPU: $t_2\mid 6$, Runs 1**

| Level | Queue contents | Queue quota |
|---|---|---|
| 2 | (empty) | 0 |
| 1 | (empty) | 0 |
| 0 | $t_4\mid 2$ (sleeping), $t_3\mid 4$, $t_5\mid 5$ (sleeping), $t_1\mid 5$ (sleeping) | 4 |

**Step 10 — CPU: $t_3\mid 4$, Runs 2**

| Level | Queue contents | Queue quota |
|---|---|---|
| 2 | (empty) | 0 |
| 1 | (empty) | 0 |
| 0 | $t_4\mid 2$ (sleeping), $t_5\mid 5$ (sleeping), $t_1\mid 5$ (sleeping), $t_2\mid 5$ (sleeping) | 3 |

**Step 11 — CPU idle: No runnable threads, end of EPOCH**

$t_3$ ran 2 and blocked, returning to level 0 as $t_3\mid 2$. Every thread is now sleeping.

| Level | Queue contents | Queue quota |
|---|---|---|
| 2 | (empty) | 0 |
| 1 | (empty) | 0 |
| 0 | $t_4\mid 2$, $t_5\mid 5$, $t_1\mid 5$, $t_2\mid 5$, $t_3\mid 2$ — all sleeping | 1 |

Callout on the slide:

> **No runnable threads**
> end of EPOCH
>
> (another scenario: all queues' quotas deplete)

The final frame repeats this state with the author's note:

> TODO: add somehow the pointer that points to the top queue to get O(1).

---

## TODO: Who's the scheduler thread?!

- Add that the scheduler thread is the thread that executes. The CPU has instruction to perform interrupt that time is out, than this thread jumps to the scheduler code, runs the algorithm and picks the next thread and context switch, the next thread uses `iret` to restore its previous context.
