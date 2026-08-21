# Operating Systems — Master Study Guide

> **Entry point** for exam preparation. Course: Operating Systems, Reichman University, Spring Semester 2026.
> Lecturer: Tsvi Cherny-Shahar · TAs: Daniel Karalnik, Liam Tal.
> This file maps the full course content across `Lectures-md/` (7 decks, 658 slides), `Recitations-md/` (7 decks, 621 slides), `Solutions-md/` (3 exercise solution sets + the exercise-1 assignment spec) and `Exams-md/` (practice exam with model solutions) — faithful English Markdown conversions of the original course PDFs in `Lectures/`, `Recitations/`, `Exercises/`, `Exercises/Solutions/` and `Practice Exam.pdf`.

---

## How these files work (read this first — humans and LLMs)

- **One Markdown file per deck**, no merges: 7 lectures, 7 recitations, 3 solution sets, 1 practice exam. Names follow the topic, not the original filename.
- **Every deck's content mirrors its source PDF exactly**: same topics, same notation, same worked examples, same code. Nothing was added from outside the slides, and nothing substantive was dropped. If a topic is not in these files, it was not taught.
- **Source-page markers.** `<!-- source pages a-b -->` comments mark which original PDF pages each stretch came from. Use them to jump back to the PDF (e.g. for a diagram you want to see rendered).
- **Figures are italic descriptions.** Diagrams, screenshots and photos are replaced by a one-or-two-sentence *italic* description of what they show and the point they make, including labels and values. There are no image files to load — every file is self-contained text.
- **Animation build-ups are merged.** The original decks export one animation step per page (180 physical pages for lecture 2, 89 for recitation 4). Build-ups were collapsed into a single section; genuine **step-by-step traces** (race conditions, scheduler timelines, producer/consumer runs, page walks) keep every step, because in this course the progression *is* the content.
- **Code is verbatim.** C, x86/x64 assembly and shell were transcribed character-for-character in fenced blocks — register names, hex values and comments included.
- **Math is LaTeX** (`$…$` inline, `$$…$$` display) — address arithmetic, turnaround/waiting times, EAT, RAID parity.
- **Boilerplate is stripped**: slide footers, lecturer/TA names, slide numbers, and the Beamer icon words the PDF text layer emits (`Chevron-right`, `Angle-Right`, `INFO-CIRCLE`, `LIGHTBULB` …) are not content and do not appear.
- Occasional italic `*Note: …*` lines flag an **apparent typo in the original slide** — the transcription follows the slide, the note explains the discrepancy. These are transcription annotations, not course content.

### Merged files

`OS-Lectures-Merged.md` and `OS-Recitations-Merged.md` are plain concatenations of the individual deck files (verified byte-for-byte identical content, with a table of contents added). Load a merged file when you want a whole track in one context window; load individual files when you want one topic. Corresponding merged PDFs are `lectures-merged.pdf`, `recitations-merged.pdf` and `lecs-and-recs-merged.pdf`.

---

## File inventory

| File | Source PDF | Slides | Topic |
|---|---|---|---|
| [OS1_Introduction](Lectures-md/OS1_Introduction.md) | `Presentation 1 - 1 Slide.pdf` | 42 | Computing history, OS concepts, terminology, processes & threads |
| [OS2_Threads_and_Synchronization](Lectures-md/OS2_Threads_and_Synchronization.md) | `Presentation 2 - 1 slide.pdf` | 180 | Stack, PCB/TCB, thread lifecycle, race conditions, locks, primitives |
| [OS3_Kernel_and_User_Mode](Lectures-md/OS3_Kernel_and_User_Mode.md) | `OS Presentation 3 - Kernel and user modes.pdf` | 50 | Kernel/user mode, system calls, signals & SEH, `setjmp`/`longjmp` |
| [OS4_Scheduling](Lectures-md/OS4_Scheduling.md) | `OS Presentation 4 - 1 Slide-2.pdf` | 65 | FCFS, SJF, Round Robin, MLFQ, CFS, RSDL |
| [OS5_Memory](Lectures-md/OS5_Memory.md) | `Presentation 5 - Memory - 1 Slide-2.pdf` | 129 | Physical memory, virtual memory, MMU, paging, TLB, swapping, eviction |
| [OS6_Filesystems](Lectures-md/OS6_Filesystems.md) | `OS 6 - Filesystems - 1 Slide.pdf` | 102 | HDD/SSD, allocation, FAT/NTFS/EXT4, links, FDs, pipes, RAID |
| [OS7_Executables_and_Linking](Lectures-md/OS7_Executables_and_Linking.md) | `Presentation 7 - Executables - 1 Slide-2.pdf` | 90 | ELF, static/dynamic libraries, DLL/SO, relocation, COW, PLT/GOT, PIC |
| [Rec1_Intro_and_the_Modern_OS](Recitations-md/Rec1_Intro_and_the_Modern_OS.md) | `Rec1_OS.pdf` | 64 | Terminology, OS abstractions, threads, **time accounting** |
| [Rec2_Processes_Threads_and_Synchronization](Recitations-md/Rec2_Processes_Threads_and_Synchronization.md) | `Rec2_OS.pdf` | 98 | Lifecycle, `fork`, zombies, critical sections, primitives, deadlock |
| [Rec3_Kernel_User_Mode_Syscalls_and_Signals](Recitations-md/Rec3_Kernel_User_Mode_Syscalls_and_Signals.md) | `Rec3_OS.pdf` | 107 | Traps, syscall routes, signal masks, async-signal-safety, uthreads |
| [Rec4_Scheduling](Recitations-md/Rec4_Scheduling.md) | `Rec4_OS.pdf` | 89 | FCFS/SJF/RR/MLFQ/RSDL worked runs, **exam-style Q&A** |
| [Rec5_Memory_and_Paging](Recitations-md/Rec5_Memory_and_Paging.md) | `Rec5_OS.pdf` | 90 | VM mapping, MMU, paging, **FIFO/Belady/LRU/Clock, thrashing, EAT** |
| [Rec6_Filesystems_and_RAID](Recitations-md/Rec6_Filesystems_and_RAID.md) | `Rec6_OS.pdf` | 113 | FAT/NTFS/inodes, **BIOS/UEFI/MBR/GPT/boot**, FDs, pipes, RAID |
| [Rec7_Virtualization](Recitations-md/Rec7_Virtualization.md) | `Rec7_OS.pdf` | 60 | Hypervisor types, CPU/memory virtualization, containers, cloud |
| [ex1_solutions](Solutions-md/ex1_solutions.md) | `ex1_sol.pdf` | 4 | Concurrency vs parallelism, interleavings, TAS vs ticket lock, prod/cons |
| [ex1_assignment](Solutions-md/ex1_assignment.md) | `ex1-2.pdf` | 15 | **Ex-1 spec**: TQ1–TQ4 questions + **PQ1 semaphore, PQ2 condition variable, PQ3 RW-lock** |
| [ex2_solutions](Solutions-md/ex2_solutions.md) | `ex2_sols.pdf` | 30 | `int 0x80` vs `syscall` cost, signals + mutex, `sigsetjmp`, uthreads spec |
| [ex3_solutions](Solutions-md/ex3_solutions.md) | `ex3_sols.pdf` | 17 | MLFQ variant, demand paging, links/partitions/indirect blocks, `fs.c` spec |
| [Practice-Exam](Exams-md/Practice-Exam.md) | `Practice Exam.pdf` | 7 | Full practice exam **with model solutions and point splits** |

---

## ⚠️ Track asymmetries — do not study one track only

The lecture and recitation tracks run in parallel for weeks 1–6, but they are **not** duplicates, and week 7 diverges completely. These gaps are verified, not assumed:

| Topic | Only in | Note |
|---|---|---|
| **Executables, ELF, linking, PLT/GOT, COW, PIC** | Lecture 7 | No recitation covers this at all. |
| **Virtualization, hypervisors, containers** | Recitation 7 | No lecture covers this — lectures mention "virtualization" only as a word in a list. ⚠️ **Likely NOT exam material** — see the scope note below. |
| **FIFO page replacement, Belady's Anomaly, thrashing, Effective Access Time (EAT)** | Recitation 5 | Zero occurrences in lecture 5. Classic exam material. |
| **BIOS, UEFI, MBR, GPT, the boot process, NTFS journaling** | Recitation 6 | Zero occurrences of BIOS/UEFI/MBR in lecture 6. |
| **Peterson's algorithm, ticketlock (TAS/FAA), memory barriers in depth** | Lecture 2 | Zero occurrences of Peterson/ticketlock in recitation 2. |
| **`fork()` semantics: zombies, `exit()` vs `pthread_exit()`, fork-inside-thread, FDs after `fork()`** | Recitations 2 & 6 | Treated far more deeply than in lectures. |

**Practical consequence:** the union of both tracks is the syllabus. Lecture 7 (executables/linking) is a single-source topic with no backup — study it from the lecture alone.

### 🔻 Scope note: virtualization is probably out

Evidence from the recorded sessions (`Transcripts-md/`), not from the slides:

- **Recitation 12** (the last recorded recitation, which finished the file-systems deck) — the TA on what comes next: *"בשבוע הבא… נדבר על איזשהו נושא בונוס כזה **שלא יהיה לבחינה**… מה שצבי מלמד בהרצאות, הוא מלמד בהרצאות. אני מדבר מבחינת תרגולים."* The following recitation is deck 7 — **virtualization** — so the TA is calling it a bonus topic that is not on the exam.
- **Lecture 14** (the last lecture, on executables) — *"זה כל מה שיהיה למבחן, זהו. לא יהיה עוד."* ("this is everything that will be on the exam, that's it — there won't be more").

**Treat Unit 21 (virtualization) as optional/low-priority.** The transcript evidence is strong but indirect — it is a TA's spoken remark about the following week, and the transcription around it is noisy. Confirm with the course staff before dropping it entirely. Everything else in this guide is exam material.

---

## Course map — topic hierarchy

The course has **7 parts**. Each unit lists its files (L = lecture, R = recitation) and subtopics.

### Part I — Foundations: What an Operating System Is

**Unit 1: History and the role of the OS**
- L: [OS1](Lectures-md/OS1_Introduction.md) — first computers (Bombe, ENIAC); generations: vacuum tubes → transistors → ICs → multi-core/virtualization; batch systems, the GM I/O monitor as the first OS; **SPOOL, multi-programming, time-sharing**; what happens when a computer starts; hardware **interrupts**.
- R: [Rec1](Recitations-md/Rec1_Intro_and_the_Modern_OS.md) — PC structure, the OS as resource manager and abstraction layer.

**Unit 2: Core terminology and time accounting** *(numeric, exam-friendly)*
- L: [OS1](Lectures-md/OS1_Introduction.md) — utilization, overhead, throughput, latency; **concurrency vs. parallelism**; context switch; process address space (code/heap/stack); the thread of the process.
- R: [Rec1](Recitations-md/Rec1_Intro_and_the_Modern_OS.md) — **3 worked exercises**: utilization/overhead/throughput/latency from a time budget; idle CPU time; context-switch cost; **time metrics and formulas** (wait time, turnaround) single- vs multi-threaded, single- vs multi-core.
- HW: [ex1 TQ1](Solutions-md/ex1_solutions.md) — concurrency vs parallelism wall-clock computation with context-switch overhead.

### Part II — Threads, Concurrency and Synchronization *(largest unit — 180 + 98 slides)*

**Unit 3: Process and thread anatomy**
- L: [OS2](Lectures-md/OS2_Threads_and_Synchronization.md) — **the stack** (frames, prologue/epilogue, `rbp`/`rsp`); **PCB / TCB**; context switch mechanics; **thread states**; scheduler & dispatcher; creating processes and threads; what threads share vs. own.
- R: [Rec2](Recitations-md/Rec2_Processes_Threads_and_Synchronization.md) — process structure, thread creation, **join vs detach**, thread vs process termination, **zombie processes**, `exit()` vs `pthread_exit()`, **fork inside a thread**, multiple threads + fork.

**Unit 4: Race conditions and mutual exclusion**
- L: [OS2](Lectures-md/OS2_Threads_and_Synchronization.md) — shared memory, IPC; **atomic operations**; the `i++` step-by-step interleaving trace (full frame-by-frame walkthrough); threads interleaving; **mutual exclusion**, critical section; tryouts 1–3 and their failures (deadlock/livelock, no mutual exclusion); **Peterson's algorithm**; busy wait / spinlock.
- R: [Rec2](Recitations-md/Rec2_Processes_Threads_and_Synchronization.md) — bank-account interleaving, critical sections, `stdatomic.h`, **exercise: safe withdrawal**, the optimization/reordering problem, "does this code work?".

**Unit 5: Locks and synchronization primitives**
- L: [OS2](Lectures-md/OS2_Threads_and_Synchronization.md) — **RMW operators** (TAS, CAS, FAA), atomic increment, C types/functions; **spinlock via TAS**; Burns' starvation-free TAS; **ticketlock** (FIFO spinlock with FAA), FIFO ticketlock without busy-wait; **memory barriers/fences**; **semaphores** (bounded/unbounded), **thread barrier**, **readers-writer lock** (+ upgradable), **condition variables**.
- R: [Rec2](Recitations-md/Rec2_Processes_Threads_and_Synchronization.md) — semaphores, **connection-pool exercise**, read-write locks, **writer starvation**, **lock upgrade**, **deadlock**, condition variables, two-conditions-one-CV, resource pool with cancellation.
- HW: [ex1 TQ2–TQ4](Solutions-md/ex1_solutions.md) — interleavings, **TAS vs ticket lock**, producer/consumer.
- HW: [ex1 assignment spec](Solutions-md/ex1_assignment.md) — the programming part, and the best statement of the *required semantics* of each primitive: **PQ1** unbounded semaphore built on a ticket lock (with the lecture's ticket lock as reference), **PQ2** condition variable, **PQ3** readers–writer lock with a **writer-preference fairness policy**. Read the "semantics"/"required assumptions" sections even if you don't re-implement the code — they state precisely the behaviour the course expects.

**Unit 6: The producer–consumer problem** *(the lecture's capstone worked example)*
- L: [OS2](Lectures-md/OS2_Threads_and_Synchronization.md) — consumers/producers with condition variables on both sides; **the missing synchronization on the shared queue**; full animated trace with an external FIFO `ext_lock` + CV; final implementation.
- R: [Rec2](Recitations-md/Rec2_Processes_Threads_and_Synchronization.md) — classic synchronization problems.

### Part III — Kernel Mode, User Mode and Signals

**Unit 7: Privilege modes and system calls**
- L: [OS3](Lectures-md/OS3_Kernel_and_User_Mode.md) — monolithic OS; **kernel-mode vs user-mode**; system call as the gate to the kernel; **`syscall`/`sysret` (x86-64) vs `int 0x80`/`iret` (x86)**; syscall vs SW interrupt vs exception; user-mode and kernel-mode worked examples (OS/161).
- R: [Rec3](Recitations-md/Rec3_Kernel_User_Mode_Syscalls_and_Signals.md) — **what is a trap**, legacy vs modern syscall routes, **the kernel stack**, syscall examples, **exercise: system-call overhead**, interrupts and exceptions.
- HW: [ex2 TQ1–TQ2](Solutions-md/ex2_solutions.md) — **`int 0x80` vs `syscall` cost calculation**, kernel entry mechanisms and signal delivery.

**Unit 8: Signals and user-space exception handling**
- L: [OS3](Lectures-md/OS3_Kernel_and_User_Mode.md) — **SEH (Windows)**, **signals (Linux)**, `pause`, asynchronous signals, waiting for a signal, **default disposition**, **POSIX delivery priority**.
- R: [Rec3](Recitations-md/Rec3_Kernel_User_Mode_Syscalls_and_Signals.md) — signal delivery flow, **process- vs thread-directed** signals, **signal masks / pending sets** (`original_mask ∪ sa_mask ∪ {S}`, `SA_NODEFER`), sending signals, `sigaction` example, **signal masking as synchronization**, **async-signal-safety**, exercises (logging, sensor monitor), **danger cases**: nested handler, signal to a lock-holding thread, interleaving analysis + the fix.
- HW: [ex2 TQ3](Solutions-md/ex2_solutions.md) — **signal delivery while holding a mutex**.

**Unit 9: User-mode threading**
- L: [OS3](Lectures-md/OS3_Kernel_and_User_Mode.md) — **`setjmp`/`longjmp`**, user-mode threading, when do we context switch.
- R: [Rec3](Recitations-md/Rec3_Kernel_User_Mode_Syscalls_and_Signals.md) — **`sigsetjmp`/`siglongjmp` gotcha**, user-mode threading, exercise: database transaction rollback.
- HW: [ex2 TQ4 + Part B](Solutions-md/ex2_solutions.md) — user-level switching with `sigsetjmp`/`siglongjmp`; **the full `uthreads` library spec** (state diagram, scheduler, virtual vs wall-clock time, quantum counting, blocking/sleeping, error messages, `uthreads.h`).

### Part IV — Scheduling

**Unit 10: CPU scheduling** *(highly numeric — expect a computation question)*
- L: [OS4](Lectures-md/OS4_Scheduling.md) — when/to whom do we context switch; **mechanism vs policy**; **I/O-bound vs CPU-bound**; **preemption and quantum**; offline: **FCFS**, **SJF** (+ estimating burst length as we go); online: **Random**, **Round Robin**, rewarding I/O-bound threads; **multi-level feedback priority queues**; **CFS**; **RSDL** with a full epoch trace.
- R: [Rec4](Recitations-md/Rec4_Scheduling.md) — worked runs of **FCFS / SJF / RR / MLFQ / RSDL**, RSDL full walkthrough, **concluding exam-style Q&A**: general concepts, true/false batteries on scheduling algorithms, **open question (autonomous drone scheduling)**, and a **performance question solved four ways with I/O** (FCFS, RR quantum = 2 ms, MLFQ, modified RSDL).
- HW: [ex3 TQ1](Solutions-md/ex3_solutions.md) — an **MLFQ variant**.

### Part V — Memory and Virtual Memory

**Unit 11: Physical memory and its evolution**
- L: [OS5](Lectures-md/OS5_Memory.md) — physical memory, **types of volatile memory**, MS-DOS-era memory, single program → multitasking, **fragmentation** (internal/external).
- R: [Rec5](Recitations-md/Rec5_Memory_and_Paging.md) — intro to physical memory, the evolution of memory management.

**Unit 12: Virtual memory and address translation**
- L: [OS5](Lectures-md/OS5_Memory.md) — **virtual memory**; types (**anonymous vs file-backed**); accessibility and **access control**; `pmap`; virtual→physical; **MMU**; mechanism vs policy; **base and bound**; **segmentation**; **CAM**; **TLB**.
- R: [Rec5](Recitations-md/Rec5_Memory_and_Paging.md) — virtual→physical mapping, the MMU.

**Unit 13: Paging, page tables and the TLB**
- L: [OS5](Lectures-md/OS5_Memory.md) — **pages**; translation on **TLB hit** / **TLB fault**; page table as an array; the **page-table walk and page fault**; TLB entry removal; page-table data structures; **multi-level page tables**; levels of memory cache; **don't allocate on `malloc`**.
- R: [Rec5](Recitations-md/Rec5_Memory_and_Paging.md) — paging.

**Unit 14: Swapping and page replacement** *(recitation is the primary source)*
- L: [OS5](Lectures-md/OS5_Memory.md) — **swap in/out**; which page to evict; **LRU**; **Clock (approximate LRU)**; **working-set** cache.
- R: [Rec5](Recitations-md/Rec5_Memory_and_Paging.md) — swap space, **demand paging**, **FIFO replacement**, **Belady's Anomaly**, LRU (linked-list implementation), **Clock**, **working set model**, **thrashing**, **Effective Access Time (EAT)**, exercises + summary. ⚠️ *FIFO, Belady, thrashing and EAT appear nowhere in the lecture.*
- HW: [ex3 TQ2](Solutions-md/ex3_solutions.md) — demand paging and page replacement.

### Part VI — File Systems and Storage

**Unit 15: Storage hardware**
- L: [OS6](Lectures-md/OS6_Filesystems.md) — **HDD** geometry, **NAND/NOR flash**, **SSD**; sector vs block vs cluster.
- R: [Rec6](Recitations-md/Rec6_Filesystems_and_RAID.md) — file-system hardware.

**Unit 16: File allocation and directories**
- L: [OS6](Lectures-md/OS6_Filesystems.md) — **contiguous**, **linked-list**, **FAT**, **NTFS (MFT)**, **inode** allocation; partitions, **FORMAT**, **MOUNT**; locating a file under FAT / NTFS / EXT4; directories.
- R: [Rec6](Recitations-md/Rec6_Filesystems_and_RAID.md) — the FS abstraction layer, linked-list allocation, **FAT**, **defragmentation**, **NTFS intro + journaling**, **I/O in NTFS exercise**, **single- and multi-level inode systems**, **EXT**, **inode exercise**.
- HW: [ex3 TQ3 + Part B](Solutions-md/ex3_solutions.md) — links, partitions and **indirect blocks**; the full `fs.c` implementation spec (disk layout, API, helpers, `fs.h`, pitfalls).

**Unit 17: Boot, partitioning and firmware** ⚠️ *recitation-only*
- R: [Rec6](Recitations-md/Rec6_Filesystems_and_RAID.md) — motherboard/ROM/firmware, **BIOS**, **UEFI** (and how it solves BIOS's limits), **the boot process**, **MBR**, disk partitions, **GPT**.

**Unit 18: Links, file descriptors and pipes**
- L: [OS6](Lectures-md/OS6_Filesystems.md) — **hard vs symbolic linking**, file attributes, **file descriptor / HANDLE**, common Unix file operations, **named pipes**.
- R: [Rec6](Recitations-md/Rec6_Filesystems_and_RAID.md) — linking, file descriptors, **exercise: FDs after `fork()`**, pipes.

**Unit 19: RAID**
- L: [OS6](Lectures-md/OS6_Filesystems.md) — **RAID 0 / 1 / 10 / 4 / 5 / 6**; **XOR parity recovery**; **RAID 6 weighted-XOR recovery**; other FS features.
- R: [Rec6](Recitations-md/Rec6_Filesystems_and_RAID.md) — RAID.

### Part VII — Executables, Linking and Virtualization

**Unit 20: Executables and linking** ⚠️ *lecture-only — no recitation backup*
- L: [OS7](Lectures-md/OS7_Executables_and_Linking.md) — hello world without `main()`; Linux assembly; **linker undefined reference**; **ELF file structure**; **what happens when we run an executable, in 10 steps**; function prologue/epilogue; **ELF header hex analysis**; why executables aren't portable across OSes (WINE); **static vs dynamic libraries**; loading a `.so` in 10 steps; **Windows DLL** (`LoadLibrary` search order, export table, `GetProcAddress`, import library, **PE IAT**); **relocation**; **copy-on-write (COW)** with three worked examples; **lazy vs eager binding**; library entry points; **DLL Hell**; ***nix SO**: symbol resolution, **PLT**, **GOT**, first-call lazy initialization, `dlopen` flags, global symbol search, dynamic symbol table, **visibility**, `DT_INIT`/`DT_FINI`, **PIC**.

**Unit 21: Virtualization** 🔻 *recitation-only — and per the TA, a bonus topic **not on the exam** (see the scope note above). Study last, or skip if short on time.*
- R: [Rec7](Recitations-md/Rec7_Virtualization.md) — a brief history of virtualization; what virtualization is; **hypervisor types (1 and 2)**; **CPU virtualization** and approaches to it; **the virtualization spectrum**; **memory virtualization**; **containers vs virtual machines**; virtualization & security; **the cloud abstraction ladder**.

---

## Dependency graph (what builds on what)

```
Terminology & time accounting (U2) ──► used by EVERY numeric question (scheduling, EAT, RAID)

Thread anatomy / stack / PCB (U3) ──► Race conditions (U4) ──► Locks & primitives (U5) ──► Producer-Consumer (U6)
        │                                     │                          │
        │                                     └──────────────────────────┴──► HW1
        │
        └──► Kernel/user mode & syscalls (U7) ──► Signals (U8) ──► User-mode threading (U9) ──► HW2 (uthreads)
        │                                            │
        └──► Scheduling (U10) ◄───────────────────────┘ (quantum = timer interrupt = signal)

Physical memory (U11) ──► Virtual memory & MMU (U12) ──► Paging & TLB (U13) ──► Swapping & replacement (U14)
                                    │
                                    └──► COW (U20) — same mechanism, different use

Storage hardware (U15) ──► Allocation & directories (U16) ──► Links/FDs/pipes (U18) ──► HW3 (fs.c)
        │                            │
        │                            └──► Boot/MBR/GPT (U17)
        └──► RAID (U19)

Executables & linking (U20) — needs the stack (U3), virtual memory (U12) and COW
Virtualization (U21) — needs kernel/user mode (U7) and memory translation (U12/U13)
```

---

## Exam format

From [Practice-Exam](Exams-md/Practice-Exam.md) — 3 hours, 100 points:

| Part | Format | Count | Points | Rule |
|---|---|---|---|---|
| **A** | True / False | 10 | 3 each = 30 | Answer all |
| **B** | Multiple choice | 4 | 5 each = 20 | Answer all |
| **C** | Open questions | 3 | 25 each = 50 | **Answer any 2 of 3** |

- **All storage units are base-2**: 1 KB = 1024 B, 1 MB = 1024 KB, 1 GB = 1024 MB (i.e. KiB/MiB/GiB).
- Open questions are graded per sub-part — show your work; the model solutions include the **point split per sub-part**.
- The practice exam's open questions were: **C1** thread interleaving under a lock, **C2** copy-on-write with threads and `fork()`, **C3** MLFQ vs RSDL. Note these span three different parts of the course — Part II, Parts V+VII, and Part IV.
- Grading overall: 3 exercises = 40% (13.3% each), exam = 60% (must score ≥ 60 to pass).

---

## Learning stages (suggested study plan)

Each stage ends with a **checkpoint** — if you can do it from memory, move on. The recitation exercises and the HW solutions are the best self-test; try solving them before reading the solution.

### Stage 0 — Orientation (1 hour)
Read this file, then skim [OS1](Lectures-md/OS1_Introduction.md) and [Rec1](Recitations-md/Rec1_Intro_and_the_Modern_OS.md). Goal: know what exists and how the parts connect.

### Stage 1 — Foundations & time accounting (½ day) · Units 1–2
Files: [OS1](Lectures-md/OS1_Introduction.md) + [Rec1](Recitations-md/Rec1_Intro_and_the_Modern_OS.md), then [ex1 TQ1](Solutions-md/ex1_solutions.md).
**Checkpoint:** define utilization vs usage vs overhead vs throughput vs latency and compute all five from a time budget; explain concurrency vs parallelism; compute wall-clock time for *n* threads on *k* cores with a per-switch cost.

### Stage 2 — Threads & synchronization (2 days — the biggest unit) · Units 3–6
Files: [OS2](Lectures-md/OS2_Threads_and_Synchronization.md) + [Rec2](Recitations-md/Rec2_Processes_Threads_and_Synchronization.md) + [ex1 solutions](Solutions-md/ex1_solutions.md).
Also read the [ex1 assignment spec](Solutions-md/ex1_assignment.md) for the exact required semantics of the semaphore, condition variable and writer-preference RW-lock.
**Checkpoint:** trace `i++` interleaving on two threads and enumerate the possible final values; state what threads share vs own; explain why tryouts 1–3 fail and how Peterson's fixes it; implement a spinlock with TAS and a ticketlock with FAA, and say why the ticketlock is starvation-free; explain why a memory barrier is needed; write the producer/consumer solution with a mutex + condition variable and say what breaks without the queue lock; explain writer starvation and lock upgrade deadlock.

### Stage 3 — Kernel/user mode & signals (1–1.5 days) · Units 7–9
Files: [OS3](Lectures-md/OS3_Kernel_and_User_Mode.md) + [Rec3](Recitations-md/Rec3_Kernel_User_Mode_Syscalls_and_Signals.md) + [ex2 solutions](Solutions-md/ex2_solutions.md).
**Checkpoint:** walk a syscall from user code to kernel and back for both `int 0x80` and `syscall`, and compute the cost difference; distinguish trap / interrupt / exception; compute the handler mask from `original_mask`, `sa_mask` and `SA_NODEFER`; explain why calling `malloc` in a signal handler is unsafe; explain the deadlock when a signal arrives while a lock is held, and the fix; describe how `sigsetjmp`/`siglongjmp` implement user-mode context switching.

### Stage 4 — Scheduling (1 day) · Unit 10 *(most computation-heavy)*
Files: [OS4](Lectures-md/OS4_Scheduling.md) + [Rec4](Recitations-md/Rec4_Scheduling.md) + [ex3 TQ1](Solutions-md/ex3_solutions.md).
**Checkpoint:** given arrival and burst times, produce the Gantt chart and compute average waiting and turnaround for FCFS, SJF, RR (a given quantum), MLFQ and RSDL — **including I/O bursts**; explain mechanism vs policy; explain why RR with a quantum larger than every burst degenerates to FCFS; explain how MLFQ and RSDL each prevent starvation, and how they differ.

### Stage 5 — Memory (1.5 days) · Units 11–14
Files: [OS5](Lectures-md/OS5_Memory.md) + [Rec5](Recitations-md/Rec5_Memory_and_Paging.md) + [ex3 TQ2](Solutions-md/ex3_solutions.md).
**Checkpoint:** translate a virtual address through a multi-level page table by hand; explain TLB hit / TLB miss / page fault and what the MMU vs the OS does in each; run **FIFO, LRU and Clock** on a reference string and count faults; **demonstrate Belady's Anomaly**; compute **EAT** given hit ratio and access/fault costs; explain thrashing and the working-set model; contrast internal and external fragmentation.

### Stage 6 — File systems & storage (1.5 days) · Units 15–19
Files: [OS6](Lectures-md/OS6_Filesystems.md) + [Rec6](Recitations-md/Rec6_Filesystems_and_RAID.md) + [ex3 solutions](Solutions-md/ex3_solutions.md).
**Checkpoint:** compare contiguous / linked / FAT / inode allocation on access pattern and fragmentation; compute the **maximum file size** for an inode with N direct + single + double + triple indirect blocks; trace locating a file in FAT, NTFS and EXT4; explain hard vs symbolic links and what happens on delete; state FD behaviour after `fork()`; for RAID 0/1/10/4/5/6 give usable capacity, fault tolerance and read/write cost, and **recover a lost block with XOR parity**; outline the boot process and contrast MBR with GPT and BIOS with UEFI.

### Stage 7 — Executables (1 day) · Unit 20 *(single-source topic)*
Files: [OS7](Lectures-md/OS7_Executables_and_Linking.md). Unit 21 / [Rec7](Recitations-md/Rec7_Virtualization.md) is 🔻 probably not on the exam — do it only if you have spare time.
**Checkpoint:** list the 10 steps of running an executable and of loading a `.so`; explain static vs dynamic linking trade-offs; explain **PLT + GOT** on a first call vs later calls and why that is lazy binding; explain **relocation and COW** and how a DLL is shared between processes; explain **PIC** and why it matters; contrast **type-1 vs type-2 hypervisors** and **containers vs VMs**.

### Stage 8 — Exam simulation (last day)
- Take the [Practice Exam](Exams-md/Practice-Exam.md) **closed-book, 3 hours**, answering only 2 of the 3 open questions, before reading the model solutions.
- Re-solve [Rec4's concluding Q&A](Recitations-md/Rec4_Scheduling.md) (true/false batteries + the drone open question + the four-algorithm performance question) — it is the most exam-shaped material in the course.
- Re-read all three [HW solution sets](Solutions-md/) — the theory questions (TQ) are written in exam style.
- Re-check the ⚠️ single-source topics above; they have no second explanation to fall back on.

---

## Notes for LLMs assisting with these files

1. **Stay inside the course scope.** Use only the terminology, mechanisms and algorithms that appear in these files. This course teaches specific variants (e.g. **RSDL** alongside CFS; **Burns' starvation-free TAS**; the **`ext_lock` + condition variable** formulation of producer/consumer). Do not substitute a textbook's version, and do not introduce topics the course never taught.
2. **Follow the course's own notation and framing**: kernel-mode vs user-mode; trap / interrupt / exception as three distinct things; virtual→physical translation as mechanism vs policy; `handler_mask = original_mask ∪ sa_mask ∪ {S}`; quantum-based preemption; base-2 storage units (1 KB = 1024 B) — **always**, since the exam states this explicitly.
3. **Prefer computation to prose when quizzing.** This course examines by hand-computation: Gantt charts and average waiting/turnaround times, page-fault counts and EAT, address translation, inode maximum file size, RAID capacity and XOR recovery, interleaving enumeration. Model questions on [Rec4's concluding section](Recitations-md/Rec4_Scheduling.md), the HW theory questions, and the [Practice Exam](Exams-md/Practice-Exam.md).
4. **Respect the track asymmetries.** Before saying "the course didn't cover X", check both tracks — the ⚠️ table above lists the topics that exist in only one of them. Lecture 7 (executables/linking) and Recitation 7 (virtualization) have no counterpart at all.
5. **File lookup.** One file per deck: `Lectures-md/OS<n>_<topic>.md`, `Recitations-md/Rec<n>_<topic>.md`, `Solutions-md/ex<n>_solutions.md` (plus `Solutions-md/ex1_assignment.md` for exercise 1's programming spec, which is *not* part of `ex1_sol.pdf`), `Exams-md/Practice-Exam.md`. Whole-track single files: `OS-Lectures-Merged.md`, `OS-Recitations-Merged.md`. Original PDFs: `Lectures/`, `Recitations/`, `Exercises/Solutions/`, `Practice Exam.pdf`; merged PDFs at the repo root.
6. **`<!-- source pages a-b -->` comments** map back to the original PDF pages — cite them when the user should look at the real diagram.
7. **`*Figure: …*` lines are descriptions of slide graphics**, not course prose. If a user needs the actual visual (a page-table walk, an RSDL epoch chart, a RAID layout), point them at the source page rather than trying to redraw it from the description.
8. **Italic `*Note: …*` lines are transcription annotations** flagging apparent typos in the original slides — they are not course content.
9. The student's working code for the three exercises lives in `Exercises/ex1/`, `ex2/` (the `uthreads` library) and `ex3/` (`fs.c`) — useful when a question is about implementation rather than theory.
