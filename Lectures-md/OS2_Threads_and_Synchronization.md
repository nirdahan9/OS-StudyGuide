# Threads, Concurrency and Synchronization

> Study notes converted from the lecture slide deck `Presentation 2 - 1 slide.pdf` (180 pages) — Operating Systems, Reichman University, Spring Semester 2026.
> Lecturer: Tsvi Cherny-Shahar · TAs: Daniel Karalnik, Liam Tal.
> Faithful conversion: every substantive item on the source pages appears here, and nothing has been added from outside them. Figures are rendered as italic *Figure:* descriptions.
> `<!-- source pages a-b -->` comments mark the original page ranges.

---

<!-- source pages 1-16 -->

## Title Slide

*Figure: The opening slide is styled as a BIOS setup utility screen, with the deck title in the title bar and a menu bar of Main / Advanced / Security / Boot / Exit.*

```
        Operating Systems - Presentation 2 - Multi-threading & Synchronization
Main       Advanced      Security     Boot      Exit

 Bios Version: 4S4EB8X0.86F              | Item Specific Help
 Service tag: XXXXX                      |
                                         | Use the keys in the footer to
 System Time                [11:31:11]   | navigate.
 System Date          [Thu 8/26/2024]    |
 Primary IDE Master  : [Not Detected]    |
 Primary IDE Slave   : [Not Detected]    |
 SATA1               : [HL-DT-ST DVDRW GH]
 SATA2               : [SAMSUNG HD103SJ]
 SATA3               : [Not Detected]
 SATA4               : [Not Detected]
 Storage Configuration
 System Information

<--> Select Menu    ^v Select Item    Enter Select / Sub-Menu    F9 Save and Exit
```

---

## Acknowledgments

Thanks to:

- Prof. Anat Bremler-Barr
- Prof. Gadi Taubenfeld
- Dr. Guy Katz and Prof. David Hay

Also, resources from:

- OS/161 Instructional Operating System (os161.org)
- ops-class.org

---

## The Stack

The stack is just a chunk of memory.

To know where it is in the memory, the address of the **top of the stack** is stored in the register **Stack Pointer** (**SP**):

- **ESP** – 32bit memory address (32bit OS)
- **RSP** – 64bit memory address (64bit OS)

The address of the base is stored in the **Base Pointer** (BP):

- **EBP** – 32bit memory address
- **RBP** – 64bit memory address

(**RIP** – 64bit Instruction Pointer)

### C (and not only C) — Local variables

Local variables are stored on the stack and freed automatically.

`thestack.c`:

```c
void f()
{
    short x = 1;
    int y = 2;
    int z = x + y;
}
```

Compiled disassembly (rows whose address/bytes columns are covered by the slide's annotation boxes are shown with the instruction column only):

```asm
0x00007ff6e7631802    55                     push   rbp
                                             mov    rbp,rsp
0x00007ff6e7631806    48 83 ec 10            sub    rsp,0x10
                                             mov    WORD PTR [rbp-0x2],0x1
0x00007ff6e7631810    c7 45 f8 02 00 00 00   mov    DWORD PTR [rbp-0x8],0x2
                                             movsx  edx,WORD PTR [rbp-0x2]
0x00007ff6e763181b    8b 45 f8               mov    eax,DWORD PTR [rbp-0x8]
                                             add    eax,edx
0x00007ff6e7631820    89 45 f4               mov    DWORD PTR [rbp-0xc],eax
0x00007ff6e7631823    90                     nop
                                             add    rsp,0x10
0x00007ff6e7631828    5d                     pop    rbp
0x00007ff6e7631829    c3                     ret
```

Annotations on the slide, pointing at the instructions above:

- `sub rsp,0x10` — Allocate size of short+int+int+pad
- `mov WORD PTR [rbp-0x2],0x1` — Store 1 into short "x"
- `mov DWORD PTR [rbp-0x8],0x2` — Store 2 into int "y"
- The group `movsx edx,…` / `mov eax,…` / `add eax,edx` / `mov DWORD PTR [rbp-0xc],eax` — z = x + y
- `add rsp,0x10` — Free the allocated stack

*Figure: the stack frame is drawn as a column of blocks with their sizes in bytes, from the top of the drawing downwards — pad (4), z (4), y (4), pad (2), x (2) — sitting above the rest of the Stack, which fades away below. Padding is for optimization.*

- `* alloca()` allocates memory on the stack (i.e. `sub rsp`)
- `*` Stack allocation/free is very cheap

### C (and not only C) — function

`PUSH REG` – write REG register to the stack:

```asm
SUB RSP, 0x8
MOV [RSP-0x8], REG
```

`POP REG` – the opposite of PUSH:

```asm
MOV REG, [RSP-0x8]
ADD RSP, 0x8
```

`JMP ADDR` – set instruction pointer RIP to ADDR:

```asm
MOV RIP, ADDR
```

`CALL ADDR` – push next instruction to STACK and JMP ADDR:

```asm
PUSH RIP
JMP ADDR
```

`RET` – the opposite of CALL:

```asm
POP RIP
```

### C (and not only C) — function – Prologue & Epilogue

```asm
; Prologue
PUSH RBP        ; Save the old base pointer
MOV RBP, RSP    ; Set the new base pointer

; Function body
...
; Call another function
CALL 0xABCDEF
...

; Epilogue
POP RBP         ; Restore the old base pointer
RET             ; Return to the caller
```

### C (and not only C) — call stack

Use stack to call and return from function.

`thestack.c`:

```c
void k(){ printf("k\n"); }

void h(){ k(); }

void g(){ h(); }
```

Disassembly (of the function that calls `k`):

```asm
0x00007ff6c7b317ee    55    push  rbp
0x00007ff6c7b317ef          mov   rbp,rsp
0x00007ff6c7b317f2          sub   rsp,0x20
0x00007ff6c7b317f6          call  0x7ff6c7b317d0 <k>
0x00007ff6c7b317fb          nop
0x00007ff6c7b317fc          add   rsp,0x20
0x00007ff6c7b31800    5d    pop   rbp
0x00007ff6c7b31801    c3    ret
```

Annotations on the slide: an orange bracket spanning `push rbp` + `mov rbp,rsp` is labelled **Prologue**; `call 0x7ff6c7b317d0 <k>` is labelled **k()**; an orange bracket spanning `pop rbp` + `ret` is labelled **Epilogue**. `sub rsp,0x20` and `add rsp,0x20` fall outside both brackets.

*Figure: a debugger CALL STACK window shows frame [1] "PAUSED ON BREAKPOINT" with the frames `k()` (thestack.c 3:1), `h()` (thestack.c 5:1, highlighted), `g()` (thestack.c 7:1), `main()` (thestack.c 18:1); frame [2] "PAUSED" shows `ntdll.dll!ntdll!ZwWaitForWorkViaWorkerFactory`, `ntdll.dll!ntdll!RtlClearThreadWorkOnBehalfTicket`, `kernel32.dll!KERNEL32!BaseThreadInitThunk`, `ntdll.dll!ntdll!RtlUserThreadStart`, `[Unknown/Just-In-Time compiled code]`.*

*Figure: alongside it, the Stack is drawn as stacked blocks — `k()` on top, then `h()`, then `g()` — above the fading remainder of the stack, matching the debugger's call stack.*

**Notice – we ignore parameters/return value.**

### multi-threaded — where is the thread's stack?

If every thread executes different code, it should have its own set of set of Stack and registers:

- Thread1: `f(){ g(); }`
- Thread2: `k(){ h(); }`

**So, where is the stack of the thread?**

One solution is the OS allocates chunks of memory from the Heap assigns it to the thread as Stack.

**What is the size of the Stack?** There's a default, but you can set it yourself.

*Figure: the process address space is drawn as a vertical bar from 0x0 at the top to 0xFF… at the bottom — Code, Data, Heap (fading downward), then a first Stack band, empty space, a second Stack band, and OS Metadata at the bottom — showing two per-thread stacks carved out of the heap region.*

*Figure: Process Explorer screenshots of POWERPNT.EXE:84644 Properties, Threads tab (Count: 78) listing TIDs with CPU, Cycles Delta and Start Address (e.g. 44264 / 1,044,720 / mso20win32client.dll+0x21d330; 45160 / 733,158 / aitrx.dll!ArtificialIntelligence_Generative_Vision_State_Destroy_+0x457f0), with the "Stack" button circled; the selected thread shows Thread ID 56392, Start Time 15:18:55 24/09/2024, State Wait:WrAlertByThread, Base Priority 8, Dynamic Priority 8, Kernel Time 0:00:00.000, User Time 0:00:00.000, I/O Priority Normal, Context Switches 1, Memory Priority 5, Cycles 1,257,196, Ideal Processor 5. Three "Stack for thread …" windows are open on top of each other — thread 44264 (frame 0 `ntdll.dll!NtWaitForSingleObject+0x14`, 1 `KERNELBASE.dll!WaitForSingleObjectEx+0x8e`, 2 `mso20win32client.dll+0x24400c`, …), thread 27016 (0 `win32u.dll!NtUserMsgWaitForMultipleObjectsEx+0x14`, 1 `mso20win32client.dll+0x301dc8`, 2 `ppcore.dll!PPMain+0x31792`, 3 `ppcore.dll+0x2f6ead`, …), and thread 14744 (0 `ntdll.dll!NtRemoveIoCompletion+0x14`, 1 `KERNELBASE.dll!GetQueuedCompletionStatus+0x53`, 2 `GROOVEEX.DLL+0xaffd7`, 3 `GROOVEEX.DLL+0xb0564`, 4 `KERNEL32.DLL!BaseThreadInitThunk+0x1d`, 5 `ntdll.dll!RtlUserThreadStart+0x28`) — each thread really does have its own stack.*

---

## Process Control Block (PCB) / Tread Control Block (TCB)

Information the OS needs about the process.

**PCB** fields:

| Field | Note on the slide |
|---|---|
| Process ID | |
| State | Running/suspended… |
| (Base) Priority | for scheduler |
| Pointers to Heap, Code & Data | |
| Heap Information | |
| Threads | |
| User | User running (permissions) |
| Environment | **Copy** of environment variables |
| Open Files | |
| Threads Local Storage (TLS) Info | |
| Exit Code | |
| … | |

**TCB** fields (each entry of the PCB's *Threads* list points to one):

| Field | Note on the slide |
|---|---|
| Thread ID | |
| **Registers (i.e. context)** | CPU registers saved for *context switch* |
| Parent Process ID | |
| (Dynamic) Priority | |
| Pointer to TLS | Each thread has a "private" storage |
| … | |

*Figure: a green arrow runs from the PCB's "Threads" row to the TCB table, showing that the PCB's thread list expands into per-thread control blocks.*

*Figure: the Process block (Code, Data, Heap) and the stacked Thread blocks (each ending in "Stack (BP & SP)") are drawn on the left, with arrows pointing into the address-space bar on the right (0x0 at the top: Code, Data, Heap, Stack, Stack, Stack, OS Metadata, down to 0xFF…) — the process fields point to Code/Data/Heap and each thread's Stack (BP & SP) points to a separate Stack region.*

---

## Context Switch — switching threads/processes

*Figure: a timeline. Thread $i$ (blue) runs until an Interrupt (lightning bolt) strikes; a red block follows, labelled "Thread that handles to interrupts (created by the OS)"; then Thread $j$ (green) runs. Time flows left to right.*

Inside the red interrupt-handling block:

- (scheduler chooses $j$ – ignore for now)
- $TCB_i.\text{context} = \text{CPU regs}$
- $\text{CPU regs} = TCB_j.\text{context}$

Context Switch is an overhead, as no job is running during the switch.

*Figure: a Process Explorer (Sysinternals, TC-PC\tcs) screenshot. The upper pane lists processes — POWERPNT.EXE (Private Bytes 695,540 K, Working Set 474,160 K, PID 84644, Microsoft PowerPoint), ai.exe, javaw.exe, Acrobat.exe, AcroCEF.exe — with POWERPNT.EXE selected; the lower Threads pane lists that one process's threads (Start Addresses `POWERPNT.EXE+0x1a10`, `mso20win32client.dll+0x2…`, `aitrx.dll!ArtificialIntelligenc…`, `mso40uiwin32client.dll+0x…`, `igd10um64xe.DLL!OpenA…`, `ucrtbase.dll!recalloc+0x10`) and its "Context Switches" column is circled in red, showing per-thread counts 45,332,242; 13,134,354; 5,628,507; 4,160,683; 801,122; 336,217. Status bar: CPU Usage: 1.47%, Commit Charge: 83.52%, Processes: 511, Physical Usage: 70.05%.*

---

## Thread States

*Figure: a state machine with five states — New, Ready, Running, Terminated, Wait — and the labelled transitions listed below.*

| From | To | Transition label |
|---|---|---|
| New | Ready | admit |
| Ready | Running | scheduler picks a thread |
| Running | Ready | Switching to another thread |
| Running | Terminated | exit |
| Running | Wait | Wait event (I/O, block…) |
| Wait | Ready | Event signaled |

Callouts on the diagram:

- When main thread is terminated…
  - **Windows:** process **is** terminated
  - **Linux:** process **is not** terminated
- When a process is terminated, the OS (mostly) cleans up all associated entities (threads, open files, memory)
- If a different process "uses" a terminated process, the terminated process is not completely deleted, but enters a ***zombie*** state, until the other process stops its usage.

---

## Scheduler & Dispatcher — multi-threaded

**CPU Scheduler:**
Schedules the threads that should run next, and for how long

**Dispatcher:**

- Dispatches the threads - context switch
- Switch to "user mode" from "kernel mode"

we'll get to that later…

---

<!-- source pages 17-26 -->

## Create Process

### Windows — `CreateProcess`

In Windows, `CreateProcess` creates a new process from a given new executable.

*Figure: Two boxes — the parent process `createprocess.exe` with an arrow down to the child process `cmd.exe` — showing that the created process runs a different executable than its parent.*

```c
void create_process()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(
        "C:\\Windows\\System32\\cmd.exe", // binary name
        "/C echo 'hello world'", // command line arguments
        NULL, // set process security attributes
        NULL, // set thread security attributes
        FALSE, // inherit handles (like, open files)
        0, // creation flags like PRIORITY, Show console and such...
        NULL, // environment variables. NULL – same as parent
        NULL, // current directory of the process. NULL – same as parent
        &si, // returned startup information
        &pi // returned created process information
    ))
    {
        fprintf(stderr, "CreateProcess failed (%d)\n", GetLastError());
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
}
```

*(file: `createprocess.c`, compiled to `createprocess.exe`)*

```shell
> gcc createprocess.c -o createprocess
> createprocess.exe
'hello world'
```

### Linux — `fork()` and `execl()`

In Linux, `fork()` creates a "**copy**" of the calling process!
`execl()` replaces the existing process with a **completely** new one!

`fork()` the **memory**, the **file handles** and **the thread calling fork()**. Therefore, RIP (next line to run) is identical!

Fork() might be "dangerous" like the infamous **fork bomb**:
`while(TRUE){fork();}`

```c
void create_process()
{
    pid_t forkret = fork();

    if (forkret == -1)
    {
        fprintf(stderr, "fork failed\n");
        return;
    }

    if (forkret == 0) // child process
    {
        printf("I am the child process. my PID is %d\n", getpid());

        execl("/bin/sh", "sh", "-c", "echo Hello World", NULL);

        perror("execl failed");  // <- This line is executed only if execl fails!!!
        exit(1);
    }
    else
    {
        printf("I am the parent process. my PID is %d\n", getpid());

        int status;
        waitpid(forkret, &status, 0);
    }
}
```

*(file: `createprocess.c`, compiled to `createprocess`)*

Output:

```
I am the parent process. my PID is 2441
I am the child process. my PID is 2445
Hello World
```

### fork / execl in pictures

*Figure: Top row — a process icon labelled "PID $x$ running 'myprog' at address ADDR" with a `fork` arrow to a second identical process icon labelled "PID $y$ running 'myprog' at address ADDR"; annotation: $y$ is a child of $x$. Bottom row — the process icon labelled "PID $y$ running 'myprog'" is crossed out with a big red X, and an `execl` arrow leads to a green process icon labelled "PID $y$ running 'otherprog'"; annotation: $y$ is still a child of $x$.*

---

## Create Thread

### Windows — `CreateThread`

In Windows, `CreateThread` creates a new thread that starts at `thread_function()`.
In the 4<sup>th</sup> parameter we can pass a pointer to arguments the thread receives.
Once the thread exits "`thread_function()`", it terminates.

`WaitForSingleObject()` stops the current thread until the target thread terminates.

```c
void thread_function(void* data)
{
    printf("I am the thread function. my thread ID is %d\n", GetCurrentThreadId());
}

void create_thread()
{
    printf("main thread id: %d\n", GetCurrentThreadId());

    DWORD thread_id;
    HANDLE hThread = CreateThread(NULL, // default security attributes
                        0, // use default stack size
                        (LPTHREAD_START_ROUTINE)thread_function, // function*
                        NULL, // argument to thread function
                        0, // use default creation flags
                        &thread_id); // returns the thread identifier
    if (hThread == NULL)
    {
        fprintf(stderr, "CreateThread failed with error %d\n", GetLastError());
        return;
    }

    WaitForSingleObject(hThread, INFINITE); // wait for thread to exit

    CloseHandle(hThread); // close the handle to the thread
}
```

Output:

```
main thread id: 32028
I am the thread function. my thread ID is 50152
```

### Linux — `pthread_create`

In Linux, `pthread_create` creates a new thread that starts at `thread_function()`.
In the 4<sup>th</sup> parameter we can pass a pointer to arguments the thread receives.
Once the thread exits "`thread_function()`", it terminates.

`pthread_join()` stops the current thread until the target thread terminates.

```c
void* thread_function(void* data)
{
    printf("I am the thread function. my thread ID is %ld\n", pthread_self());
    return NULL;
}

void create_thread()
{
    printf("main thread id: %ld\n", pthread_self());

    pthread_t thread;
    int ret = pthread_create(&thread, // pointer to thread descriptor
                        NULL, // use default attributes
                        thread_function, // thread function entry point
                        NULL); // parameters to pass to the thread function
    if (ret != 0)
    {
        fprintf(stderr, "pthread_create failed with error %d\n", ret);
        return;
    }

    pthread_join(thread, NULL); // wait for thread to exit, 2nd param gets the ret val
}
```

Output:

```
main thread id: 140737351563072
I am the thread function. my thread ID is 140737351558720
```

---

## Multi-Threading

Single-threaded case: one thread $\vec{t_1}$ must both **draw the window** and **save file to disk**.

- While saving, the thread cannot draw the window (i.e. not responding)

Multi-threaded case:

- $\vec{t_1}$ — draw the window; updates saving the progress in the User Interface
- $\vec{t_2}$ — save file to disk; write progress to variable

*Figure: Top — a single thread $\vec{t_1}$ branching to the two tasks "Draw the window" and "Save file to disk". Bottom — the two tasks split across $\vec{t_1}$ and $\vec{t_2}$. To the right, a screenshot of "Eula.txt - Notepad (Not Responding)" with the dialog "Notepad is not responding — If you close the program, you might lose information", offering "Close the program" and "Wait for the program to respond".*

---

## Distributed Algorithms & Systems

The usage of *threads* and *processes*, on a *single* or *multiple* computers, **work together** toward a common goal.

The fields of **distributed algorithms** and **distributed systems** are **at the heart** of today:

- large-scale applications
- cloud systems and applications
- multi-core CPU/GPU architectures (used for AI)
- More…

Studied in-depth, at the **Distributed Algorithms course by Prof. Gadi Taubenfeld**

---

## Intra-Process Communication / Inter-Process Communication (IPC)

*(Title emphasizes the contrast: **Intra**-Process Communication vs **Inter**-Process Communication (IPC).)*

### Shared Memory Model

*Figure: Three threads $\vec{t_i}$, $\vec{t_j}$, $\vec{t_k}$ each connected by a double-headed arrow labelled "R/W" to a single shared memory box — all three read and write the same memory.*

### Message Passing Model

*Figure: Thread $\vec{t_i}$ "send"s a message box `m` along an arrow to thread $\vec{t_j}$, which "receive"s it.*

Only IPC model in computer network

---

## Shared Memory

**All threads of P share P's memory.**

*Figure: Three process boxes $P_1$, $P_2$, $P_3$, each holding an address-space column with the segments Code, Data, Heap, (memory mapped), Stack, OS data. In $P_1$: threads $\vec{t_1}$ and $\vec{t_2}$ point into $P_1$'s Heap, while $\vec{t_3}$ points across into the "memory mapped" segment of $P_2$; $\vec{t_4}$ (in $P_2$) also points into that same "memory mapped" segment. Above the diagram is a "storage" (floppy-disk) icon, reached by exactly two arrows: one from $\vec{t_5}$ (in $P_2$) and one from $\vec{t_6}$ (in $P_3$). $P_3$'s "memory mapped" segment has no incoming arrow, and $P_1$ has no memory-mapped segment.*

---

## Atomic Operation

**Atomic operation** takes **one** CPU cycle.

Context switch can happen at any time, instructions that take more than 1 CPU cycle (i.e. **not atomic**) might get "split" while another thread is running.

`x++;` → `x = x+1;` →

```asm
MOV EAX, [0x123]
ADD EAX, 0x1
MOV [0x123], EAX
```

*(the address `0x123` is annotated as holding `x`)*

Notation convention used from here on:

- To simplify, instead of MOV, to load a word we'll use **LW**, to store a word we'll use **SW**
- To refer a register we'll use r1, r2, …

```asm
LW  r1, x
ADD r2, r1, 1
SW  x, r2
```

**NOTICE – Not every** ASSEMBLY OPCODE **is atomic** (e.g. PUSH, POP, CALL and more…)

---

<!-- source pages 27-41 -->

## Example: two threads incrementing a global — `multithread.c`

```c
void f()
{
    x = 0; // x is global
    pthread_t thread1;
    pthread_t thread2;
    int ret1 = pthread_create(&thread1, NULL, g, NULL);
    int ret2 = pthread_create(&thread2, NULL, g, NULL);

    if (ret1 != 0 || ret2 != 0)
    {
        fprintf(stderr, "failed with error ret1 %d ; ret2 %d\n", ret1, ret2);
        return;
    }

    pthread_join(thread1, NULL); // wait for thread to exit
    pthread_join(thread2, NULL); // wait for thread to exit

    // x should be 2000000
    const char* result = x == 2000000 ? "HURRAY! x = %d\n" : "OH NO! x = %d\n";
    printf(result, x);
}
```

```c
void* g(void*)
{
    for(int i = 0; i < 1000000; i++)
    {
        x++;
    }
    return NULL;
}
```

**What is the output?**

---

## `i` → 5 ; `i++` ; step by step

A step-by-step trace of two threads running the same loop body, with the loop bound reduced to 5 (`For i → 5`) instead of 1000000. The shared variable starts at `X = 0`, and the loop body `x++` is compiled to the three instructions:

```asm
LW  r1, x
ADD r2, r1, 1
SW  x, r2
```

*Figure: each frame shows the shared value `X` at the top, the two threads' loop counters `i` side by side (left counter = orange-arrow thread, right counter = blue-arrow thread), an orange arrow and a blue arrow marking each thread's position in the three-instruction body, and two register boxes on the right — a blue-outlined box (the blue-arrow thread's `r1`/`r2`) above a yellow-outlined box (the orange-arrow thread's `r1`/`r2`).*

### The trace, frame by frame

| Frame | Orange ▶ at | Blue ▶ at | `i` (orange) | `i` (blue) | `X` | Blue box `r1`,`r2` | Yellow box `r1`,`r2` |
|---|---|---|---|---|---|---|---|
| 1 | `LW` | `LW` | 0 | 0 | 0 | `?`, `?` | `?`, `?` |
| 2 | `LW` | `ADD` | 0 | 0 | 0 | `0`, `?` | `?`, `?` |
| 3 | `LW` | `SW` | 0 | 0 | 0 | `0`, `1` | `?`, `?` |
| 4 | `ADD` | `SW` | 0 | 0 | 0 | `0`, `1` | `0`, `?` |
| 5 | `ADD` | past `SW` (end of body) | 0 | 0 | 1 | `0`, `1` | `0`, `?` |
| 6 | `ADD` | `LW` | 0 | 1 | 1 | `1`, `1` | `0`, `?` |
| 7 | `ADD` | `ADD` | 0 | 1 | 1 | `1`, `1` | `0`, `?` |
| 8 | `ADD` | `SW` | 0 | 1 | 1 | `1`, `2` | `0`, `?` |
| 9 | `ADD` | past `SW` (end of body) | 0 | 1 | 2 | `1`, `2` | `0`, `?` |
| 10 | `ADD` | `LW` | 0 | 2 | 2 | `1`, `2` | `0`, `?` |
| 11 | `ADD` | `LW` (with loop-back arc) | 0 | 3 | 3 | `2`, `3` | `0`, `?` |
| 12 | `ADD` | `LW` (with loop-back arc) | 0 | 4 | 4 | `3`, `4` | `0`, `?` |
| 13 | `SW` | `LW` | 0 | 4 | 4 | `3`, `4` | `0`, `1` |
| 14 | past `SW` (end of body) | `LW` | 0 | 4 | **1** | `3`, `4` | `0`, `1` |

Reading of the trace:

- Frames 1–5: the blue thread runs the whole body once while the orange thread is still stuck at `LW`/`ADD`. Blue loads `x = 0` into `r1`, computes `r2 = 1`, stores it — `X` becomes 1.
- Frame 4: the orange thread performs its `LW`, reading `x = 0` into its `r1`. `X` is still 0 at this point — the blue thread's `SW` only takes effect on frame 5 — so the value read is the current one, and it becomes stale only afterwards. That `0` stays in the orange thread's `r1` for the rest of the trace.
- Frames 6–12: the blue thread keeps looping on its own, driving `X` up to 4 (`i` reaches 4, its `r1 = 3`, `r2 = 4`). Frames 11–12 draw a loop-back arc around the three instructions to indicate the repeated iterations.
- Frame 13: the orange thread finally executes `ADD r2, r1, 1` with its stale `r1 = 0`, producing `r2 = 1`.
- Frame 14: the orange thread executes `SW x, r2` and writes `1` back to the shared variable — **`X` drops from 4 back to 1**.

*Note: on frame 6 (page 33) the blue box's `r1` already changes to `1` in the same frame in which the blue counter reaches `i=1` and the blue pointer returns to `LW`; on frame 10 (page 37) the blue counter reaches `i=2` with the pointer back at `LW` but the blue box is unchanged (`r1 = 1`, `r2 = 2`), and the corresponding update to `r1 = 2`, `r2 = 3` appears only on frame 11 (page 38). Transcribed as it appears on the slides.*

---

<!-- source pages 42-56 -->

## `i→5 ; i++ ; step by step` (continued)

The trace continues with the same two threads running the same loop body, `For i → 5`:

```asm
LW  r1, x
ADD r2, r1, 1
SW  x, r2
```

*Figure: two coloured arrows (orange and blue) act as the two threads' program counters, each pointing at the instruction it is about to execute; two register boxes are shown on the right — the blue-framed box holds the blue thread's `r1`/`r2`, the yellow-framed box holds the orange thread's `r1`/`r2`. Above the code, `i=…  i=…` gives each thread's loop counter (orange on the left, blue on the right). The shared variable is shown as `X = …` at the top.*

The progression, frame by frame:

| Frame | Just executed | `X` | orange `i` | orange `r1`,`r2` | blue `i` | blue `r1`,`r2` | Arrow positions |
|---|---|---|---|---|---|---|---|
| p42 | orange looped back to the top of the body | 1 | 1 | 0, 1 | 4 | 3, 4 | both arrows at `LW r1, x` |
| p43 | blue: `LW r1, x` (reads `X = 1`) | 1 | 1 | 0, 1 | 4 | **1**, 4 | orange at `LW`, blue at `ADD` |
| p44 | orange: `LW`, `ADD`, `SW` — a whole iteration, then loops back | **2** | 2 | 1, 2 | 4 | 1, 4 | orange back at `LW` (loop arc), blue still at `ADD` |
| p45 | orange: another full iteration | **3** | 3 | 2, 3 | 4 | 1, 4 | orange back at `LW` (loop arc), blue still at `ADD` |
| p46 | orange: another full iteration | **4** | 4 | 3, 4 | 4 | 1, 4 | orange back at `LW` (loop arc), blue still at `ADD` |
| p47 | orange: its last full iteration; the orange thread is finished and its arrow disappears | **5** | 5 | 4, 5 | 4 | 1, 4 | only the blue arrow remains, at `ADD` |
| p48 | blue: `ADD r2, r1, 1` → `r2 = 1 + 1` | 5 | 5 | ?, ? | 4 | 1, **2** | blue at `SW x, r2` |
| p49 | blue: `SW x, r2` — writes `2` over the `5` | **2** | 5 | ?, ? | 4 | 1, 2 | blue arrow past the end of the body |
| p50 | final state, boxed for emphasis | **`X = 2`** | 5 | ?, ? | 4 | 1, 2 | — |

---

## Threads Interleaving

Unless we explicitly synchronize threads, they can:

- Run in **any order**
- Start/Stop at **any time**
- Run/Stop for **arbitrary amount of time**

- **Advanced -** threads can crash & CPU can change order of instruction (for optimization)

Studied in-depth, at the **Distributed Algorithms course**

---

## Mutual Exclusion

In *Solution of a problem in concurrent programming control* (1965), **Edsger W. Dijkstra**, defined the problem we've just seen and the need for *Mutual Exclusion between the processes/threads*.

*Figure: a black-and-white portrait photograph of Edsger W. Dijkstra accompanies the slide.*

- ***Mutual Exclusion*** – No more than one thread/process can be concurrently in a *critical section*
- ***Critical Section*** – Code that is executed with mutual exclusion to prevent concurrent access

---

## Critical Section

*Figure: a flow diagram of the four regions a process cycles through — `remainder code` (grey, with a solid self-loop back to itself) → `entry code` (blue, with a dashed self-loop, i.e. it may spin there) → `critical section` (red) → `exit code` (blue) → and an arc back up to `remainder code`.*

- **Mutual Exclusion:** No two processes are in their critical section at the same time.
- **Deadlock-freedom:** If a process is trying to enter its critical section, then some process, not necessarily the same one, eventually enters its critical section.
- **Starvation-freedom:** If a process is trying to enter its critical section, then this process must eventually enter its critical section.
- **Livelock-freedom:** If processes are trying to enter their critical sections, then eventually progress is made. processes do not remain active without making progress.

\* For the rest of **this presentation** – "thread"/"process" may be used interchangeably

### Where is the critical section?

```c
void g(void*)
{
    for(int i = 0; i < 1000000; i++)
    {
        x++;
    }
}
```

*Figure: `x++;` is underlined in red with an arrow labelled "← The critical section".*

---

## Critical Section – tryout 1

Shared variable: `turn=0`

$t_0$:

```c
while(turn == 1){skip}

Critical Section

turn=1
```

$t_1$:

```c
while(turn == 0){skip}

Critical Section

turn=0
```

*Figure: each thread's code sits in a box with a large curved arrow on its left showing the whole block repeating in a loop. A callout points at the `{skip}` in $t_1$'s while-loop: "Or `yield`".*

### Mutual exclusion?

Assume $t_0$ and $t_1$ are in the CS.

If $t_0$ is in CS (assuming it got first w.l.o.g) → `turn == 1` is false → **`turn=0`** (as `turn` is binary) when $t_0$ got into the CS

If $t_1$ is in CS (assuming it got second w.l.o.g) → `turn == 0` is false → **`turn=1`** when $t_1$ got into the CS → $\boldsymbol{t_0}$ **executed `"turn=1"`** after the CS, but it **contradicts** the assumption $t_0$ is still in the CS.

### Starvation?

$t_1$ got into *entry code* →

$t_0$ doing other stuff…

*Figure: a four-panel photo of a man waiting in a field — standing, checking his watch, sitting, then lying flat — illustrating $t_1$ waiting forever.*

---

<!-- source pages 57-72 -->

## Busy Wait / Spinlock / Wait

```c
while(condition)
{
    skip
}
```

- **Busy Wait** – repeatedly *polls* for a condition until it is met
- **Spinlock** – implementation of busy wait – let the CPU spin until condition is met
  - *Example:* the condition is met within 2 CPU cycle – better to spin than context switch
- **Wait** – change thread state to "sleep/block" (not scheduled for CPU) until someone else *interrupts* and wakes the thread from its slumber

---

## Critical Section – tryout 2

Shared initial state: `flag={F,F}`

```c
// t0                    // t1
flag[0] = T              flag[1] = T
while(flag[1])           while(flag[0])
{skip}                   {skip}

Critical Section         Critical Section

flag[0] = F              flag[1] = F
```

*Figure: two side-by-side thread boxes, $t_0$ on the left and $t_1$ on the right, with a shared `flag` state box above them; a coloured arrow beside each box marks that thread's current instruction as the trace advances.*

### Interleaving trace

| Step | `flag` | $t_0$ is at | $t_1$ is at |
|---|---|---|---|
| 1 | `{F,F}` | `flag[0] = T` | `flag[1] = T` |
| 2 | `{T,F}` | `while(flag[1])` | `flag[1] = T` |
| 3 | `{T,T}` | `while(flag[1])` | `while(flag[0])` |
| 4 | `{T,T}` | `{skip}` | `while(flag[0])` |
| 5 | `{T,T}` | `{skip}` | `{skip}` |

Result: **Deadlock** (actually… livelock)

*Figure: a photo of a completely gridlocked traffic intersection covers the code, captioned in red "Deadlock (actually… livelock)".*

---

## Critical Section – tryout 3

Shared initial state: `flag={F,F}`

```c
// t0                    // t1
while(flag[1])           while(flag[0])
{skip}                   {skip}
flag[0] = T              flag[1] = T

Critical Section         Critical Section

flag[0] = F              flag[1] = F
```

### Interleaving trace

| Step | `flag` | $t_0$ is at | $t_1$ is at |
|---|---|---|---|
| 1 | `{F,F}` | (before start) | (before start) |
| 2 | `{F,F}` | `while(flag[1])` | `while(flag[0])` |
| 3 | `{F,F}` | `flag[0] = T` | `while(flag[0])` |
| 4 | `{F,F}` | `flag[0] = T` | `flag[1] = T` |
| 5 | `{T,F}` | `Critical Section` | `flag[1] = T` |
| 6 | `{T,T}` | `Critical Section` | `Critical Section` |

Result: **No mutual exclusion**

*Figure: a photo of two cars that have crashed head-on in an intersection covers the code, captioned in red "No mutual exclusion".*

---

## Critical Section – (Gary Lynn) Peterson's algorithm (1981)

Shared initial state: `flag={F,F} ; turn=0`

```c
// t0                              // t1
flag[0] = T                        flag[1] = T
turn=1                             turn=0
while(flag[1] and turn==1)         while(flag[0] and turn==0)
{skip}                             {skip}

Critical Section                   Critical Section

flag[0] = F                        flag[1] = F
```

### Proof of mutual exclusion

Assume $t_0$ and $t_1$ are in the CS.

$t_0$ is in CS (assuming it got first w.l.o.g) $\rightarrow$ `turn == 0` *or* `flag[1]==F` when entering CS

$t_1$ is entering CS (assuming it got second w.l.o.g) before $t_0$ got out $\rightarrow$ **`turn == 1` or `flag[0]==F`**

- **if `turn==0`** $\rightarrow$ `flag[0]` must be false $\rightarrow$ $t_0$ got out of CS $\rightarrow$ **contradicts** that $t_0$ is still in the CS
- **if `flag[0]==T`** $\rightarrow$ `turn` must be 1 $\rightarrow$ $t_0$ executed "turn=1" and got into the CS before $t_1$ $\rightarrow$ `flag[1] == F` *or* `turn == 0`
  - `flag[1] == F` $\rightarrow$ **contradicts** the assumption that $t_1$ is in the CS
  - `turn == 0` $\rightarrow$ **contradicts** the assumptions that `turn` must be 1

Original algorithm for 2 threads, can be easily generalized for $n$ threads

---

<!-- source pages 73-88 -->

## Read-Modify-Write (RMW) operators

Modern "advanced" CPUs supports **hardware RMW atomic operations**.
Using these operators, we can **easily implement a Critical Section.**

BUT, In many cases **we prefer** simpler CPUs, ones that might still lack these operators:

- Prioritize **lower power consumption** over anything else
- A **large number of simple cores** rather small and sophisticated CPUs
- Embedded systems that prioritize **lower cost over performance**
  (I have never seen an ad for a fridge proud that they are using RMW CPU)

*Figure: A picture of a fridge with an ad panel reading "RMW operators", captioned "Expensive fridge due to RMW operators (image from MS copilot)".*

### Three RMW operators pseudocode

```c
bool test-and-set(bool* val) // TAS
{
    bool prev = *val;
    *val = true;
    return prev;
}
```

```c
int fetch-and-add(int* val, int inc) // FAA
{
    *val = *val + inc;
    return *val;
}
```

```c
bool compare-and-swap(int* val, int cmpr, int new_val) // CAS
{
    if(*val != cmpr){ return false; }

    *val = new_val;
    return true;
}
```

---

## Atomic Increment

```c
void inc(int* val, int to_add)
{
    return fetch-and-add(val, to_add);
}
```

If CPU doesn't support FAA, but does support CAS, you can implement with a busy-wait:

```c
void inc(int* val, int to_add)
{
    done = false;
    while(!done){
      int prev = *val;
      // returns true if *val and prev didn't change
      done = compare-and-swap(val, prev, prev+to_add);
    }
}
```

---

## Related C types and functions

`stdatomic.h` – atomic library
`sched.h` – scheduler interface

`atomic_flag` data type – unsigned char (boolean) used in atomic library

`volatile [variable]` keyword (in C):

- Compiler does not apply optimizations of the store/load of the variable
- Always read from the memory

---

## Implementing Spinlock (TAS)

```c
void spinlock_init(atomic_flag* lock)
{
    atomic_flag_clear(lock);
}

void spinlock_acquire(atomic_flag* lock)
{
    while(atomic_flag_test_and_set(lock))
    {
        // scheduler - yield
        sched_yield();
    }
}

void spinlock_release(atomic_flag* lock)
{
    atomic_flag_clear(lock);
}
```

*Figure: An arrow points at `atomic_flag_test_and_set(lock)` with the annotation "Returns false if lock is FALSE and sets lock to TRUE".*

*Note: on the later repeat of this same code the comment is written as `// scheduler – yield (i.e. skip)`.*

```c
void thread_func(void* arg)
{
    atomic_flag* lock = (atomic_flag*)arg;
    spinlock_acquire(lock); // critical section

    printf("Thread %lu in CS\n",
                pthread_self());

    spinlock_release(lock);
}
```

```c
int main()
{
#define N 2

    atomic_flag lock;
    spinlock_init(&lock);

    pthread_t threads[N];

    for(int i=0 ; i<N ; i++){
        pthread_create(&threads[i], NULL,
                        thread_func, &lock);
    }

    for(int i=0 ; i<N ; i++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}
```

Source file: `spinlock_tas.c`

### Properties of the TAS spinlock

- Mutual Exclusion? Yes
- Deadlock Free? Yes
- Starvation Free? **No**
  t1 can repeatedly get into CS while t0 doesn't "make it"

A Queue can fix this!
(only a queue?!)

---

## Alan Burns TAS starvation-free

```
waiting[i] := true
key := true      // local variable
while (waiting[i] and key)
    {key = test-and-set(lock)}
waiting[i] := false
critical section
j = (i+1) mod n
while (j ≠ i) and (not waiting[j])
{j := (j+1) mod n}
if j==i
        then lock = false
        else waiting[j] := false
```

The code is bracketed into labelled phases:

| Lines | Phase |
|---|---|
| `waiting[i] := true` / `key := true` | Announce |
| `while (waiting[i] and key) {key = test-and-set(lock)}` | Try to get lock |
| `waiting[i] := false` / `critical section` | Get into CS |
| `while (j ≠ i) and (not waiting[j]) {j := (j+1) mod n}` | Find next thread waiting |
| `then lock = false` | No one is waiting, release the lock |
| `else waiting[j] := false` | Let j in |

*Figure: State diagram beside the code — a `waiting` array with indices 0, 1, 2, 3, … , n-1 all holding `false`, a single `lock` variable holding `false`, and a separate (grey, uninitialized) `key` box.*

---

## Ticketlock — FIFO Spinlock with FAA

Use an integer to represent the queue.
When $t_i$ wants to gets into the CS (entry code):

- Takes a number (atomically)
- Wait for its turn (atomically)

When it leaves (exit code):

- Move the turn to the next in line

*Figure: A "PLEASE Take A Number" ticket dispenser next to a "NOW SERVING 532" display — the deli-counter analogy for the ticket lock.*

```c
typedef struct
{
    atomic_int ticket;
    atomic_int cur_ticket;
}ticket_lock;

void ticketlock_init(ticket_lock* lock)
{
    atomic_init(&lock->ticket, 0);
    atomic_init(&lock->cur_ticket, 0);
}
```

```c
void ticketlock_acquire(ticket_lock* lock)
{
    // get my ticket
    int my_ticket = atomic_fetch_add(&lock->ticket, 1);

    // wait until it is my turn
    while(atomic_load(&lock->cur_ticket) != my_ticket)
    {
        sched_yield();
    }
}

void ticketlock_release(ticket_lock* lock)
{
    atomic_fetch_add(&lock->cur_ticket, 1);
}
```

Source file: `ticketlock.c`

### Ticketlock usage example

Global variable:

```c
x = 0;
```

```c
void thread_func(void* arg)
{
    ticket_lock* lock = (ticket_lock*)arg;
    ticketlock_acquire(lock); // critical section

    x++;
    printf("Thread %lu - %d\n", pthread_self(), x);

    ticketlock_release(lock);
}
```

```c
int main()
{
#define N 100

    ticket_lock lock;
    ticketlock_init(&lock);

    pthread_t threads[N];

    for(int i=0 ; i<N ; i++){
        pthread_create(&threads[i],
                NULL,
                thread_func,
                &lock);
    }

    for(int i=0 ; i<N ; i++){
        pthread_join(threads[i], NULL);
    }

    printf("Final x = %d\n", x);

    return 0;
}
```

Source file: `ticketlock.c`

---

## FIFO Ticketlock without Busy-wait

The threads in the previous Ticketlock ***polls*** indefinitely until it's their turn.
Another approach is to ***suspend*** the thread and ***interrupt*** (wake) it when its turn arrives.

Threads state can be controlled:

- In windows:
  - `SuspendThread()` and `ResumeThread()`
- In Linux:
  - No "official" mechanism
  - Can be implemented using signals (we will get into that!)
  - `sigwait()` – suspend the thread
  - `pthread_kill()` – wakes up the thread (yes, I know the naming is not good…!)

### FIFO Ticketlock without Busy-wait (mutex)

```c
sigset_t set;
#define MAX_THREADS 256
typedef struct
{
    // the lock
    atomic_flag stick;

    // ticket for FIFO
    atomic_uchar ticket;
    atomic_uchar cur_ticket;

    // array of the sleeping threads
    pthread_t* waiting_threads;
    int max_threads;
}tlint;
```

```c
void tlint_init(tlint* lock)
{
    // initialize the atomic flag
    atomic_flag_clear(&lock->stick);

    // initialize the ticket and cur_ticket
    atomic_init(&lock->ticket, 0);
    atomic_init(&lock->cur_ticket, 0);

    // initialize the array that holds
    // the waiting threads
    lock->waiting_threads = calloc(MAX_THREADS,
                                    sizeof(pthread_t));
    lock->max_threads = MAX_THREADS;
}
```

The `tlint_acquire` function is built up over three slides; the complete function is:

```c
void tlint_acquire(tlint* lock)
{
    // get a ticket
    int my_ticket = atomic_fetch_add(&lock->ticket, 1);

    // set to "my_ticket" the thread ID
    lock->waiting_threads[my_ticket] = pthread_self();

    // test and set the atomic flag, if it is already set, wait
    char is_my_turn_and_lock = 0;

    while(!is_my_turn_and_lock)
    {
        // try to get the "lock"
        while(atomic_flag_test_and_set(&lock->stick))
        { // didn't get the lock
            // is it my turn?
            if(atomic_load(&lock->cur_ticket) == my_ticket)
            {
                // its my turn! Retry to get the lock!
                sched_yield();
                continue;
            }

            // don't have the lock, not my turn – go to sleep...
            int sig; //"set" - signal to wait on (e.g. SIGUSR1)
            sigwait(&set, &sig);
        }

        // I got the lock !!!

        // make sure it is my turn
        if(atomic_load(&lock->cur_ticket) != my_ticket)
        {
            atomic_flag_clear(&lock->stick);
            sched_yield();
        }
        else
        {
            is_my_turn_and_lock = 1;
        }
    } // while(!is_my_turn_and_lock)

    // current thread has got lock
    lock->waiting_threads[my_ticket] = 0;
}
```

*Figure: A red-outlined callout on the last slide of the build-up shows the missing piece of the release side:*

```c
pthread_kill(next_thread, SIGUSR1);
```

To wake the thread in "release"

---

<!-- source pages 89-101 -->

## Memory Barrier (Memory Fence)

Modern CPUs employ performance optimization, one them is **out-of-order execution**.

**The CPU**, in some circumstances can reorder some operations to improve performance.

In Multi-threaded application the optimization might cause synchronization issues, as the CPU is **not aware** of multi-threading.

The problem gets worse in **multi-core** environments where the **cache** is distributed.

**Memory barriers are CPU instructions** that tells the CPU **not to re-order** instructions

### Example: reordering breaks a hand-off

Two threads, written as:

```c
while(done != 0){skip}
assert(x==42)
```

```c
x = 42;
done = 1; 
```

The CPU **might** execute the right-hand thread as:

```c
done = 1; 
x = 42;
```

*Figure: a red arrow labelled "CPU might execute" points from the original writer code down to the reordered version, in which `done = 1;` is committed before `x = 42;`.*

Fixed with fences:

```c
while(done != 0){skip}
mm_lfence
assert(x==42)
```

```c
x = 42;
mm_sfence
done = 1; 
```

*Figure: a callout box "Commit all store/load operations up to this point" points at both the `mm_lfence` in the reader and the `mm_sfence` in the writer.*

### The three barrier instructions

- `mm_lfence` – load barrier. commit all **load** (read) operations up to this point
- `mm_sfence` – store barrier. commit all **store** (write) operations up to this point
- `mm_mfence` – full barrier. commit all **load and store** operations up to this point

C/C++ `volatile` **does not** place memory barrier

Java/C# `volatile` **does** place full memory barrier to ensure expected semantic of execution order

---

## Synchronization Primitives

*Synchronization primitives* are mechanisms provided by a platform (OS, runtime framework) to achieve ***required synchronization*** between concurrent executions.

Some considered *low level*: Atomics, spinlock, memory barriers more…

Others considered *higher level* as they usually use lower-level primitives internally:

- **Semaphore/Mutex** – Critical section of $1 \rightarrow N$ threads
- **Condition Variables** – Wait on a condition until its signal
- **Reader(s)/Writer(s)** – Allow multiple/single threads performing read/write operations

---

## Semaphore

Semaphore concept invented by Dijkstra (1963)

- When a new "thread" needs a resource, *it tries* (**P**roperen) to get it or **wait** for it.
- When a resource becomes available, it *increments* (**V**erhogen) the available resources or **signals** that another resource is available
- Initialize with the number of available resources
- "1" available resource is a mutex

### Worked illustration: three workstations and a reception desk

*Figure: three desks each with a computer represent the available resources; a reception desk holds the semaphore counter, shown on a red digital display. Arriving people (emoji faces) call `wait()`/`P()` at the reception desk to take a seat, and calling `signal()`/`V()` when leaving a desk frees it.*

The counter is traced step by step:

| Step | Action | Counter | State |
|---|---|---|---|
| 1 | Initial state — three free desks | `Count = 3` | all three desks empty |
| 2 | A thread arrives at the reception desk and calls **wait()** or P() | `3` | all three desks still empty |
| 3 | It is admitted and sits at the first desk; a second thread arrives and calls **wait()** or P() | `2` | desk 1 occupied |
| 4 | The second thread sits at the middle desk; a third thread arrives and calls **wait()** or P() | `1` | desks 1 and 2 occupied |
| 5 | The third thread sits at the right desk; a fourth thread arrives and calls **wait()** or P() | `0` | all three desks occupied |
| 6 | No resource is free, so the fourth thread blocks at the reception desk. The thread at the middle desk finishes and calls **signal()** or V() | `1` | desks 1 and 3 occupied, middle desk released |
| 7 | The waiting thread is admitted and takes the freed middle desk | `0` | all three desks occupied again, nobody waiting |

---

<!-- source pages 102-117 -->

## Semaphore — API, bounded and unbounded

```c
void semaphore_init(semaphore* sem, int initial_value);

// decrement the semaphore value
void semaphore_wait(semaphore* sem);

// increment the semaphore value
void semaphore_signal(semaphore* sem);
```

Usage — the critical section may be entered by up to `N` threads at once:

```c
semaphore sem;
semaphore_init(&sem, N);

void test_func(void* arg)
{
    semaphore* sem = (semaphore*)arg;
    semaphore_wait(sem);

    Critical Section up to N threads

    semaphore_signal(sem);
}
```

- **Bounded Semaphore**: *counter* bounded to **N**!
- **Unbounded Semaphore**: *counter* is **not** bounded! (i.e. we can init with 0 and increment with *signal*)

Implementation… HW

---

## Thread Barrier

**Thread barrier** stops $N$ threads until all of them reach the barrier.

```c
void thread_barrier_init(thread_barrier *barrier,
                         lock* shared_lock_to_use,
                         int thread_to_wait);

void thread_barrier_wait(thread_barrier *barrier);
```

```c
thread_barrier_init(&barrier, &lock, 3);
```

```c
thread_barrier_wait(&barrier);
```

### Animated trace — three threads reaching the barrier

The barrier is initialized for 3 threads, and each thread runs the same code block:

```c
thread_barrier_init(&barrier, &lock, 3);
```

```c
... code ...

thread_barrier_wait(&barrier);
```

*Figure: three orange arrows to the left of the code box represent the three threads; their vertical position shows which line of the block each thread is currently at, and they advance frame by frame.*

The progression, one frame per slide:

1. Two threads are still at the `... code ...` line; the third has moved down and to the right, on its way toward `thread_barrier_wait`.
2. The third thread now sits exactly at the `thread_barrier_wait(&barrier);` line — it is blocked there.
3. A second thread has also dropped to the `thread_barrier_wait` line (further left, still approaching); one thread remains up at `... code ...`.
4. All three arrows are lined up at the `thread_barrier_wait(&barrier);` line — all $N = 3$ threads have reached the barrier.
5. A single thread — the rightmost arrow — has advanced past the wait line: its arrow now sits below the code box, while the other two arrows still sit at `thread_barrier_wait(&barrier);`.

---

## Readers-Writer Lock

Assume **shared** resource $M$ (data structure, file, shared variable, database etc.)

We want a lock that prioritizes and limits certain operations over others using $M$.

- Some operations (usually **read**) can be executed concurrently by many threads - **Readers**
- Some operations (usually **write**) must not run concurrently by many threads – **Writers**

### Animated trace — readers and a writer on $M$

*Figure: a document icon labelled $M$ with smiley-face threads on the left issuing operations toward it.*

1. Three reader threads all have arrows labelled **Read** pointing into $M$ simultaneously; a fourth thread sits idle above them.
2. A separate thread appears to the right of $M$, labelled **Wants to write** — it cannot proceed while the readers hold $M$.
3. The three readers have finished and disappeared; only the idle thread and the waiting writer remain, with no arrows into $M$.
4. The writer now has a **Write** arrow pointing into $M$ — it holds $M$ exclusively, and no readers are present.
5. The writer is gone; the remaining thread now has a **Read** arrow into $M$.

---

## Reader(s)-Writer(s) Lock

There are variants for *many* readers and *many* writers….

("reader" and "writer" does not have to be actual read/write, but simply a distinction between the groups)

*Figure: two side-by-side pictures of the same resource $M$. Left — three reader threads each with a **Read** arrow into $M$ while three writer threads wait idle beside it. Right — the mirror situation: three writer threads each with a **Write** arrow into $M$ while three reader threads wait idle.*

### Readers-Writer Lock API

```c
void rwlock_init(rwlock* lock);
void rwlock_acquire_read(rwlock* lock);
void rwlock_release_read(rwlock* lock);
void rwlock_acquire_write(rwlock* lock);
void rwlock_release_write(rwlock* lock);
```

```c
rwlock rw;
rwlock_init(&rw);
```

```c
rwlock_acquire_write(&rw);
Writer section
rwlock_release_write(&rw);
```

```c
rwlock_acquire_read(&rw);
Readers section
rwlock_release_read(&rw);
```

Implementation of *multiple* readers and *single* writer - HW

---

## Readers-Writer Upgradable Lock

Atomically switch R/W modes.

No "standard" interface/naming for upgrade/downgrade.

```c
void rwlock_acquire_upgrade_reader(rwlock* lock);
void rwlock_acquire_downgrade_writer(rwlock* lock);
```

Downgrade — start as writer, continue as reader:

```c
rwlock_acquire_write(&rw);
Writer section

rwlock_acquire_downgrade_writer(&rw);
Readers section

rwlock_release_reader(&rw);
```

Upgrade — start as reader, switch to writer only if needed:

```c
rwlock_acquire_read(&rw);
Readers section

if(not synced condition){
   rwlock_acquire_upgrade_reader(&rw);
   Writer section
   rwlock_release_write(&rw);
}
else{
   rwlock_release_read(&rw);
}
```

---

<!-- source pages 118-130 -->

## Condition Variable

### The motivation: instead of polling

*Figure: several angry threads repeatedly querying a shared resource (a document icon), each with a speech bubble stacked five deep saying "Is it ready?" — the caption reads "Instead of polling…".*

Instead of polling…

### Wait: interrupt when ready

Instead of asking again and again, a thread calls `wait()` and is woken when the condition holds.

*Figure: left — threads pointing at the resource with `wait()` labelled above them, still emitting "Is it ready?" bubbles, captioned "Interrupt when ready"; right — the same threads under `wait()` are now asleep (sleeping emoji) next to the resource, no longer asking.*

- `wait()`
- Interrupt when ready
- `wait()`

### Signal vs. broadcast

*Figure: left — a group of sleeping threads that called `wait()`; one thread below sends `signal()` (a single arrow to one sleeper), with the bubble "Wake up – its ready", and only that one thread wakes. Right — the same group under `wait()`; one thread sends `broadcast()` (arrows to all of them), with the bubble "Hey everyone! Wake up – its ready", and all of them wake.*

- `signal()` — "Wake up – its ready" (one waiter wakes).
- `broadcast()` — "Hey everyone! Wake up – its ready" (all waiters wake).

### Condition variable API

```c
void condition_variable_init(condition_variable* cv);
void condition_variable_wait(condition_variable* cv, ticket_lock* ext_lock);
void condition_variable_signal(condition_variable* cv);
void condition_variable_broadcast(condition_variable* cv);
```

The `ticket_lock* ext_lock` parameter of `condition_variable_wait` is circled on the slide, with the callout:

> Why "wait" requires an external lock?
> Let's look at the consumers-producers problem…

Usage:

```c
ticket_lock lock
ticketlock_init(&lock);

condition_variable cv;
condition_variable_init(&cv);

condition_variable_wait(cv, lock);
```

```c
// tell/wake one waiting thread COND has met
condition_variable_signal(&cv);
```

```c
// tell/wake all waiting thread COND has met
condition_variable_broadcast(&cv);
```

Implementation - HW

---

## Consumers-Producers

The scenario has three columns: **Producers** on the left, a bounded **Queue** of 7 slots in the middle, and **Consumers** on the right. The slides animate one full cycle of the system, step by step.

*Figure: the standing layout — a vertical 7-cell queue between the "Producers" group on the left and the "Consumers" group on the right; a magenta arrow runs from the producers into the top of the queue, and a blue arrow runs from the top of the queue out to the consumers.*

| Step | Producers | Queue | Consumers |
|---|---|---|---|
| 1 | none active | empty | asleep, thought bubble: "Waiting for items in the queue" |
| 2 | a large group of producers appears and starts producing (magenta arrow producers → queue) | still empty | still asleep, "Waiting for items in the queue" |
| 3 | now asleep | full — all 7 slots hold an item | awake and working (blue arrow queue → consumers); bubble still shows "Waiting for items in the queue" |
| 4 | one producer wakes up, the rest stay asleep | 6 items (bottom slot freed) | working |
| 5 | all producers asleep again | full again — 7 items | working |
| 6 | no producers shown | 7 items, being drained (blue arrow queue → consumers) | working |
| 7 | no producers shown | 4 items left | working |
| 8 | no producers shown | empty | still awake but idle, thought bubble: "Waiting for items in the queue" |
| 9 | no producers shown | empty | asleep again, thought bubble: "Waiting for items in the queue" |

The cycle is: consumers sleep on an empty queue → producers fill it → consumers wake and drain it while producers sleep on a full queue → the queue empties → consumers go back to sleep.

---

<!-- source pages 131-140 -->

## Consumers-Producers — condition variables on both sides

The scenario is extended with a **condition variable on each side of the queue**:

- On the **producers'** side: condition variable `is_full`.
- On the **consumers'** side: condition variable `is_work`.

*Figure: The three-column layout — Producers (left), Queue (a 7-cell vertical buffer, currently empty), Consumers (right) — with the label "Condition variable `is_full`" over the producer side and "Condition variable `is_work`" over the consumer side.*

### The code on both sides

Producer side:

```c
queue.enqueue(work)
is_work=true
cond.signal()
```

Consumer side:

```c
while(1)
{
 while(!is_work)
 {
   wait(&cond, &ext_lock);
 }
 work = queue.deque();
 consume(work);
 is_work = queue.len()>0
}
```

A callout points at the `&ext_lock` argument of `wait`:

> Let's ignore `ext_lock`

### Walkthrough of the flow

*Figure: Blue arrows on the left represent producer threads advancing line by line through the producer code; orange arrows on the right represent consumer threads advancing through the consumer code; documents appearing in the queue cells are produced work items.*

The animation runs as follows:

1. **Start.** Three producer threads (blue arrows) are queued up to the left of the producer code; three consumer threads (orange arrows) sit at the `wait(&cond, &ext_lock);` line inside `while(!is_work)` — all blocked. The queue is empty.
2. The first producer reaches `queue.enqueue(work)`. The queue is still shown empty at this instant.
3. That producer advances to `cond.signal()`, and **one document now sits in the top queue cell** — the enqueue took effect.
4. A second producer advances to `cond.signal()` as well; the queue now holds **two documents**.
5. The producers spread out along the code (one still queued at its initial position to the left of the block, one at `cond.signal()`, one past the end of the block); the queue still holds two documents.
6. **A consumer wakes up.** Only two orange arrows remain at the `wait` line; the third has advanced past the inner `while(!is_work)` loop and now points at `work = queue.deque();`.
7. The remaining queued producer advances to `queue.enqueue(work)`; the queue still holds two documents.

## The missing synchronization on the shared queue

A red warning box is placed over the producer side, and both `queue.enqueue(work)` (producer) and `work = queue.deque();` (consumer) are highlighted in red:

> **Reading & writing not synchronized in shared queue!**

The box then turns green with an added assumption:

> **Reading & writing not synchronized in shared queue!**
> **Let's assume we used RWLock/semaphore/mutex…**

---

<!-- source pages 141-151 -->

## Consumers-Producers — running the scenario

The picture is the same three-column diagram throughout: **Producers** on the left, a shared **Queue** in the middle, **Consumers** on the right, with a **condition variable `is_full`** on the producer side and a **condition variable `is_work`** on the consumer side. A callout still says *"Let's ignore ext_lock"*, so `&ext_lock` is greyed out in the consumer code.

Producer side:

```c
queue.enqueue(work)
is_work=true
cond.signal()
```

Consumer side:

```c
while(1)
{
 while(!is_work)
 {
   wait(&cond, &ext_lock);
 }
 work = queue.deque();
 consume(work);
 is_work = queue.len()>0
}
```

*Figure: three blue arrows on the left stand for producer threads pushing work into the queue; the queue is drawn as a seven-slot column holding document icons; orange arrows run from the queue into the consumer code and one orange arrow marks the line the consumer is currently executing. The arrows advance frame by frame.*

### Step-by-step trace

The following frames advance one animation step each; the queue contents and the marked consumer line are the content.

1. **Queue holds 3 items.** The consumer's marker sits on `work = queue.deque();` — it is about to take an item.
2. **Queue holds 2 items.** The marker has moved to `consume(work);` — the item was removed from the queue and is being consumed.
3. **Queue holds 2 items.** The marker is on `is_work = queue.len()>0`, and a curved loop-back arrow is drawn from the bottom of the loop body up to `while(!is_work)`, showing the consumer going round the `while(1)` loop again.
4. **Queue holds 1 item.** Another turn of the loop has consumed an item; the marker is again on `is_work = queue.len()>0`, loop-back arrow still shown.
5. **Queue is empty.** The last item has been consumed; the marker is on `is_work = queue.len()>0`, which now evaluates to false because `queue.len()` is `0`.
6. **Queue is empty.** The marker has moved into `wait(&cond, &ext_lock);` — the condition `!is_work` held, so the consumer blocked on the condition variable.
7. **Queue is empty, consumer still in `wait(...)`.** On the producer side one of the three blue arrows has moved from the upper row down to the lower row; nothing has been enqueued.
8. **Spurious (fake) wakeups.** An orange callout box labelled **"Spurious (fake) wakeups"** points at the `while(!is_work)` line, and the consumer's marker jumps from `wait(...)` back up to `while(!is_work)`: the thread woke up without a real signal.
9. **Back into `wait(...)`.** Because the queue is still empty, `!is_work` is still true, so the re-tested `while` sends the consumer straight back to `wait(&cond, &ext_lock);`.
10. **Spurious (fake) wakeups again.** The callout reappears and the marker is once more on `while(!is_work)`; on the producer side all three blue arrows are now lined up together.
11. **Back into `wait(...)` again.** The condition is still false, so the consumer blocks in `wait(&cond, &ext_lock);` once more.

---

<!-- source pages 152-166 -->

## Consumers–Producers with a (FIFO) `ext_lock`

The previous version of this diagram told us to ignore `ext_lock`. Now the slide changes the instruction:

> Let's use (FIFO) `ext_lock`
> `ext_lock` released while waiting
> re-acquire when wakeup

Both sides of the diagram now take the external lock around their critical section.

*Figure: three-column layout — **Producers** on the left (labelled "Condition variable `is_full`"), a vertical **Queue** of cells in the middle, and **Consumers** on the right (labelled "Condition variable `is_work`"). Blue arrows on the left represent producer threads and their position in the producer code; orange arrows on the right represent consumer threads and their position in the consumer code; a document icon inside the top queue cell means the queue holds one work item.*

### Producer code

```c
acquire(&ext_lock)
queue.enqueue(work)
is_work=true
cond.signal()
release(&ext_lock)
```

### Consumer code

```c
while(1)
{
 acquire(&ext_lock)
 while(!is_work)
 {
   wait(&cond, &ext_lock);
 }
 work = queue.deque();
 is_work = queue.len()>0
 release(&ext_lock)
 consume(work);
}
```

Note the two changes on the consumer side compared with the earlier version: `is_work = queue.len()>0` is now computed **inside** the lock, and `release(&ext_lock)` happens **before** `consume(work);` — consuming is done outside the critical section.

### Animated trace

Three producer threads (**P1**, **P2**, **P3** — P1 is the rightmost blue arrow, closest to the producer code) and three consumer threads (**C1**, **C2**, **C3** — C1 is the rightmost orange arrow, closest to the consumer code).

| # | Producers | Queue | Consumers |
|---|---|---|---|
| 1 | — | empty | C1, C2, C3 all at `acquire(&ext_lock)` |
| 2 | — | empty | C1 at `while(!is_work)`; C2, C3 at `acquire(&ext_lock)` |
| 3 | — | empty | C1 at `wait(&cond, &ext_lock);`; C2, C3 at `acquire(&ext_lock)` |
| 4 | — | empty | C1, C2 at `wait(&cond, &ext_lock);`; C3 at `acquire(&ext_lock)` |
| 5 | — | empty | C1, C2, C3 all at `wait(&cond, &ext_lock);` |
| 6 | P1, P2, P3 arrive, all still before `acquire(&ext_lock)` | empty | all three at `wait(&cond, &ext_lock);` |
| 7 | P1 at `cond.signal()`; P2, P3 before `acquire(&ext_lock)` | one work item | all three at `wait(&cond, &ext_lock);` |
| 8 | P1 at `release(&ext_lock)` | one work item | all three at `wait(&cond, &ext_lock);` |
| 9 | P1 past `release(&ext_lock)` — finished | one work item | all three at `wait(&cond, &ext_lock);` |
| 10 | P1 finished | one work item | C1 at `work = queue.deque();`; C2, C3 at `wait(&cond, &ext_lock);` |
| 11 | P1 finished; P2 now at `acquire(&ext_lock)`; P3 still before it | one work item | C1 at `work = queue.deque();`; C2, C3 at `wait(&cond, &ext_lock);` |
| 12 | unchanged | empty | C1 at `is_work = queue.len()>0`; C2, C3 at `wait(&cond, &ext_lock);` |
| 13 | unchanged | empty | C1 past `release(&ext_lock)`, at `consume(work);`; C2, C3 at `wait(&cond, &ext_lock);` |
| 14 | P2 at `queue.enqueue(work)`; P3 still before `acquire(&ext_lock)` | empty | C1 back at `acquire(&ext_lock)` (top of the loop); C2, C3 at `wait(&cond, &ext_lock);` |
| 15 | P2 at `cond.signal()`; P3 still before `acquire(&ext_lock)` | one work item | C1 at `acquire(&ext_lock)`; C2, C3 at `wait(&cond, &ext_lock);` |

Points the trace makes:

- All three consumers first pile up on `acquire(&ext_lock)`; only one at a time gets past it.
- A consumer that finds `is_work` false calls `wait(&cond, &ext_lock)`, which **releases `ext_lock` while it waits**, letting the next consumer acquire it — that is how all three end up parked in `wait`.
- The signalled consumer (C1) does not run immediately: it must **re-acquire `ext_lock` on wakeup**. Meanwhile P2 blocks on `acquire(&ext_lock)` because C1 now holds it.
- C1 dequeues (queue becomes empty), recomputes `is_work = queue.len()>0` inside the lock, releases the lock, and only then runs `consume(work);`. In that frame P2 is still stopped at `acquire(&ext_lock)`; in the next frame C1 is already back at `acquire(&ext_lock)` at the top of the loop and P2 has advanced to `queue.enqueue(work)`.

---

<!-- source pages 167-180 -->

## Consumers–Producers — animated trace with `ext_lock` and the condition variable (continued)

### The standing slide

Every frame of this run shows the same three columns — **Producers**, **Queue**, **Consumers** — with a **condition variable `is_full`** labelled over the producer side and a **condition variable `is_work`** labelled over the consumer side. The queue is drawn as a tall box of seven cells; a document icon in the top cell means the queue holds one work item.

Producer code:

```c
acquire(&ext_lock)
queue.enqueue(work)
is_work=true
cond.signal()
release(&ext_lock)
```

Consumer code:

```c
while(1)
{
 acquire(&ext_lock)
 while(!is_work)
 {
   wait(&cond, &ext_lock);
 }
 work = queue.deque();
 is_work = queue.len()>0
 release(&ext_lock)
 consume(work);
}
```

A green callout, with a leader line pointing at the `wait(&cond, &ext_lock);` line, states:

- Let's use (FIFO) `ext_lock`
- `ext_lock` released while waiting
- re-acquire when wakeup

*Note: on the frame corresponding to page 170 the callout reads "Let's use ext_lock" — the "(FIFO)" word is missing on that one animation step only.*

*Figure: three blue arrows on the left point rightwards at the producer code and three orange arrows on the right point rightwards at the consumer code. Each arrow is one thread; its vertical position is the source line that thread is currently at, and arrows parked on the row below the last line of the code are idle threads. The orange arrows sit in three fixed columns — leftmost (call it C1), middle (C2) and the rightmost one that touches the code (C3) — so several consumers can be shown on the same line at once.*

### The trace, step by step

| Page | C1 (left) | C2 (middle) | C3 (touching code) | Queue | Producers |
|---|---|---|---|---|---|
| 167 | `wait(&cond, &ext_lock);` | `wait(&cond, &ext_lock);` | `acquire(&ext_lock)` | 1 item | 1 at `acquire(&ext_lock)`, 2 idle |
| 168 | `wait(&cond, &ext_lock);` | `wait(&cond, &ext_lock);` | `while(!is_work)` | 1 item | 1 at `acquire(&ext_lock)`, 2 idle |
| 169 | `wait(&cond, &ext_lock);` | `wait(&cond, &ext_lock);` | `work = queue.deque();` | 1 item | 1 at `acquire(&ext_lock)`, 2 idle |
| 170 | `wait(&cond, &ext_lock);` | `wait(&cond, &ext_lock);` | `is_work = queue.len()>0` | empty | 1 at `acquire(&ext_lock)`, 2 idle |
| 171 | `wait(&cond, &ext_lock);` | `wait(&cond, &ext_lock);` | `release(&ext_lock)` | empty | 1 at `acquire(&ext_lock)`, 2 idle |
| 172 | `wait(&cond, &ext_lock);` | `wait(&cond, &ext_lock);` | `consume(work);` | empty | 1 at `acquire(&ext_lock)`, 2 idle |
| 173 | `wait(&cond, &ext_lock);` | `wait(&cond, &ext_lock);` | `consume(work);` | 1 item | all 3 idle |
| 174 | `wait(&cond, &ext_lock);` | `work = queue.deque();` | `consume(work);` | 1 item | all 3 idle |
| 175 | `wait(&cond, &ext_lock);` | `consume(work);` | `consume(work);` | empty | all 3 idle |
| 176 | `wait(&cond, &ext_lock);` | `consume(work);` | `acquire(&ext_lock)` | empty | all 3 idle |
| 177 | `wait(&cond, &ext_lock);` | `acquire(&ext_lock)` | `acquire(&ext_lock)` | empty | all 3 idle |
| 178 | `wait(&cond, &ext_lock);` | `acquire(&ext_lock)` | `wait(&cond, &ext_lock);` | empty | all 3 idle |
| 179 | `wait(&cond, &ext_lock);` | `wait(&cond, &ext_lock);` | `wait(&cond, &ext_lock);` | empty | all 3 idle |

Reading the progression:

1. **167 → 169** — C3 holds `ext_lock`, passes the `while(!is_work)` test because the queue holds an item, and dequeues it. A producer is stuck at `acquire(&ext_lock)` for the whole of this stretch because C3 owns the lock.
2. **170** — C3 executes `is_work = queue.len()>0`; the item is gone and the queue box is now empty.
3. **171** — C3 executes `release(&ext_lock)`, giving up the lock.
4. **172** — C3 runs `consume(work);`, i.e. it works on the item *outside* the critical section.
5. **173** — the blocked producer has completed its whole critical section and returned to idle; the queue holds one item again. All three producers are now idle for the rest of the trace.
6. **174** — C2, which had been blocked inside `wait(&cond, &ext_lock);`, has been woken, re-acquired `ext_lock`, re-tested `while(!is_work)` and fallen through to `work = queue.deque();`.
7. **175** — C2 reaches `consume(work);` and the queue is empty again. Both C2 and C3 are consuming concurrently, outside the lock.
8. **176 → 177** — C3 loops round to `acquire(&ext_lock)`; then C2 finishes consuming and also arrives at `acquire(&ext_lock)`, so both consumers are contending for the lock at the same time.
9. **178** — C3 wins the lock, finds `is_work` false (empty queue) and blocks in `wait(&cond, &ext_lock);`, releasing `ext_lock` as it does; C2 is still at `acquire(&ext_lock)`.
10. **179** — C2 takes the freed lock, also finds no work, and blocks in `wait(&cond, &ext_lock);`. All three consumers C1, C2 and C3 are now lined up on the `wait(&cond, &ext_lock);` line, the queue is empty and all three producers are idle.

---

## Consumers–Producers — implementation

Implementation… HW 🎉
