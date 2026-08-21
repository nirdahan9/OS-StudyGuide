# Kernel Mode, User Mode and System Calls

> Study notes converted from the lecture slide deck `OS Presentation 3 - Kernel and user modes.pdf` (50 pages) — Operating Systems, Reichman University, Spring Semester 2026.
> Lecturer: Tsvi Cherny-Shahar · TAs: Daniel Karalnik, Liam Tal.
> Faithful conversion: every substantive item on the source pages appears here, and nothing has been added from outside them. Figures are rendered as italic *Figure:* descriptions.
> `<!-- source pages a-b -->` comments mark the original page ranges.

---

<!-- source pages 1-14 -->

*Figure: Title slide — the green Android mascot lying on its back with an open laptop resting on it and a large red warning triangle overlaid, on a black background.*

**Kernel, User, and all in between**

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

## What happens when we turn on the computer… continued…

*Figure: Boot-chain diagram. A top box labelled "efi system partition (FAT filesystem)" contains two files — `bootmgfw.efi` ("Typical for windows") and `grubx64.efi` ("Typical for Linux") — with the caption "EFI file name is based on the configuration written in the EFI partition". An arrow leads down to a "Main windows" partition (NTFS filesystem) box holding `[windir]/system32/winload.efi` and `[windir]/system32/winresume.efi`, which in turn arrows right into a second "Main windows" partition (NTFS filesystem) box holding `[windir]/system32/ntoskrnl.exe` plus "Libraries that interacts with devices called 'Device Drivers'". A callout points at the EFI partition: ".efi is a Portable Executable file format (PE)".*

- The EFI file name is based on the configuration written in the EFI partition.
- `.efi` is a Portable Executable file format (PE).
- Libraries that interact with devices are called **"Device Drivers"**.

*Figure: The same slide overlaid with Windows Device Manager screenshots — the device tree of machine `tc-pc` (Audio inputs and outputs, Batteries, Bluetooth, Disk drives → SAMSUNG MZVL21T0HCLR-00BL2, Display adapters, Network adapters, …), the drive's Properties dialog, and a "Driver File Details" dialog listing the driver files `C:\WINDOWS\system32\DRIVERS\disk.sys`, `C:\WINDOWS\System32\drivers\EhStorClass.sys`, `C:\WINDOWS\System32\drivers\partmgr.sys`, with Provider: Microsoft Corporation, File version 10.0.22621.1 (WinBuild.160101.0800), Digital Signer: Microsoft Windows. This shows the real device drivers loaded on a live system.*

---

## Monolithic OS

*Figure: Logos of ATARI, Commodore, Apple II and MS-DOS head the slide, with screenshots of an MSDOS 1.25 session (`A>dir` listing COMMAND.COM 17664, FORMAT.COM 6016, CHKDSK.COM 6400, SYS.COM, DEBUG.COM 1196…, SLOOP, "6 File(s)") and of Atari BASIC running `10 PRINT "HELLO WIKIPEDIA!"` / `20 GOTO 10`.*

- Boot process completes
- Runs a single process/application:
  - Usually the **"terminal"** to load other programs
  - Other examples:
    - ATARI gaming system – ATARI BASIC or a game
- If a program crashes – the "computer" crashes
- **The program can access entire memory**

### The memory map (640k)

*Figure: A single flat address space drawn from `0x0` at the top to `0xFFFFF` at the bottom, total "640k of memory", with arrows showing the program reaching into every region. Regions listed top-to-bottom: `0000h to 9FBFFh` conventional memory for program; `A0000h to AFFFFh` Video RAM for graphics (CGA, EGA, VGA); `B8000h to BFFFFh` Text video RAM for color displays; then unlabelled space. Beside it: "The program can access entire memory" and "Buggy code could literally access where it shouldn't !", illustrated with a corrupted, glitched screen image.*

| Range | Use |
|---|---|
| `0000h to 9FBFFh` | conventional memory for program |
| `A0000h to AFFFFh` | Video RAM for graphics (CGA, EGA, VGA) |
| `B8000h to BFFFFh` | Text video RAM for color displays |

- The program can access entire memory
- Buggy code could **literally** access where it shouldn't!

---

## Kernel-mode (privileged) | User-mode

*Figure: A horizontal black line splits the slide. Above it, green boxes for user-mode programs: File Explorer, Browser, Paint, Games, Office apps. Below it, blue boxes for kernel-mode components: Device drivers, Graphics drivers, Filesystem drivers, Crypto Operations, Malware scanners, Network protocols, Remote "drives", Virtualization. Arrows from "Office apps" and "Games" down toward the kernel line are stopped by red X marks — user-mode code cannot cross into kernel-land directly.*

- **User Mode (User-land)** – if app crashes – only it crashes
  - Browser
  - File Explorer
  - Games
  - Office apps
  - Paint
- **Kernel Mode (Kernel-land)** – if driver crashes – the whole computer crashes (a.k.a blue screen)
  - Device drivers
  - Filesystem drivers
  - Graphics drivers
  - Malware scanners
  - Network protocols
  - Remote "drives"
  - Crypto Operations
  - Virtualization

*Figure: The same slide overlaid with a PowerShell window running `PS C:\> driverquery`, showing the columns Module Name / Display Name / Driver Type / Link Date, with rows such as `1394ohci` (1394 OHCI Compliant Ho…, Kernel), `3ware` (Kernel, 19/05/2015 1:28:03), `ACPI` (Microsoft ACPI Driver, Kernel), `aehd` (Android Emulator hyper…, Kernel, 26/03/2024 22:55:18), `AppleSSD` (Apple Solid State Driv…, Kernel, 11/11/2019 23:24:17), `AppvStrm`/`AppvVemgr`/`AppvVfs` (File System), `arcsas` (Adaptec SAS/SATA-II RA…, Kernel). It demonstrates the real list of kernel-mode drivers on a Windows machine.*

### CPU Rings

*Figure: Four nested ellipses — the innermost green one labelled Ring3 holding the user-mode apps (File Explorer, Browser, Paint, Games), then Ring2 and Ring1 (yellow and orange) and the outermost red Ring0 in the kernel-mode half, holding "FS drivers" (Ring2 area) and "drivers" (Ring1 area). The same black line separates User Mode (top) from Kernel Mode (bottom). A callout notes "Usually, only ring0 and ring3 are used".*

- CPU holds **Current Privileged Level** (CPL) and enforces the memory access using the code segment in the **IP = (code segment) + (offset)**
- Segments are set by the OS during Boot using a **Page table** (we'll get into that!)
- Usually, only ring0 and ring3 are used

---

## System Call – Access the Kernel

*Figure: The user-mode / kernel-mode line again. In user-mode an "App" box issues **Open file**, with a double arrow crossing the line down into kernel-mode. Below the line: a red box `trap_handler() in legacy or skipped in "syscall"` and a red box `syscall_dispatch(X)`, connected by arrows both to each other and back up to the crossing point. `syscall_dispatch(X)` connects to blue kernel boxes Filesystem drivers, Malware scanners and Manufacturer Disk Driver, which in turn talks to a physical hard-disk image. A blue banner on the left states "Any access to 'Kernel' is done via System Call". A magenta arrow points from below at the ring transition, labelled "No context switch!".*

- **Open file** creates a **system call** to ask the kernel to access the file
- System call instruction (e.g., `syscall` or `int 0x80`) triggers kernel entry with code X in RAX register
- `trap_handler()` in legacy or skipped in "syscall"
- `syscall_dispatch(X)`
- Any access to "Kernel" is done via **System Call**
- **No context switch!** Thread transitions from Ring3 to (usually) Ring0
- Modern CPU `syscall` is a "special" trap that skips some of the "regular" trap route, to lower overhead.
- `syscall_entry` address is set during boot:

```c
wrmsrl(MSR_LSTAR, (u64)entry_SYSCALL_64);
```

---

## Syscall vs. SW Interrupt vs. Exception

- **Syscall – The process asks the OS for a service:**
  - `explorer.exe` – I need to open this file
  - `/bin/curl` – Open a new TCP socket to connect 192.168.0.1
  - `int 0x80` or `syscall` instructions
  - Enter ring0 **intentionally**
- **Exception – The process has done something I didn't expect**
  - `java.exe` – Divide by 0
  - `/bin/python` – Access to memory location 0x0 **(why is it illegal?!)**
  - Unexpected
  - Can result in termination or signal/SEH
- **SW Interrupt**
  - `int 3` – breakpoint triggered
  - `int 1` – execute one step of the IP (used by debuggers)
  - `int 0x80` – syscall in "legacy" OS (not distinguishing between SC and Int)
  - `int 0x20` (windows) – crash program

All three trigger a CPU transition to kernel mode (Ring0), **but differ in intent**:

- → System Call = A service request triggered by **application logic**
- → Exception = An **unexpected error** during execution
- → SW Interrupt = An **external/manual trigger** inserted **outside normal app logic**

---

<!-- source pages 15-30 -->

## `syscall` and `int 0x80`

- `int` CPU instruction stands for *interrupt*
- `int X` raises software interrupt with code X
  - On SW-int CPU saves context and jumps to pre-defined handler
- `int 0x80` invokes syscall interrupt (up to 32bit arch)
  - `eax` – syscall number | `ebx, ecx, edx`… holds arguments
- `syscall` CPU instruction invokes syscall
  - Faster, avoids interpretation of 0x80 code
  - Replaces `int 0x80`
  - `rax` – syscall number | `rdi, rsi, rdx, r10`… holds arguments

---

## `syscall` and `sysret` (in x86-64) (simplified)

### `syscall`

- RCX ← RIP - **Save RIP (next instruction address)** into RCX
- R11 ← RFLAGS - Save RFLAGS into R11 (contains carry flag, zero flag, etc.)
- RIP ← **syscall handler address**
- CS ← sets "code segment" register to "kernel mode" – **switch to ring0**
- RFLAGS ← RFLAGS & ~SFMASK – clears out (i.e. masks) user-mode related flags

### `sysret`

- RIP ← RCX – **Restores RIP**
- RFLAGS ← R11 – Restores RFLAGS
- CS ← sets "code segment" register to "user mode" – **return to ring3**

### Notes on `syscall`/`sysret`

- No usage of the *stack* (i.e. not writing to memory - slow)
- Notice, the application cannot just set "CS" directly and change rings (i.e. `mov CS ← X` is illegal)
- CS is changed only using specific CPU instructions, which controls if and where you can switch
- In Kernel mode, there's a different stack (RSP changes) with different addresses (we "ignore" this in the slide…)

---

## `int` and `iret` (in x86) (simplified)

### `int x`

- **if** ring3 → ring0 **then** push ESP – update ESP to stack in kernel memory space
- push EFLAGS - Save EFLAGS on the stack
- push CS - Save CS on the stack
- push EIP - **Save EIP on the stack**
- CS ← ring0 CS from memory – **switch to ring0**
- EIP ← trap handler from memory - loaded from IDT[X] - Interrupt Descriptor Table
- **if** ring3 → ring0 **then** ESP ← kernel stack address – written in memory

### `iret`

- pop EIP – **Restores EIP**
- pop CS - sets "code segment" register to "user mode" – **return to ring3**
- pop EFLAGS – Restores RFLAGS
- **if** ring0 → ring3 **then** pop ESP – restores stack to user mode

### Notes on `int`/`iret`

- Uses stack – which is slower than register
- `int` instruction exists in x64, but less common due to `syscall` instruction

---

## System Call (user-mode) – example x64

`Hello.s`:

```asm
    .section .data      # data section
msg:      .asciz "Hello, World!\n" # string to print

    .section .text
    .globl _start
_start:
    # print to the screen
    movq $1, %rax         # syscall number for sys_write (write to file)
    movq $1, %rdi         # 1 is the "file" STDOUT
    movq $msg, %rsi       # address of the string to output
    movq $14, %rdx        # number of bytes
    syscall # call the syscall_handler (int 0x80 in legacy calls trap_handler)

    # Check if sys_write succeeded
    test %rax, %rax       # check if the return value is zero or negative
    js sys_write_failed   # jumps if sys_write_failed is negative

    # call exit(0)
    movq $60, %rax  # syscall number for sys_exit
    xor %rdi, %rdi  # return 0 status
    syscall         # call the kernel
sys_write_failed: # calls exit(1)
```

Annotations on the slide:

- **Arguments to syscall** (the four `movq` instructions loading `%rax`, `%rdi`, `%rsi`, `%rdx`) goes into `sysframe` or `trapframe` in legacy
- `test %rax, %rax` – Sets zero flag (ZF) and sign flag (SF)
- `js sys_write_failed` – Jumps if the SF is 1 (negative)

### Building and running it

```shell
tcs@tc-pc:~/asm_example$ gcc -nostartfiles -no-pie -o hello hello.s
tcs@tc-pc:~/asm_example$ ./hello
Hello, World!
```

- **-nostartfiles:** Tells gcc there is no "main()" function. Entrypoint at label "_start"
- **-no-pie:** Do not build "position independent executable". In short, the created memory address are fixed

---

## OS/161

OS/161 – A pedagogy operating system for SYSTEM/161

- Ships with gcc (C compiler) that compiles to SYSTEM/161
- Ships with gdb (debugger) for SYSTEM/161

SYSTEM/161 – Emulator for a mix of R3000 & MIPS32 processor

- 32 Bit
- 31 "hardware" slots (network, disk, console etc.)

http://os161.org/

---

## System Call (kernel-mode) – example (older model, uses `int 0x80`, 32bit)

During **boot**, the OS places in a **predefined location** a "trap handler".

`start.s`:

```asm
li a0, EXADDR_GENERAL # Address 0x80000080
la a1, mips_general_handler # address of beginning of mips_general_handler
la a2, mips_general_end # end of mips_general_handler
sub a2, a2, a1
jal memmove
nop
```

`exception-mips1.s`:

```asm
mips_general_handler:
   j common_exception
```

*Figure: an arrow points at the `mips_general_handler` block — the "`syscall`" from the previous example reaches here! (It is `int 0x80` in 32bit).*

### `common_exception`

`exception-mips1.s`:

```asm
common_exception:
   # lots of code that does:
   # 1. saves the registers on the stack (so it can resume it)
   # 2. builds instance of "trapframe" struct containing
   #    information about the interrupt (+ the syscall arguments)

   # 3. calls mips_trap(struct trapframe *tf)
   addiu a0, sp, 16 # set trapframe* parameter – similar to %rax in x64

   # jal makes sure the IP register is set to the instruction after jal
   # when returning from mips_trap
   jal mips_trap    # call function

   # after return – restore the registers from the stack
   # sets return value and error into the relevant register
```

### `mips_trap`

`trap.c`:

```c
void mips_trap(struct trapframe *tf)
{
  …

  // Extract the code info from the register fields.
  uint32_t code = (tf->tf_cause & CCA_CODE) >> CCA_CODESHIFT;

  …

  // The interrupt is a system call (0x80 – EX_SYS)
  switch (code) {
    …
    case EX_SYS:
        syscall(tf); // system call dispatcher
        break;
    …
```

### `syscall` – the system call dispatcher

`syscall.c`:

```c
void syscall(struct trapframe *tf)
{
    int callno = tf->tf_v0;
    int32_t retval = 0;

    switch (callno) {
        …
        case SYS_write: // Writing to a file using the "filesystem driver"
            err = sys_write((int)tf->tf_a0, // File descriptor handler (STDOUT)
                    (const void *)tf->tf_a1, // Buffer
                    (size_t)tf->tf_a2, // Size of buffer
                    &retval); // Number of written bytes
            break;
        …

        default:
            err = ENOSYS;
    }
```

And the return path, in the same `syscall.c`:

```c
void syscall(struct trapframe *tf)
{
    …

    if (err) {
        tf->tf_v0 = err; // will be loaded in register v0 during restore of regs
        tf->tf_a3 = 1; // will be loaded to register a3 during restore of regs
    else { // Success
        tf->tf_v0 = retval;
        tf->tf_a3 = 0;      /* no error */
    }
```

*Note: the slide's `if`/`else` block is missing a closing brace before `else` — transcribed as shown.*

#### Trapframe ↔ x64 Linux register mapping

| Trapframe | x64 Linux | usage |
|---|---|---|
| tf_**v**0 | %rax | syscall code |
| tf_**a**0 | %rdi | arg1 |
| tf_**a**1 | %rsi | arg2 |
| tf_**a**2 | %rdx | arg3 |
| tf_**v**0 | %rax | return value |
| tf_a3 | - | 1=error \| 0 = OK |

---

## System Call

*Figure: a left green box `write()` and a right green box `Returned from write()` are each bracketed as "Usage"; between them a vertical stack of purple boxes — `Interrupt 0x80`, `handler()`, `sys_write()`, `sys_write()` — is bracketed as "Overhead !".*

---

## System Call – a few examples

**Process:**

- `fork()`
- `exec()`
- `exit()`
- `getpid()`
- `wait()`

**File system:**

- `open()`
- `close()`
- `write()`
- `read()`
- `seek()`
- `fstat()`
- `mkdir()`
- `chdir()`

**Security:**

- `chmod()`

**Network:**

- `socket()`
- `bind()`
- `listen()`

**Time:**

- `time()`
- `settime()`

**System:**

- `reboot()`

---

<!-- source pages 31-50 -->

## Signals & SEH — User-Space Exception Handling

- **Signals** (Linux) and **Structured Exception Handling** (**SEH**, Windows) allow **user-mode applications** to register handlers and try to respond to OS-level exceptions
- The application provides a handler function pointer, which the OS calls if the corresponding exception or signal occurs
- If no custom handler is set, the OS performs the **default action** (e.g., terminate the process or print a stack trace)

---

## SEH Exception (Windows)

- **`SetUnhandledExceptionFilter`** function in WINAPI sets an exception handler in **TIB** (Thread Information Block), a process-wide structure with information about the threads.
- **`AddVectoredExceptionHandler`** is similar to `SetUnhandledExceptionFilter`, but allows adding a vector of handlers called one after the other.

### Example — `seh_example.c`

```c
LONG ExceptionFilter(EXCEPTION_POINTERS *ExceptionInfo)
{
    if(ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_INT_DIVIDE_BY_ZERO)
    {
        printf("Caught divide by zero exception! :-o \n");

        // Skip the instruction pointer to the next instruction
        ExceptionInfo->ContextRecord->Rip++;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
int main(){
    printf("Registering custom exception filter.\n");
    SetUnhandledExceptionFilter(ExceptionFilter);

    printf("Going to divide by zero.\n");
    int x = 1 / 0; // This will trigger the exception

    printf("Phew... that was a close one.\n");

    return 0;
}
```

Questions posed on the slide:

- **What is the output?**
- Arrow pointing at `ExceptionInfo->ContextRecord->Rip++;` — **If we remove this line?**
- Arrow pointing at `SetUnhandledExceptionFilter(ExceptionFilter);` — **If we remove this line?**

---

## Signals (Linux)

- Signals are a well-known mechanism in Unix-like OSes (Linux, macOS, etc.) that allow the **OS** or another **user-mode process** to notify a process of an event (e.g., segmentation fault, termination request)
- In Linux, each process has a `task_struct` (its PCB), which includes a **signal handler table** used by the kernel to manage signal delivery
- The **signal handler table** maps **each signal number to** a **handler function pointer**, if one is set. If **no handler** is registered, the kernel uses a **default action**

### Signals — `pause()`

A two-thread walkthrough, built up step by step across the slides.

**Step 1 — the waiting thread calls `pause()`:**

```c
void* waiting_thread(void* arg)
{
    pause(); // wait (blocks) for any signal
    return NULL;
}
```

1. Stops the thread (state changes to block)
2. Saves context on stack
3. Places **signal frame (i.e. a struct)** on the stack:
   1. `sigreturn` → next address execute (after pause)
   2. A **placeholder** (space in the memory) for the incoming signal number
4. Places in **RIP** the address of the *signal handler* (**stored in the PCB not TCB**)

**Step 2 — the sending thread signals it:**

```c
void* sending_thread(void* arg)
{
    pthread_t* target = (pthread_t*)arg;
    pthread_kill(*target, SIGUSR1);
    return NULL;
}
```

1. `pthread_kill(tid, SIGUSR1)`
   - `syscall 270`
   - `SIGUSR1` (== 10) is passed as argument
2. Kernel passes the signal (10) to the other thread

**Step 3 — delivery back on the waiting thread:**

- OS writes on the waiting thread stack that the received signal is SIGUSR1
- and moves it to READY state (wake up!)
- Calls signal handler:
- Signal handler sees SIGUSR1 is not handled and reverts to default behavior which is… **TERMINATE PROCESS (in Linux…)**
- Process terminates

**Step 4 — let's register a "dummy" handler:**

```c
void handler(int sig) { }

void* waiting_thread(void* arg)
{
    signal(SIGUSR1, handler);
    pause(); // wait (blocks) for any signal
    return NULL;
}

void* sending_thread(void* arg)
{
    pthread_t* target = (pthread_t*)arg;
    pthread_kill(*target, SIGUSR1);
    return NULL;
}
```

### Signals — asynchronous signal

```c
void handler(int sig) {// handle }

void* waiting_thread(void* arg)
{
    signal(SIGUSR1, handler);
    while(1) // doing some work
    return NULL;
}

void* sending_thread(void* arg)
{
    pthread_t* target = (pthread_t*)arg;
    pthread_kill(*target, SIGUSR1);
    return NULL;
}
```

On incoming signal of SIGUSR1:

1. Saves context (+RIP) on stack
2. push **signal frame** on the stack:
   1. `sigreturn` → next address execute (after pause)
   2. A place for the incoming signal number
3. Update **RIP** to *signal handler*
4. Run handler
5. Resumes context (+ RIP) – thread resumes

### Signals — wait a signal

We just want to wait for signal…
We don't want a **handler** or **resetting the RIP**

**Simply… wait…**

```c
void* waiting_thread(void* arg) {
    sigset_t set;
    int sig;

    // 1. Initialize empty signal set
    sigemptyset(&set);

    // 2. Add SIGUSR1 to the set
    sigaddset(&set, SIGUSR1);

    // 3. Block SIGUSR1 in this thread
    // 3rd out params retrieves previous mask
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    // 4. Wait for SIGUSR1
    sigwait(&set, &sig);
    return NULL;
}
```

`pthread_sigmask`:

- **Prevents** the OS from running the **signal handler**
- Affects **only the calling thread**
- **Stored on the TCB!**
- To undo: `pthread_sigmask(SIG_UNBLOCK, &set, NULL);`

`sigwait`:

- Blocks the thread until a signal in the set is receives
- **No signal frame**
- **No RIP changes**

---

## Default Behavior (default disposition)

Every signal has a **default behavior** used when **no handler is registered**.
Depends on the signal #, one of the following can occur:

1. Terminate the process
2. Ignore
3. Stop or continue process (affects all threads)
4. Core dump (write all the process memory to disk)

---

## Signal Delivery Priority (POSIX Rules)

| Priority | Condition | What Happens |
| --- | --- | --- |
| 1st | ***sigwait()*** — Threads are waiting in **sigwait()** | **One thread** receives the signal and returns. signal is consumed, **no handler runs** |
| 2nd | ***handler*** — Signal is **not blocked** and **signal # is registered to a handler** (i.e., using `signal()`/`sigaction()`) | **One arbitrary <u>eligible</u> thread** (i.e., not blocking the signal) receives the signal. Does not have to be the one that called *signal()*. **handler runs** |
| 3rd | ***handler – but blocked*** — Signal **has a handler, but blocked in all threads** | Signal is **queued as pending** |
| 4th | ***No sigwait() && no handler → Default*** — Signal has **no handler and no sigwait() on it** | Default action applies. process **may** terminate, stop, etc. |

---

## setjmp & longjmp

- `setjmp` function (standard C function) – stores the current thread context in `jmp_buf` struct (i.e., RIP, RSP, etc.) – and **returns 0!**
  - "Saves" thread context
- `longjmp` function restores the context of the thread and **returns non-zero**
  - "Loads" thread context

### Example 1 — `setjmp_longjmp_example.c`

```c
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>

jmp_buf buf;

int main()
{
    if (setjmp(buf) == 0) // returns 0 on set, non-0 on jmp
    {
        printf("jump to else\n");
        longjmp(buf, 1); // Jump back to the point set by setjmp

        printf("We never get here...\n");
    }
    else
    {
        // Returned from longjmp call, returns 1
        printf("IN ELSE!\n");
    }

    printf("Program continues normally.\n");
    return 0;
}
```

**What is the output?**

*Note: on the slide the opening quote of `printf(“We never get here...\n");` is a typographic quote, not a plain `"`.*

### Example 2 — `sig_example.c` (signal handler + longjmp)

```c
jmp_buf buf; // holds the state of the thread

void signal_handler(int signum){
    if (signum == SIGFPE)
    {
        printf("Caught divide by zero exception! :-o \n");
        printf("Restoring the state of the thread\n");
        longjmp(buf, 1);
    }
}

int main(){
    // Register the custom signal handler
    signal(SIGFPE, signal_handler);

    if (setjmp(buf) == 0){
        int res = 1 / 0;
        printf("NOT executed!\n");
    } else{
        printf("Phew... that was a close one...\n");
    }

    return 0;
}
```

**What is the output?**

### Example 3 — longjmp into a dead stack frame

```c
jmp_buf buf;
void bad_func()
{
    if (setjmp(buf) == 0)
    {
        // Save context here
        return;  // Return makes the stack frame invalid
    }
    else
    {
        // This code is dangerous: returning to a dead stack
        printf("Returned via longjmp into a dead stack!\n");
    }
}

int main() {
    bad_func();      // setjmp called here
    longjmp(buf, 1); // jumps to a function that already returned
    return 0;
}
```

**What is the output?**

### Example 4 — longjmp across threads

```c
void* thread2_func(void* arg) {
    printf("Thread 2: jumping back to Thread 1...\n");

    // JUMPING to Thread 1's stack! NOOooo....!
    // Undefined behavior :-( - hopefully crash
    longjmp(buf, 42);
    return NULL;
}

void* thread1_func(void* arg) {
    int val = setjmp(buf);
    if (val == 0) {
        printf("Thread 1: setjmp returned 0\n");

        // create thread2
        pthread_t t2;
        pthread_create(&t2, NULL, thread2_func, NULL);
        pthread_join(t2, NULL);  // Will likely never return

    } else { printf("got here from longjmp"); }

    return NULL;
}
```

**What is the output?**

*Note: the "Undefined behavior" comment on the slide contains a frowning-face emoji rather than the characters `:-(`.*

---

## User-mode threading

- An application can use `setjmp`/`longjmp` to **"jump a thread around"**
- This jump, **when scheduled**, mimics the **concurrency done in kernel**
- Pros:
  - No system call overhead on context switch
  - The required state is smaller
- Cons:
  - Affects the OS process scheduling (Kernel not aware of these threads)
  - **No true parallelism** on multiple cores - all user-mode threads run on a single kernel thread

Callout box on the slide: **Implementation… HW**

---

## Next lecture

But when do we context switch?
To whom do we context switch?

All and more…

*Figure: "IN THE NEXT LECTURE…" title-card graphic in Back-to-the-Future styling on a black slide, teasing the scheduling topics listed above.*
