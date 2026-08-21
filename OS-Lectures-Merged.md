# Operating Systems — All Lectures (Merged)

> Single-file merge of all 7 lecture decks — Operating Systems, Reichman University, Spring Semester 2026.
> Lecturer: Tsvi Cherny-Shahar · TAs: Daniel Karalnik, Liam Tal.
>
> This file is a **concatenation** of the individual deck files in `Lectures-md/` — identical content, nothing added or removed. Use it when you want the whole track in one context window; use the individual files when you want to load only one topic.
>
> Source PDFs: `Lectures/` (658 pages total, also merged as `lectures-merged.pdf`).
> Each deck keeps its own `#` title and source note. `<!-- source pages a-b -->` comments mark original page ranges within each deck.

## Contents

1. [Introduction to Operating Systems](#introduction-to-operating-systems) — `Presentation 1 - 1 Slide.pdf`, 42 pages · also standalone at [`Lectures-md/OS1_Introduction.md`](Lectures-md/OS1_Introduction.md)
2. [Threads, Concurrency and Synchronization](#threads-concurrency-and-synchronization) — `Presentation 2 - 1 slide.pdf`, 180 pages · also standalone at [`Lectures-md/OS2_Threads_and_Synchronization.md`](Lectures-md/OS2_Threads_and_Synchronization.md)
3. [Kernel Mode, User Mode and System Calls](#kernel-mode-user-mode-and-system-calls) — `OS Presentation 3 - Kernel and user modes.pdf`, 50 pages · also standalone at [`Lectures-md/OS3_Kernel_and_User_Mode.md`](Lectures-md/OS3_Kernel_and_User_Mode.md)
4. [CPU Scheduling](#cpu-scheduling) — `OS Presentation 4 - 1 Slide-2.pdf`, 65 pages · also standalone at [`Lectures-md/OS4_Scheduling.md`](Lectures-md/OS4_Scheduling.md)
5. [Memory and Virtual Memory](#memory-and-virtual-memory) — `Presentation 5 - Memory - 1 Slide-2.pdf`, 129 pages · also standalone at [`Lectures-md/OS5_Memory.md`](Lectures-md/OS5_Memory.md)
6. [File Systems](#file-systems) — `OS 6 - Filesystems - 1 Slide.pdf`, 102 pages · also standalone at [`Lectures-md/OS6_Filesystems.md`](Lectures-md/OS6_Filesystems.md)
7. [Executables, Linking and Loading](#executables-linking-and-loading) — `Presentation 7 - Executables - 1 Slide-2.pdf`, 90 pages · also standalone at [`Lectures-md/OS7_Executables_and_Linking.md`](Lectures-md/OS7_Executables_and_Linking.md)

---

# Introduction to Operating Systems

> Study notes converted from the lecture slide deck `Presentation 1 - 1 Slide.pdf` (42 pages) — Operating Systems, Reichman University, Spring Semester 2026.
> Lecturer: Tsvi Cherny-Shahar · TAs: Daniel Karalnik, Liam Tal.
> Faithful conversion: every substantive item on the source pages appears here, and nothing has been added from outside them. Figures are rendered as italic *Figure:* descriptions.
> `<!-- source pages a-b -->` comments mark the original page ranges.

---

<!-- source pages 1-21 -->

## Title Slide

*Figure: The opening slide is styled as a Windows "Blue Screen of Death" — a full blue screen of white monospace text announcing the operating systems course.*

```
A problem has been detected and the computer has been shut down to prevent damage.

E_OPERATING_SYSTEMS_COURSE

If this is the first time you've seen this error screen, restart your computer. If this screen appears again, follow these steps:

Check to make sure any new hardware or software is properly installed. Make sure you submit your homework and pass the test.

If problems continue, disable or remove any newly installed hardware or software. Disable BIOS memory options such as caching or shadowing. If you need to use Safe Mode you are stuck. There is no safe mode.

Technical Information:

*** TSVI CHERNY SHAHAR (cherny.tsvi@runi.ac.il)


Collecting data for crash dump ...
Initializing disk for crash dump ...
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

## Course Information

- Lecturer: Tsvi Cherny-Shahar
- TAs: Liam Tal & Daniel Karalnik
- Office Hours: Schedule via Moodle or mail
- Textbooks:
  - Modern Operating Systems, Second Edition (2001) By Andrew S. Tanenbaum
  - Ops-class.org

Materials presented takes priority over other materials

*Figure: Cover of "Modern Operating Systems", Second Edition, by Andrew S. Tanenbaum.*

### HW & Grading

- 3 Hands-on/theoretical Exercises (in C) – 40% (13.3% each)
- Test (studied material) – 60% (60 to pass, must pass to complete)

### What will we learn?

- Multi-threading & Synchronization
- Kernel/User modes - What is that?! and why?
- Scheduling - Which job to run? For how long?
- Memory management and Virtual memory
- How the file system works?
- How executables and dynamic libraries work? Literally!

### Ask Questions!!!

IF YOU DON'T TELL ME YOU DON'T UNDERSTAND
I DON'T KNOW YOU DON'T UNDERSTAND!

IF YOU DON'T UNDERSTAND – MANY DON'T UNDERSTAND

IT IS NOT THAT EVERBODY UNDERSTAND
IT'S BECAUSE THEY ARE TOO SHY TO ASK!
HELP EVERYONE - ASK!

*Note: "EVERBODY" is spelled as in the original slide.*

---

## First Computers

Two machines open the story:

- **The Bombe** — Built to crack the Enigma
- **ENIAC** — 1st **general purpose Turing-complete** computer

*Figure: Section-opener photographs — the Bombe's rows of rotating drums on the left, and the ENIAC machine room with an operator at a rack of cabling on the right.*

### The Bombe

*Bombe* is an Electro-magnetic **device** designed by Alan Turing and refined by Gordon Welchman to crack the Enigma. Completed in 1940.

Hard-wired device to **run a single program**

*Figure: Portraits of Alan Turing and Gordon Welchman beside a photograph captioned "Bombe 1940" showing the machine's front panel of drums.*

### ENIAC (1st gen 1945-1955)

*ENIAC* (Electronic Numerical Integrator and Computer) was the first **general purpose Turing-complete** computer in 1945.

As such, it was reprogrammable…

Designed by John Mauchly and John Presper

*Figure: Portraits of John Mauchly and John Presper next to a photograph captioned "ENIAC 1945" of the ENIAC machine room.*

### How to program and execute on such a computer?

*Figure: Photograph of ENIAC operators standing at a wall-sized panel, plugging and re-plugging cables — the physical act of "programming" the machine.*

The first six main programmers of ENIAC: Kay McNulty, Betty Jennings, Betty Snyder, Marlyn Meltzer, Fran Bilas and Ruth Lichterman wrote the first programs using **plug-boards** to connect different components of ENIAC to produce the required calculation.

Later 1st gen computers also used **vacuum tubes** alongside plug-boards.

*Figure: Portrait photographs of the six programmers — Marlyn Meltzer, Fran Bilas, Ruth Lichterman, Kay McNulty, Betty Jennings, Betty Snyder — plus a photo of two of them operating an ENIAC plug-board unit.*

---

## First Computers – Transistor Computer (2nd gen 1955-1965)

With the appearance of transistors, computer became smaller, and programs were written using punched cards or magnetic tape.

*Figure: Period photographs of operators at card-punch machines, a UNIVAC magnetic tape drive, and a keypunch/printer terminal.*

### Punch cards and libraries

Programmers would punch their program's statements into punch cards.

With time, *libraries* of punch cards and magnetic tapes emerged containing common statements for re-use.

But still, programmer would need to **manually operate the computer**, feeding their programs.

*Figure: Close-up of a punched card labelled "FORTRAN STATEMENT", showing the numbered column grid and punched holes encoding one statement.*

### Batch processing

By collecting multiple statements together, i.e. batch of cards, the programmers create a whole program. That is "Batch processing".

Computer were also used to write the cards into magnetic tapes for better performance…

*Figure: Photograph of a thick stack of punched cards with hand-written red markings, captioned: "Punched cards batch = a list of statements = a program!"*

### The machines involved

| Machine | Input | Output |
| --- | --- | --- |
| IBM 1401 | slow punch card | "fast" magnetic tape |
| IBM 7151 Mainframe | magnetic tape | magnetic tape |
| IBM 1403 Printer | magnetic tape | written paper! |

*Figure: Three photographs, one per machine — the IBM 1401, the IBM 7151 mainframe room with rows of tape drives, and operators at the IBM 1403 printer.*

### The programmer's physical workflow

Programmers would manually input their program into the computer, get the output, walk to another machine.

Programmers would, physically, wait in line for their turn to *process* their programs.

**Configuration and setup** of the computers' plug-boards and program, for each programmer, was also a "waste" a lot of time (and money) even before running the programs themselves (let alone time spent on bugs…)

*Figure: Line-drawing pipeline of three stations. Station 1 — a programmer at a 1401 with a card reader and tape drive: "Batch of cards → 'fast' magnetic tapes". Station 2 — an operator carries the input tape to a 7094 flanked by input tape, system tape and output tape: "Run program and get output". Station 3 — an operator carries the output tape to a 1401 with a 1403 printer: "Print the output".*

---

## Toward a system that operates the execution of the program

In General Motors 1954, batch processing was part of the daily work of the programmer.

***Robert L. Patrick***, a programmer in GM, tried to "squeeze" more jobs into his "time slot" with the machine.

Instead of writing his entire batch into the tape, he "pre-created" a tape with his program using memory addresses he pre-defined.

Now, his cards would only need to contain modifications and updates of the program on tape, which requires less cards (i.e. more time!)

> "He was then able to get four or five runs each day in one quarter of the total machine time of other programmers' single runs"\*

\* https://www.abortretry.fail/p/the-history-of-gm-naa-io-and-share

*Figure: Photograph captioned "GM IBM 701 - 1954" showing the GM machine room with the IBM 701 cabinets.*

### Robert's contributions

Robert had 4 main observations and contributions:

- Get programmers off the console and out of the machine room
- Standardizing on setup
- Avoid reloading by using magnetic tape and updating with a change deck
- Reuse code liberally wherever possible

In 1955, Robert's techniques were merged into GM's effort to formalize GM I/O, a system that **operates** the process of **executing programs**.

\* https://www.abortretry.fail/p/the-history-of-gm-naa-io-and-share

*Figure: The General Motors "GM" logo appears in the slide corner.*

---

## GM I/O (or GMR Monitor) – The 1st operating system

The system has three phases:

1. Batch the jobs (with regards to their input formats)
2. Run the jobs
3. Convert output to human readable format and print

The key point is Robert's program on tape, where programmers would just change parts of it on the magnetic tape without worrying about the computer's configuration (plug boards).
**The rest of the program remained the same**

It is a program that its goal is to execute other programs.

\* https://www.abortretry.fail/p/the-history-of-gm-naa-io-and-share

---

## Multi-Programming & Time Sharing (3nd gen 1965-1980)

*Note: the slide title reads "3nd gen" in the original.*

When programmers ran a program, many times it used peripherals (e.g. printer).

For example, the program sends the output to the printer and **waits** for the printer to finish its job.

While the program **waits**, the CPU is not running the program, therefore **it can run other jobs in the meantime**!

### SPOOL, Multiprogramming, Time-Sharing

**S**imultaneous **P**eripheral **O**peration **O**n-**L**ine or just… **SPOOL**

**Multi-Programming:**
Using a **"job pool"** data structure (and algorithms), OS can choose (or **schedule**) which is the *best job* to run **while waiting** improve overall *CPU Utilization*.

**Time-sharing (multi-tasking):**
Similar to mutli-programming but switches jobs based on time-frame.

*Note: "mutli-programming" is spelled as in the original slide.*

*Figure: Memory is drawn as a vertical stack of blocks — from top: Job 3, Job 2, Job 1, and at the bottom the "operating system" block; a brace on the right labels the whole stack "Memory". An arrow points from Job 1 to a box reading "Waiting for the printer to finish…". A thought bubble from the operating system block asks "Which job to run in the meantime?" with "Pool: Job1, Job2, Job3".*

---

<!-- source pages 22-42 -->

## Later Computer Generations

**4th gen (since 1980): Personal computer & hand-held devices**

*Figure: photos of an early personal computer with a green-text CRT, a modern Windows laptop, a smartphone and a tablet.*

**5th gen (since recent years): Multi-core, virtualization, containerizing, subsystems**

*Figure: icons illustrating the 5th generation — a CPU chip labelled "CPU MULTIPLE CORES" plus a GPU chip labelled "GPU THOUSANDS OF CORES", a monitor displaying a virtual machine inside a desktop, the Docker whale logo, and a Windows logo with the Linux penguin peeking through it (subsystems).*

---

## Terminology

- **Latency** (time unit) – The time to complete a job
- **Throughput** (amount/time) – The amount of data processed in a time unit
- CPU **Utilization** – The time the CPU is executing ***none-OS* code**
- **Overhead** – Time spent on other things that are not running the program's code (e.g. access memory, OS computation, etc.)
- CPU **Usage** – Utilization + Overhead

*Figure: latency is illustrated by two "Job" bars, a short one spanning 1ms and a long one spanning 2s. Throughput is illustrated by a single "Job" bar annotated $2\frac{bytes}{ms}$.*

---

## PC Motherboard

**Connects the hardwares**

*Figure: fully annotated photo of an ASUS P5AD2-E Premium motherboard (from computerhope.com), with callouts to 2x PCI Express slots, 3x PCI slots, 1 AGP slot, onboard audio integrated circuit, CD-IN, SPDIF, 1394 header and 1394 controllers, onboard LED, SATA RAID, jumpers, SATA controller, USB header, serial port connector, ATA controller, Game/MIDI header, FWH in PLCC (circled), system panel connectors, primary and secondary ATA RAID, coin cell CMOS backup battery, 4x SATA connections, Southbridge, 24-pin motherboard power connector, primary ATA (IDE) connector, floppy connector, Marvell onboard wireless chipset, 2x 3-pin case fan connectors, back panel and I/O connections, heat sink, P4 power connector, inductor (coil), capacitors, Gigabit LAN header, oscillator, CPU socket lever, CPU socket, voltage regulator, Northbridge, 4-pin CPU fan connection, 4x DIMM memory slots, Super I/O and mounting screw hole.*

## Simplified view of a PC

*Figure: block diagram — CPU, Memory, Video controller, Keyboard controller, Floppy disk controller and Hard disk controller all hang off a single shared **Bus** at the bottom; above the controllers sit their devices, the Monitor on the video controller, the Keyboard, the Floppy disk drive and the Hard disk drive.*

---

## Interrupt (Hardware)

When hardware wants to signal the CPU something happen, it uses an interrupt signal on the BUS.

Each interrupt type has a dedicated entry in the CPU.

On interrupt, the CPU immediately stops, looks at the ***interrupt table*** where in the memory it should **JMP** by setting the ***Instruction Pointer***.

The previous value is stored and restored after the interrupt has been handled.

***Instruction Pointer*** – a register holding the address of the **next** instruction to execute (also called "Program Counter").

*Figure: a **Disk drive** wired to a **Disk controller**, which together with an **Interrupt controller** and the **CPU** all hang off one shared bus. Four numbered arrows trace the interrupt: (1) CPU → Disk controller over the bus (issuing the command), (2) Disk controller → Interrupt controller over the bus, (3) Interrupt controller → CPU directly, and (4) an arrow over the bus back up into the CPU.*

---

## What happens when a modern computer starts?

- The CPU **instruction pointer (IP)** is **hard-wired** to **0xFFFFFFF0**
- At **0xFFFFFFF0** lies the code for the ***Unified Extensible Firmware Interface (UEFI)*** or in older systems, ***Basic Input Output System (BIOS)***
- The code of the UEFI (a.k.a firmware) is stored on a special storage based on the motherboard
- First instructions run the **Power-On Self Test** (POST)
  - In case a test fails, the computer BEEP a special beep based on the error
  - How does the POST communicate with the Speaker (also called PC-Speaker)? The UEFI contains code to use the built-in speaker

### Finding a bootable device

- The UEFI code than loads the configuration settings of the computer to find a "bootable" device (huh?! Storage?!)
  - The settings are stored on a Non-Volatile Memory (NVRAM), or in older systems using CMOS memory.
  - This dedicated memory is accessed by the firmware to get the bootable devices and boot order (among other things…)
  - How does the UEFI reads the data? It has basic I/O code to read the data

*Figure: photo of an "ASUS UEFI BIOS Utility – Advanced Mode" screen showing BIOS Information (version 0406 x64, build date 06/27/2013, EC version MBE2-Z87-0225, ME version 9.0.0.1310, South Bridge Stepping 04/C1), CPU Information (Intel(R) Core(TM) i7-4770K CPU @ 3.50GHz, speed 3507 MHz), Memory Information (Total Memory 16384 MB (DDR3), Memory Frequency 1600 MHz), System Language English, System Date [Friday 07/30/2021], System Time [20:36:53], Access Level Administrator.*

- UEFI will try to boot based on the order configured in the settings
  - Today UEFI firmware also support network devices for boot, not just storage devices
- UEFI looks for the **bootloader**, the "OS executable" ***file (?!)*** – to run the OS.

*Figure: a "Please select boot device:" boot menu listing `USB:Fujitsu Remote Iso CDROM` (highlighted), `SCSI:#5000 ID00 LUN0 ATA S` and `Network:IBA GE Slot 0100 v1371`, with the help text "↑ and ↓ to move selection / ENTER to select boot device / ESC to boot using defaults".*

### Finding the bootloader file

- How to find the **bootloader file** on the disk?
  - UEFI must **read** the data **from the disk**
  - UEFI must **have code** that **reads data from the storage device**
  - UEFI must understand some sort of "**filesystem**?!"
- UEFI looks for a **partition** - segments of sectors within the storage.
  **The partition is** formatted with the **FAT filesystem** to understand what is a file.
  The UEFI code **must understand**, to some extent, **what are partitions** and must **understand FAT**.
- Don't worry – we'll get into all that in details!
- UEFI find the **EFI System Partition (ESP)** which is formatted to **FAT** (usually FAT32)
- Finds the **bootloader file** located in a predefined location
- Executes the Bootloader! (?!?! EXECUTE ?!?!)

*Figure: a Windows Disk Management strip for "Disk 0, Basic, 953.85 GB, Online" showing a 260 MB "Healthy (EFI System Par…)" partition, a 951.65 GB "Windows-SSD (C:) NTFS Healthy (Boot, Page File, Crash Dump, Basic Data Partition)" partition, and a 1.95 GB "Healthy (Recovery Partition)"; beside it, a file icon labelled `winload.efi`.*

### The bootloader is the OS

The **Bootloader** is the ***operating system executable***!

At this point – **the program called <u>operating system</u>** takes over and starts running…

- But **how** can the **UEFI understand** the **executable** file format? How does it know how to run it?
- **.efi** boot loaders are in the **Portable Executable (PE) format**. The same format as **Windows executables**.
  UEFI must know how to execute a PE executable! **Even if it is not Windows!**

If no bootable device is found, you'll get something like: (which means UEFI must know how to print to the screen)

*Figure: photo of a boot failure screen reading "No bootable devices found. / Press F1 key to retry boot. / Press F2 key for setup utility. / Press F5 key to run onboard diagnostics."*

---

## Some OS Abstraction

- **Thread** – Abstract the CPU execution of the program
- **Address Space** – Abstracts the memory
- **File** – Abstract the inner structure of storage
- **Process** – Ties all entities of a running program together

*Figure (images from ops-class.org): an Address Space bar running from `0x00000000` to `0x80000000` with green `code`, `heap` and `stack` regions; a File box holding the path `/home/challen/cse421/slides.html`; and a Process box that contains Thread 1, Thread 2 and Thread 3 arrows over one Address Space (with three stacks), plus two File Handles (mode=RW, offset=… and mode=R, offset=…) pointing at the files `/home/challen/cse421/slides.html` and `/home/challen/cse421/figures/file.svg`.*

---

## The thread of the process

In a single operating system process, like **MS-DOS**, the thread running the OS is also used to run applications.

In a multi-program operating system, like **Windows / macOS / Android**, a thread is spawned to run a process.
That is the ***main thread***, and the program executes as long as the main thread runs.

Multiple programs (with a single thread) running **concurrently** → multiple threads running **concurrently**

*Figure: three separate boxes, each a process with one Thread arrow over its own Address Space (code / heap / stack), are contrasted with a single larger box holding one process whose Thread arrow branches into three Thread arrows over one shared Address Space.*

*Figure: a Process Explorer (Sysinternals) screenshot listing processes (conhost.exe, OpenConsole.exe, pwsh.exe, py.exe, python.exe, POWERPNT.EXE selected with PID 84644, ai.exe, javaw.exe, Acrobat.exe) with the Threads pane below showing threads of POWERPNT.EXE; the first row (TID 27016, User Time 00:03:56, Kernel Time 00:02:33, CPU Time 00:06:30, Start Time 09/24/24 15:18:41, Start Address POWERPNT.EXE+0x1a10, Base Pri 8, Dyn Pri 10) is circled with the caption "The first thread is the main thread…". Status bar: CPU Usage 7.80%, Commit Charge 76.40%, Processes 487, Physical Usage 67.96%.*

---

## Concurrency vs. Parallelism

***Concurrency*** - run multiple tasks in *overlapping* time periods

***Parallelism*** – run multiple tasks simultaneously

*Figure: on **1 CPU core**, task A runs, then B runs, then A again, then B again — the bars alternate along the time axis and never coincide. On **2 CPU cores**, the A bar and the B bar span the same full stretch of the time axis, one above the other.*

---

## Context Switch (time sharing)

We have several processes, but 1 CPU core.

We want to run the processes ***concurrently*** (the user "feels" it runs in parallel).

***Take turns on the CPU!***

The CPU **switch its context** between the different processes.

*Figure: timeline of A → CS → B → CS → A → CS → B, where the small orange "CS" boxes between the blue task bars mark a context switch. Alongside, two boxes each showing a Thread arrow over an Address Space (code / heap / stack), both feeding one CPU chip.*

The same happens in a single process with multiple threads…

*Figure: the same A / CS / B / CS / A / CS / B timeline, but now the CPU is fed by a single box in which one Thread arrow branches into three Thread arrows over one shared Address Space.*

---

## Multi-thread & Scheduling

The operating system decides which thread to run next.

In other words, it ***schedules*** the tasks.

*Figure: timeline with three tasks — A runs, CS, B runs, CS, A runs again; separately a CS precedes C, which runs on its own track later on the time axis. Orange "CS" boxes denote context switches.*

Processes and their threads have different priorities. This is one of the factors that affect the schedulers.

*Figure: Process Explorer with POWERPNT.EXE (PID 84644) selected and its Properties → Threads tab open (Count: 85), listing TIDs 3044, 3564, 8704, 11036, 12844, 13776, 14016, 14744, 17388, 18568, 19568, 20080, 22824, 24668, 26280, 27016 (selected, start address POWERPNT.EXE+0x1a10), 32292, 32764, 35276 with start addresses such as ntdll.dll!RtlClearThreadWorkOnBehalfTicket+0x70, mso20win32client.dll+0x21d330, ucrtbase.dll!recalloc+0x10, GROOVEEX.DLL+0xb04f0, msvcrt.dll!endthreadex+0x30. The detail pane circles Thread ID 27016, Base Priority 8 / Dynamic Priority 10, and Context Switches 19,118,806 (Kernel Time 0:02:34.281, User Time 0:03:57.531, Cycles 4,582,209,307,546, I/O Priority Normal, Memory Priority 5, Ideal Processor 2); the main window's "Processes: 487" counter is also circled.*

---

## Process Address Space

The memory of a **single-threaded process** is (usually) split into 5 sections:

- **Code** (or Text) – Instruction to execute
- **Data** – Static data compiled into executable file
  - Hard coded strings…
- **Heap** – Manually managed memory
  - malloc, new …
- **Stack** – Automatically managed memory
  - Local variables allocation
  - Function (Call Stack)
- **Metadata** – OS data about the process

*Figure: vertical memory map running from `0x0` at the top to `0xFF…` at the bottom, in order: Code (or Text), Data, Heap (growing downward), free space, Stack (growing upward), OS Metadata.*

---

## That's it for the intro…

Next, on **Operating Systems**:

- Multi-threading
- Stacks for everyone – Stacks in Multithreaded process
- IPC – Inter-process communication
- Shared Memory & Message Passing
- Critical Section
- Synchronization

---

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

---

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

---

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

---

# Memory and Virtual Memory

> Study notes converted from the lecture slide deck `Presentation 5 - Memory - 1 Slide-2.pdf` (129 pages) — Operating Systems, Reichman University, Spring Semester 2026.
> Lecturer: Tsvi Cherny-Shahar · TAs: Daniel Karalnik, Liam Tal.
> Faithful conversion: every substantive item on the source pages appears here, and nothing has been added from outside them. Figures are rendered as italic *Figure:* descriptions.
> `<!-- source pages a-b -->` comments mark the original page ranges.

---

<!-- source pages 1-14 -->

## Memory — Presentation 5

*Figure: Title slide with a thought bubble containing the word "Memory", beside a CommitStrip comic: a developer is told by Chrome that it is having trouble opening new tabs and that the IDE, Skype and everyone else have problems too; the developer decides to boost the IDE, hands Chrome "an extra 4 gigs" to share around ("there aren't any more slots left"), Chrome goes off and eats it all ("Nom nom nom"), then tells the waiting IDE, Skype and NetBeans "Burp! Um… He told you to get lost" — a joke about one process hogging all of physical memory.*

### Acknowledgments

Thanks to:

- Prof. Anat Bremler-Barr
- Prof. Gadi Taubenfeld
- Dr. Guy Katz and Prof. David Hay

Also, resources from:

- OS/161 Instructional Operating System (os161.org)
- ops-class.org

---

## Physical Memory

For physical memory in memory tapes **to access a random address**, it had to **physically roll the tape** to that position.

*Figure: Photographs of a Sony audio cassette tape and a technician mounting a reel of tape on a UNIVAC tape drive — illustrating sequential-access storage.*

In 1947, **Frederick Viehe,** an amateur inventor wrote (and patented) the concept of the first **Random Access Memory (RAM)**.

RAM can access any random memory address in the same time.

In 1949, **Dr. An Wang,** created independently a different concept of RAM and patented it.

In 1956, IBM purchased the rights for Wang's design for 500,000$.

*Figure: Portrait photograph captioned "An Wang 1920-1990".*

In 1951, MIT researchers lead by **Jay Forrester** also designed independently a RAM.

**But also** created the first implementation of a RAM

Only in the 70's RAMs became cost-effective and commercially available, mainly Intel's DRAM chip

Its usage expanded in the 80's with the emerge of the Personal Computer (PC)

*Figure: Photograph captioned "Jay Forrester 1918-2016", holding a framed core-memory plane.*

---

## Types of volatile memory

| Type | Refresh State | Speed | Cost | Usually Used In… |
|---|---|---|---|---|
| **Dynamic RAM (DRAM)** | Rewrite the data every 10ms-100ms | Slowest | Cheapest | Main memory (older) |
| **Static RAM (SRAM)** | Remembers as long as there's power | Fastest | Most expensive | CPU caches (L1, L2, L3) CPU registers |
| **Synchronous Dynamic RAM (SDRAM)** | Like DRAM but refreshed in sync with the clock (~64ms) | Faster than DRAM due to its sync with the clock | More expensive than DRAM | Main memory |

---

## Memory in MS-DOS Era (~80s)

MS-DOS designed for 1MB of physical memory

$2^{20}$ Address space

If less memory was installed, DOS used the "conventional memory" for devices and others, leaving less memory for applications

| Memory Type | Range | Use |
|---|---|---|
| Conventional | 0–640KB | Main program memory in DOS + a bit taken by the OS |
| Upper Memory Area (UMA) | 640KB–1MB | Device drivers, Video… |

That means applications has **a bit less than 640k! That is it!**

*Figure: Screenshots of 1980s DOS-era software — the game Alley Cat by Bill Williams, a Norton-style two-pane DOS file manager, a platform/adventure game, Wolfenstein 3D, the MS-DOS Executive window listing .PIF files, and a Lotus-style text-mode spreadsheet — showing what fit in under 640k.*

### Beyond the 1MB limit

If you purchased more than 1MB of memory, you can't use it!
It is out of the address space!

But applications get more demanding and require more memory…

So, memory that is **beyond** the 1MB, is called "Extended Memory" – but how apps can use it?

This is where memory managers comes in… to go **beyond the 640k**

The different techniques to get the same goal:
Extended Memory, Expanded Memory

The **application** had to choose which of them to use!

| Memory Type | Range | Use |
|---|---|---|
| Conventional | 0–640KB | Main program memory in DOS |
| Upper Memory Area (UMA) | 640KB–1MB | Device drivers, Video… |
| High Memory Area (HMA) | 1024KB–1088KB | The first 64KB beyond 1MB (HMA) |
| Extended Memory | >1MB (linear) | Used by XMS (e.g., HIMEM.SYS) |
| Expanded Memory | Bank-switched | EMS (via EMM386 or QEMM) |

### Extendable memory - XMS (HIMEM.sys)

- Setting in config.sys `DOS=HIGH` placed some of the memory used for kernel in conventional memory, to the **High Memory Area (HMA).**
  It is the first 64kb beyond the 1MB.
- **HIMEM.sys** provides **XMS API**, which gave access to the additional memory. But HOW? It's beyond the address space?
- **XMS** returns a *HANDLE* (like ID) to the memory requested, and the programmer can use XMS API to copy to/from the handle to the conventional memory

### Expandable memory – EMS and Bank Switching

- **Requires** hardware support based on LIM EMS specification
  - **L**otus-**I**ntel-**M**icrosoft
  - Lotus was a (very) successful spreadsheet company
- Maps a 16kb **"page"** (or bank) of the additional memory into one of the four available slots in the EMS frame (which is 64kb)
- **EMS frame** is usually at the UMA
- **Faster** than XMS

### Result

Using XMS and EMS we can run much better applications

*Figure: Screenshots of more advanced early-90s software — Windows File Manager and Program Manager, Doom, Civilization, a Monkey Island-style adventure game, and a 3D CAD wireframe house editor — showing what became possible with extended/expanded memory.*

---

<!-- source pages 15-30 -->

## From Single Program to Multitasking

*Figure: a framed pixel-art picture of a green computer-chip character wearing a crown and holding a "640K" sign, captioned "640k King" (Source: ChatGPT).*

For a time, the single program ruled the digital world — **the 640k king!**

*Figure: a framed pixel-art picture of three people protesting with signs reading "MORE PROGRAMS!", "MORE SPREADSHEETS!" and "MORE GAMES!".*

But time went by, and the world have changed… and people, once modest, started to demand more!

- More programs!
- More spreadsheets!
- More Games!

**All of them at same time!**

*Figure: a framed pixel-art picture of an hourglass whose sand is draining below a RAM stick, marking the passage of an era.*

And so, the age of the single-process gave way… **and the age of multitasking has begun!**

*Figure: a framed pixel-art picture of a small green floppy-disk character standing before a throne room full of crowned application characters.*

So how does, once lone program, fit in the court of applications?

---

## Physical Memory

The following sequence loads processes directly into one flat block of physical memory, step by step.

*Figure: throughout the sequence, physical memory is drawn as a single wide rectangle; each process is a colored block occupying a contiguous horizontal region of it, and white regions are free memory.*

1. **Empty physical memory** — one empty rectangle, all free.
2. **Calc loads** — a narrow blue block labelled `Calc` at the left edge; the rest of memory is free.
3. **Browser loads** — a large green block labelled `Browser` placed immediately after `Calc`; a smaller free region remains at the right.
4. **Calc terminates** — the blue `Calc` block disappears, leaving a free hole at the left; `Browser` stays where it is, with free space on both sides.
5. **IDE wants to load** — a purple block labelled `IDE` (with a worried face) is drawn superimposed on the memory rectangle, straddling the left free hole: it begins to the left of the rectangle's left edge, covers the whole left hole, overlaps onto the green `Browser` block and spills below the rectangle's bottom edge. The superposition shows directly that it is bigger than the hole — it does not fit in the free space left of `Browser`, and it does not fit in the free space right of `Browser` either.

### Looks like we ran out of memory…

Maybe we can split the memory?

6. **IDE is split** — the purple `IDE` block is placed in *two* pieces: one piece in the free hole to the left of `Browser`, one piece in the free region to the right of `Browser`. Two thin white slivers remain — one between the left `IDE` piece and `Browser`, one at the far right edge, after the right `IDE` piece. Both slivers are labelled with arrows pointing at them:

*Figure: memory now reads left-to-right as `IDE` | tiny free sliver | `Browser` | `IDE` | tiny free sliver, with two arrows from the slivers meeting at the caption "External fragmentation".*

---

## Fragmentation

**External Fragmentation** is free memory space **between processes**

**Internal Fragmentation** is free memory space **inside a process**

---

## Physical Memory (continued)

7. **Terminal wants to load** — an orange block labelled `Terminal` (with a worried face) is drawn outside the memory rectangle. Memory currently holds `IDE` | sliver | `Browser` | `IDE` | sliver. `Terminal` says:

   > There's not enough memory! Even if you split me!

8. **IDE terminates….** — both purple `IDE` pieces disappear. Memory now holds a free hole at the left, `Browser` in the middle, and a large free region at the right. `Terminal` (now with a happy face) is still waiting outside.

9. **Terminal loads, Word wants to load** — `Terminal` is placed as an orange block in the region immediately right of `Browser`, leaving a small free sliver at the far right edge; a free hole remains at the left of `Browser`. A blue block labelled `Word` (surprised face) containing `int64_t[5000]` is drawn outside the memory rectangle.

10. **Word cannot be split** — `Word` (now with an angry face) says:

    > You can't split me! I need 5000 **continuous** 64bit ints!

**Looks like we'll need a different approach…**

---

<!-- source pages 31-46 -->

## Virtual Memory

Ideally, we want to give **every process** the **entire** memory space.
How much memory do we need?

Each process has the **entire memory space** to itself:

- **In 32bit OS**: 4 bytes $\rightarrow 2^{32} = 2^{2} \times 2^{30} = 4_{GB}$  ($2^{30} = 1G$)
- **In 64bit OS**: 8 bytes $\rightarrow 2^{64} = 2^{4} \times 2^{60} = 16_{EB}$ ($2^{60} = 1Exa$)

In current 64bits OS usually uses "only" 48 bits:

$$2^{48} = 2^{8} \times 2^{40} = 256_{TB}$$

**for each process** (shared by user and kernel)

(and in some OS its even smaller… but… more than enough)

### How much memory do I have?

```powershell
PS C:\> systeminfo | findstr /C:"Total Physical Memory"
Total Physical Memory:     32,492 MB
PS C:\>
```

*Figure: A PowerShell screenshot reporting 32,492 MB of physical memory, followed by a confounded-face emoji.*

### Adding a level of indirection

So… Let's add another level of indirection (i.e. abstraction layer)

Let's give **each process the entire address space of virtual addresses** and let the OS **translate** the *virtual address* $\rightarrow$ *physical address*

*Figure: A debugger hover showing pointer `p` holding `0xd8bc3ffc7c` with `*p = 5`, produced by the code `int x = 5; int* p = &x;`. An arrow labelled "NOT A "REAL" ADDRESS!" points at `0xd8bc3ffc7c` — the value the program sees is a virtual, not physical, address.*

### Virtual memory layout

Important locations in the memory, can be predefined (e.g., code, heap)

**Not everything** can be predefined (dynamic libraries: .dll, .so, .dylib)

**Layout** predefined by the OS executable data structure (PE in Windows, ELF in Linux, Mach-o in MacOS)

*Figure: A vertical address-space bar, top to bottom: Code, Data, Heap (fading downward into free space), a large unused gap, and Stack at the bottom.*

---

## Virtual Memory - Types

**Anonymous memory** – Memory allocated with malloc

- `malloc, stack, mmap, HeapAlloc, CreateFileMapping`
- In many OSes, we can name the allocated memory
  - `shm_open, CreateFileMapping`
  - In some cases, the OS can tell a memory is stack or heap and name them

**File backed memory** – A file loaded into the memory

- `mmap, CreateFileMapping`
- File containing serialized executable data structure (PE, ELF)

### Virtual Memory - Anonymous

Memory does not have to be mapped to physical memory immediately, but only when used.

```c
int x[1000] // In virtual memory, might not be mapped to physical
x[3] = 5 // Now it is placed in physical (not the necessarily the whole array)
```

### Virtual Memory – File-Backed

**Buffered I/O** – Loads chunks of the file to chunks of memory (e.g. `fread`)

**Memory-Mapped I/O** – Loads the file directly into memory, allowing RAM-like access to the file (e.g., `mmap, CreateFileMapping`)

- We can write changed in the memory back to the file
  - (msync, FlushViewOfFile)
- Loading PE/ELF (i.e. executable) uses memory mapped I/O

#### File-Backed terminology

**Resident Memory** – Memory loaded into physical memory

**Dirty** – Modified memory (since its load)

**Evict/Revoke** – Memory loaded into physical but removed

- If not dirty memory – already in the file, simply remove from physical

**SWAP** – A file that stores:

- Evicted Anonymous (or named) memory
- Evicted dirty memory

---

## Virtual Memory - Accessibility

Share (or not) virtual memory between processes:

**Private** – Only "mine" (malloc, stack etc.)

**Shared** – Virtual memory can be mapped to multiple processes:

- Loaded dynamic libraries
- mmap, CreateFileMapping
- *Copy-on-write (COW)* – memory is shared, until a **write** is made
  - Write makes the OS copy the memory and make it private

## Virtual Memory – Access Control

If there's an abstraction layer, we can add feature to it, like Access Control:

- **Read** – **load** from address is allowed
- **Write** – **store** to address is allowed
- **Execute** – **set IP** to this address is allowed

In case of violation, OS raises an exception (can lead to termination)

---

## Virtual Memory - pmap

The full mapping listing for the `bash` process with PID 13:

```console
tcs@tc-pc:~$ pmap -xx 13
13:   -bash
Address           Kbytes     RSS   Dirty Mode  Mapping
0000557414ad4000     188     188       0 r---- bash
0000557414b03000     892     884       0 r-x-- bash
0000557414be2000     232     120       0 r---- bash
0000557414c1d000      16      16      16 r---- bash
0000557414c21000      36      36      36 rw--- bash
0000557414c2a000      44      28      28 rw---   [ anon ]
0000557414f50000    1644    1548    1548 rw---   [ anon ]
00007fb7297e1000     348     128       0 r---- LC_CTYPE
00007fb729838000       4       4       0 r---- LC_NUMERIC
00007fb729839000       4       4       0 r---- LC_TIME
00007fb72983a000       4       4       0 r---- LC_COLLATE
00007fb72983b000       4       4       0 r---- LC_MONETARY
00007fb72983c000       4       4       0 r---- SYS_LC_MESSAGES
00007fb72983d000       4       4       0 r---- LC_PAPER
00007fb72983e000      12       8       8 rw---   [ anon ]
00007fb729841000     160     160       0 r---- libc.so.6
00007fb729869000    1620    1224       0 r-x-- libc.so.6
00007fb7299fe000     352     192       0 r---- libc.so.6
00007fb729a56000       4       0       0 ----- libc.so.6
00007fb729a57000      16      16      16 r---- libc.so.6
00007fb729a5b000       8       8       8 rw--- libc.so.6
00007fb729a5d000      52      24      24 rw---   [ anon ]
00007fb729a6a000      56      56       0 r---- libtinfo.so.6.3
00007fb729a78000      68      68       0 r-x-- libtinfo.so.6.3
00007fb729a89000      56      52       0 r---- libtinfo.so.6.3
00007fb729a97000      16      16      16 r---- libtinfo.so.6.3
00007fb729a9b000       4       4       4 rw--- libtinfo.so.6.3
00007fb729a9c000       4       4       0 r---- LC_NAME
00007fb729a9d000       4       4       0 r---- LC_ADDRESS
00007fb729a9e000       4       4       0 r---- LC_TELEPHONE
00007fb729a9f000       4       4       0 r---- LC_MEASUREMENT
00007fb729aa0000      28      28       0 r--s- gconv-modules.cache
00007fb729aa7000       8       8       8 rw---   [ anon ]
00007fb729aa9000       8       8       0 r---- ld-linux-x86-64.so.2
00007fb729aab000     168     168       0 r-x-- ld-linux-x86-64.so.2
00007fb729ad5000      44      40       0 r---- ld-linux-x86-64.so.2
00007fb729ae0000       4       4       0 r---- LC_IDENTIFICATION
00007fb729ae1000       8       8       8 r---- ld-linux-x86-64.so.2
00007fb729ae3000       8       8       8 rw--- ld-linux-x86-64.so.2
00007ffead92f000     136     112     112 rw---   [ stack ]
00007ffead9c9000      16       0       0 r----   [ anon ]
00007ffead9cd000       8       4       0 r-x--   [ anon ]
---------------- -------- -------- --------
total kB            6300    5204    1840
```

### Reading a pmap line — anonymous mapping

The line `0000557414c2a000  44  28  28 rw---  [ anon ]` (highlighted in the output):

- Virtual Memory starts at `0x557414c2a000`
- Size – 44k
- Resident (in physical) - 28k
- Dirty – 28k
- Permissions – Read/write
- Mapping – Anonymous (not a file)

Probably allocated heap or stack

### Reading a pmap line — file-backed mapping

The line `0000557414b03000  892  884  0 r-x-- bash` (highlighted in the output):

- Virtual Memory starts at `0x557414b03000`
- Size – 892k
- Resident (in physical) - 884k
- Dirty – 0
- Permissions – Read/Execute
- Mapping – bash executable

Probably the code section of bash

---

## Virtual to Physical

We need to map virtual addresses to the physical address.

How to translate the *virtual address* to its mapped *physical address?*

### A first (bad) idea: ask the kernel

```
store(0x800…023, 4)
```

Process can ask the kernel "can I get the address of 0x8000 … 023" ?

The kernel (i.e., OS) performs the translation and returns the physical address

### Why that does not work

- It is unsafe, we let the user-mode process actively initiate, or be part of the translation workflow.
- Besides, we don't want to let user-mode "see" through the abstraction (meaning, "see" physical addresses)
- Also, the kernel has to translate **for each memory access.**

Too slow! We need something faster… Maybe the hardware can help?

---

<!-- source pages 47-60 -->

## Memory Management Unit (MMU) – 90s

*Figure: Hardware block diagram. A "CPU package" box contains the CPU, the Memory Management Unit, and a Cache (TLB - Translation Lookaside Buffer) attached to the MMU. An arrow labelled "The CPU sends virtual addresses to the MMU" points from the CPU down into the MMU; an arrow labelled "The MMU sends physical addresses to the memory" points from the MMU down to the Bus. Separate "Memory" and "Disk controller" boxes also hang off the Bus.*

- The CPU sends **virtual addresses** to the MMU.
- The MMU sends **physical addresses** to the memory.
- The MMU is backed by a cache: the **TLB (Translation Lookaside Buffer)**.

---

## Virtual to Physical: mechanism vs. policy

MMU is a (hardware) mechanism that translates and access the physical memory, but it **does not** set the **policy:**

- Which address is "read only"?
- Which address belongs to which process?
- etc.

The **OS** sets the **Policy**, **MMU** is the **executing mechanism**.

---

## First try…

*Figure: Walk-through diagram. On the left, a "User Process" box and a "Kernel" box; in the middle a "CPU" box feeding down into an "MMU" box, which has a small "Cache" box attached below it; on the right a tall "Physical Memory" column. Each step below adds one arrow or label to this picture.*

The translation attempt proceeds step by step:

1. The **User Process** issues address `0x5000` to the **CPU**.
2. The **CPU** passes `0x5000` down to the **MMU**.
3. The MMU: *"I can't find it in the cache!"*
4. The MMU raises an exception to the **Kernel**: *"Exception! I can't find 0x5000! Where is it?"*
5. The Kernel answers the MMU: *"Oh, its at 0x8453"*.
6. The **Cache** now holds the mapping `0x5000 → 0x8453`.
7. The MMU accesses **Physical Memory** at the location marked `0x8453`.

The scenario then repeats with a different address:

8. The **User Process** issues address `0x10000` to the **CPU** (the cache still holds only `0x5000 → 0x8453`).
9. The **CPU** passes `0x10000` down to the **MMU**.
10. The MMU raises an exception to the **Kernel**: *"Exception! I can't find 0x10000! Where is it?"*
11. The Kernel answers: *"WHAT?! You are not allowed to access 0x10000!"*
    - **Access Violation** E_ACCESSVIOLATION (win)
    - **Segmentation Fault** SIGSEGV (*nix)

*Figure: at the last step a red alarm-siren icon is drawn next to the Kernel box, marking the illegal access.*

### Why this first try is not good enough

We don't want to access kernel every time… **its slow!**

Let's try to improve that with some extra-data…

---

<!-- source pages 61-71 -->

## Base and Bound

**UNIVAC EXEC II (1962)**
**IBM System/360 (late 60s)**

Assign for **each process**:

- **Base** (physical) address
- **Bound** offset (from the base)

Together they set the process memory bounds.

Instead of returning the physical address, return the base and bound.

### Walkthrough

*Figure: the same block diagram is advanced step by step — User Process → CPU → MMU → Cache, with the Kernel off to the side and a Physical Memory column on the right.*

1. The CPU issues address `0x5000` down to the MMU. The MMU has nothing for it and raises an exception to the Kernel: *"Exception! I can't find 0x5000! Where is it?"*
2. The Kernel answers the MMU with **Base 0x20000**, **Bound 0x10000**.
3. The MMU sends `0x25000` on to physical memory, reaching the process's region **P**, which spans `0x20000` to `0x30000`. The pair `P: Base 0x20000, Bound 0x10000` is now held next to the Cache.
4. Next the CPU issues `0x15000` to the MMU, with `P: Base 0x20000, Bound 0x10000` still cached.
5. The MMU checks the bound: `0x15000 > 0x10000`.
6. The MMU raises an exception to the Kernel: *"Exception! Access violation!"*

### Trade-offs

While it is **fast** (1 op to check, 1 op to translate), the memory must be continuous!

Also, suffers from

- **internal** fragmentation
- **external** fragmentation

*Figure: a physical memory column holding two whole processes P (top and bottom) and, in the middle, one process laid out as Code / Data / Heap / Stack. Red arrows from "external fragmentation" point at the white gaps between and around the P blocks; yellow arrows from "internal fragmentation" point at the unused space inside the middle process's own region (between Data/Heap and between Heap and Stack).*

---

## Segmentation

**Multics (late 60s)**
**Windows 3.x (80s)**

Base and Bound can be extended: Split the process into **Segments**.

Each segment holds either "code, heap, stack etc." segment.

The **segmentation** helps **reduce** the **external** fragmentation, but the **internal** fragmentation remains.

*Figure: a physical memory column whose slots, top to bottom, are: (gap), Code P1, (gap), Stack P1, Code P2, Data P2, Heap P1, Heap P2, Stack P2, Data P1, (gap) — segments of two processes interleaved freely rather than kept in one contiguous block each.*

---

## Content Addressable Memory (CAM)

CAM memory (hardware) lookups in an entire table in **one clock cycle**.

- **Binary CAM** – matches 0/1
- **Trinary CAM** – matches 0/1/\*

Pros: Fast

Cons:

- **Very** Expensive
- Higher energy consumption

common in CPU and Routers

*Figure: a "Pattern →" box feeds a key table and a "Match →" box comes out of it, illustrating a single-cycle whole-table lookup.*

| Keys | Values (Phy) |
| --- | --- |
| 00101001 | 111111 |
| 11010101 | 001011 |
| 01010110 | 000010 |

### CAM size

What is the CAM size? Depends on your computer.

- **Small/cheaper** – 32 to 64 *fast* entries
- **Regular** – 64 to 128 *fast* entries
- **High end PCs** – 128 to 256 *fast* entries
- **Industrial (servers)** – 256 to 1 MB *fast* entries
- **Really expensive** – 2MB to 1GB *fast* entries

---

<!-- source pages 72-87 -->

## Translation Lookaside Buffer (TLB) — Improving the cache

Assign a TLB for every core; it maps VA $\rightarrow$ Physical.

| VA | Physical |
|---|---|
| 0xPID_1230 | 0x0456 |
| 0xPID_374A | 0x7593 |
| 0xPID_840F | 0xAB45 |

Search `0x(PID_1230)`
Finds $\rightarrow$ `0x0456`

Prepending the PID is often called "tagged" CAM.

- If an entry is not found, ask the kernel (exception)
- If kernel doesn't find $\rightarrow$ segmentation fault

The problem is that CAMs are not infinite.

Caching virtual addresses individually causes many **cache-miss** – entry not in cache.

Instead, of caching individual address, **let's group them in Pages of addresses.**

---

## Pages (+MMU modern virtual memory)

Page is a **continuous** chunks of memory of a **specific** process.

Pages of **a process** are stored in **Page Table**.

**Every process** has a **Page Table** (accessible from the PCB).

### Page Table Entry

Each page has its **Page Table Entry (PTE)** which contains:

- **Physical Page Number (PPN)**
- **Validity** – Is currently mapped to physical memory
- **Permissions** (Read/Write/Execute)
- **Accessibility** (shared/private)
- **Access** – Was the page accessed or not?
- **Dirty** – Was it modified?

And more…

### Where page tables live

Page tables are **created** by the OS and are **stored in physical RAM**.

**Page Table Base Register (PTBR)** holds **physical address** of the page table for the <u>current process</u>.

Assume the page table is **always** in the RAM ($\leftarrow$ **not true**).

**MMU can access** the page tables **directly** (without the OS) (through there to the relevant PTEs).

### Splitting the virtual address

Assume a **page size is 4k**.

Virtual Address has 64 bits (in 64bit OS).

$2^{12} = 4096 \rightarrow$ 12 bits are **offset** inside the page.

- The **last** 12 bits in the VA are the **offset** *within* the page
- The rest of the **top bits** (64-12=52) are the **Virtual Page Number (VPN)**

`0x8000 1234 5678 90ab` — here `8000 1234 5678 9` is the VPN and `0ab` is the offset.

---

## Translation — TLB hit

The example uses the virtual address `0x8000 1234 5678 90ab`, split into VPN `0x8000 1234 5678 9` and offset `0ab`.

TLB contents:

| VPN | PPN |
|---|---|
| 0x8000 1234 5678 9 | 0x6573 3293 5842 8 |
| 0x6234 ABCD EF0 0 | 0x3829 5837 A57F F |
| 0x1DDD AAAA CCCC 2 | 0xBCEA 38D7 BEEF |

*Figure: physical memory is drawn as a column of 4k frames; PTBR points at a frame at the very top (the page table), and three occupied frames are labelled 0x39…, 0x65… and 0xBC…, with the 4k frame size marked by a brace.*

Steps of the translation:

1. The VA `0x8000 1234 5678 90ab` is split into VPN (`8000 1234 5678 9`) and offset (`0ab`).
2. The VPN is looked up in the TLB and matches the first row, whose PPN is `0x6573 3293 5842 8`.
3. The PPN is concatenated with the offset, giving the physical address `0x6573 3293 5842 80ab`, which points inside the physical frame labelled `0x65…`.

---

## Translation – TLB-Fault

Now the access is to virtual address `0x9000 8765 123C 9EEE` (VPN `0x9000 8765 123C 9`, offset `EEE`).

The VPN is not present in any TLB row — "?".

---

## Page Table as Array

Page table (holding the PTEs) of current process are stored in an array

```c
PTE page_table[MAX_PAGES]
```

Page table base register (in the CPU) holds:

```c
PTBR = page_table
```

The **index** of `page_table` is the virtual page number (**VPN**).

Access to PTE is $O(1)$.

---

## Translation — TLB miss walk and page fault

Continuing with `0x9000 8765 123C 9EEE`, the TLB still holds only:

| VPN | PPN |
|---|---|
| 0x8000 1234 5678 9 | 0x6573 3293 5842 8 |
| 0x6234 ABCD EF0 0 | 0x3829 5837 A57F F |
| 0x1DDD AAAA CCCC 2 | 0xBCEA 38D7 BEEF |

Steps:

1. The VA `0x9000 8765 123C 9EEE` is presented; its VPN is not in the TLB.
2. **TLB miss** – not in TLB! Find the relevant PTE and get the PPN.
3. The page table array is indexed by the VPN:

   ```c
   page_table[VPN (0x9000 8765 123C 9)]
   ```

   with `PTBR = &page_table[MAX_PAGES]`.

   *Figure: the page table is drawn as a long purple array starting at the address held by PTBR; one cell inside it is highlighted as the PTE, showing its PPN and Valid fields.*

4. The PTE contains:
   - PPN
   - Valid Bit
   - …
   - …

   If Valid bit is 0, page is not in physical memory. **Page Fault!**

   **Raise Exception** using the CPU to the kernel.
   **OS loads the memory** from the file/swap to the physical memory into a free frame.
   **OS updates** the PTE with the correct PPN.

5. The resolved mapping is inserted into the TLB as a new row:

| VPN | PPN |
|---|---|
| 0x8000 1234 5678 9 | 0x6573 3293 5842 8 |
| 0x6234 ABCD EF00 0 | 0x3829 5837 A57F F |
| 0x1DDD AAAA CCCC 2 | 0xBCEA 38D7 BEEF |
| 0x9000 8765 123C 9 | PPN… |

*Note: the second VPN is written `0x6234 ABCD EF0 0` on the earlier slides of this sequence and `0x6234 ABCD EF00 0` on the last one; transcribed as printed.*

---

<!-- source pages 88-105 -->

## TLB entry removal

A TLB entry is removed on:

- TLB is full
- Page eviction
- (older architectures) on **process** context switch
- Page mapping
- In ARM – **TLBI** instruction, removes range of addresses or levels
- More…

---

## Page Table Data Structure

Page table as array **must** be continuous!

Not continues $\rightarrow$ the other extreme – Linked List.
Not continuous, but search is O(n)

We can try to meet in the middle…
**Multi-level page tables**

---

## Multi-level page table

Assume we are using 4 levels page table:

Split the VPN to 4 parts $\rightarrow$

$$\frac{52}{4} = 13_{bits} = 2^{13} = 8192_{bytes} = \frac{2^{13}}{sizeof(PTE)=8} = \mathbf{1024}_{entries}$$

*Note: in the original slide a stray “3” is rendered next to the denominator `sizeof(PTE)=8` (apparently $8 = 2^3$).*

Size of PTE (usually) 64bit:

- **0-11 – flags**
- **12-64 – PPN**
- (actually, PPN is smaller, based on how much RAM I have)

### Splitting the VPN

Address: `0x 9000 8765 123C 9`

Binary: `1001 0000 0000 0000 1000 0111 0110 0101 0001 0010 0011 1100 1001`

The 52 bits are cut into four 13-bit parts (the colouring on the slide cuts mid-nibble):

| Part (13 bits) | Value | Level |
|---|---|---|
| 1st | `0x1200` | $L_4$ |
| 2nd | `0x043D` | $L_3$ |
| 3rd | `0x0511` | $L_3$ |
| 4th | `0x13C9` | $L_1$ |

*Note: the third part is labelled $L_3$ on the slide; from the surrounding slides it should be $L_2$.*

Each part is the ***index*** of the $L_i$ level.
$L_1$ contains the PTEs, all others point to lower level. PTBR points to $L_4$.

### Walking the levels

*Figure: a staircase of four tables. PTBR points into the $L_4$ table at index `[0x1200]`; that entry's arrow goes up to the $L_3$ table, indexed at `[0x043D]`; that arrow goes up to the $L_2$ table, indexed at `[0x0511]`; that arrow goes up to the $L_1$ table, where the entry at `[0x13C9]` is the PTE itself.*

Performance - $c$

where $c$ is the number of levels

**Notice,** there are multiple tables in $L_3, L_2$ & $L_1$

### Paging multi-level page table

Once the page table is “broken” into parts,
we can **evict** most of the table and **load** it with **page fault** when required.

1. $L_4$ (i.e. the lowest level) **must be** in the **physical memory** at all time
2. Every entry in $L_i$ is a PTE of the node in the upper level $L_{i-1}$
3. $L_1$ holds the PTEs for the table itself

*Figure: the same four-level staircase, with the $L_4$ entry at `[0x1200]` expanded into a "PTE $L_3$ node" box containing PPN and Valid Bit, and the address of the next entry given as the formula below.*

$$L_3\,PPN + (0x43D \times sizeof(PTE))$$

### Hard-wiring

As the MMU reads the page table, the structure of the Page table is hard-wired to the MMU

With that, it is configurable (to the extent of the hardware specifications) using control registers in the CPU

---

## SWAP in/out a page

When **running out of physical memory**, the OS can remove (swap out) pages to the disk, to a “swap file” (`/dev/sdb` in Ubuntu, `c:\pagefile.sys` in Windows)

Swapping out also occurs as “maintenance”, while computer is idle

During a **page fault**, the OS loads (swap in) the page into the physical memory

### SWAP out a page — step by step

*Figure: three panels — a Disk box containing a Swap file box on the left, a VPN→PPN table in the middle with the PTE box below it, and a Physical Memory column on the right holding Page1 “data1”, Page2 “data2”, Page3 “data3” and one empty frame.*

Starting state:

| VPN | PPN |
|---|---|
| 0x8000 1234 5678 9 | 0x6573 3293 5842 8 |
| 0x6234 ABCD EF00 0 | 0x3829 5837 A57F F |
| 0x1DDD AAAA CCCC 2 | 0xBCEA 38D7 BEEF |

PTE:

- PPN = 0x382…
- Valid Bit = 1

Physical memory: Page1 “data1”, Page2 “data2”, Page3 “data3”, plus one highlighted empty **Frame**: Page size free space.
Swap file: empty.

The steps:

1. **Write the page to disk.** “data2” is copied into the swap file. The table entry `0x6234 ABCD EF00 0 → 0x3829 5837 A57F F` is cleared (the row becomes blank). Physical memory still holds Page2 “data2”; the PTE is still PPN = 0x382…, Valid Bit = 1.
2. **Free the frame.** Page2 “data2” is removed from physical memory — its frame becomes empty. The PTE still reads PPN = 0x382…, Valid Bit = 1. Swap file holds “data2”.
3. **Invalidate the PTE.** PTE becomes:
   - PPN = (empty)
   - Valid Bit = 0

### SWAP in a page — step by step

Starting state (the end state of the swap out): swap file holds “data2”; the middle table row is blank; physical memory holds Page1 “data1”, an empty frame, Page3 “data3” and an empty frame; PTE has PPN = (empty), Valid Bit = 0.

The steps:

1. **Load the page into a free frame.** Page2 “data2” is placed into the free frame at the bottom of physical memory (the frame it originally occupied stays empty). The PTE is updated:
   - PPN = 0xABC…
   - Valid Bit = 1

   The swap file still contains “data2”.
2. **Refill the table entry.** The row for `0x6234 ABCD EF00 0` is restored with the new PPN:

   | VPN | PPN |
   |---|---|
   | 0x8000 1234 5678 9 | 0x6573 3293 5842 8 |
   | 0x6234 ABCD EF00 0 | 0xABCD EEEE FFFF C |
   | 0x1DDD AAAA CCCC 2 | 0xBCEA 38D7 BEEF |

3. **Free the disk copy.** “data2” is removed from the swap file (the swap file box becomes empty).

In file-backed memory the data on the disk is **not** removed.

### Pre-allocation of the swap file

To speed up the swap out, the SWAP file pre-allocates space on the disk

Therefore, SWAP files are quite large with space available (at their location in the file)

*Figure: a Windows "Performance Options" dialog, Advanced tab — Processor scheduling ("Adjust for best performance of:" with Programs selected over Background services) and Virtual memory ("A paging file is an area on the hard disk that Windows uses as if it were RAM", Total paging file size for all drives: 3072 MB, with a Change… button).*

```shell
tcs@tc-pc:~$ swapon --show
NAME      TYPE       SIZE USED PRIO
/dev/sdb  partition    4G   0B   -2
tcs@tc-pc:~$ free
              total       used       free     shared  buff/cache  available
Mem:       16238968     360660   15738748       2408      139560   15643912
Swap:       4194304          0    4194304
```

---

<!-- source pages 106-117 -->

## Which page to evict?

Easiest to implement $\rightarrow$ random

But, what is the best page to evict?

The one we don't use!
Or at the least… **least recently used**.

---

## Least Recently Used (LRU) Cache

Every time element is used:

- If not in the LRU data structure:
    - If LRU is full:
        - Remove the last element (i.e. least used)
    - Add to the data structure at the front (i.e. most used)
- Else $\rightarrow$ move element to the front (i.e. most used)

### LRU cache with a linked list — worked trace

The cache holds at most $N = 3$ elements. The list runs from **front** (most recently used) at the top to **back** (least recently used) at the bottom.

*Figure: a vertical singly-linked list of coloured boxes, labelled "front" above the top box and "back" below the bottom box, with downward arrows from each box to the next; a purple arrow at the left points at the element currently being accessed.*

1. **Start:** the list contains two elements — front → `1` → `2` → back.
2. **Element `3` is used** (not in the list, list not full): it is added at the front → `3` → `1` → `2` → back. The list is now full ($N = 3$).
3. **Element `1` is accessed** (purple arrow points at `1`, the middle element). It is already in the list.
4. **Move `1` to the front** (most used): front → `1` → `3` → `2` → back.
5. **Element `1` is accessed again** (purple arrow points at `1`, now the front element).
6. **No change** — it is already at the front: front → `1` → `3` → `2` → back.
7. **Element `4` is used** — it is not in the list, and the list is full. `4` is shown to the side, outside the list, while the list still holds front → `1` → `3` → `2` → back.
8. **Evict the last element** (`2`, the least used): the list becomes front → `1` → `3` → back, with `4` still waiting outside.
9. **Add `4` at the front** (most used): front → `4` → `1` → `3` → back.

**Tip:**
As **searching** in linked list for the accessed element (to move to the front) **is O(n)**, usually, you would want to **have pointers** directly **to the elements.**

---

<!-- source pages 118-129 -->

## Clock LRU – approx. LRU

The memory is accessed at the speed of the clock cycles.

We need the **hardware** to update for changes.
The **MMU** updates the *reference* bit in the PTE.

If the MMU updates the LRU for any page access – it is too slow!

Where else do we know the page is accessed? **On new TLB entry**
Crude, as it does not distinguish between single and multiple access

When **adding a new TLB entry**, **set** its PTE *reference* bit to 1

### Data structure

Linked list?
Another large data structure, not efficient enough

So, we will use a **circular** data structure **of all the PTEs**.

A **finger pointer** iterates the PTEs and:

1. If the *reference* bit = 1 → set to 0 and continue
2. If the *reference* bit = 0 → evict

### Worked trace of the clock hand

*Figure: ten PTE boxes arranged in a circle (a clock face), each labelled with its reference bit; a black arrow (the finger pointer) starts pointing at the top box and sweeps clockwise.*

Reading the ring clockwise starting from the top box, the initial reference bits are:

$$1,\; 0,\; 1,\; 0,\; 1,\; 1,\; 0,\; 1,\; 0,\; 1$$

| Step | Finger points at | Its reference bit | Action |
|---|---|---|---|
| 1 | Top box (position 1) | 1 | Set to 0 and continue |
| 2 | Position 2 (next clockwise) | 0 | Evict — the box is removed from the ring |
| 3 | Position 3 | 1 | Set to 0 and continue |
| 4 | Position 4 | 0 | Evict — the box is removed from the ring |

After step 2 the ring is down to nine boxes; after step 4 it is down to eight, and the finger continues clockwise from there.

Between step 2 and step 3 the slide notes when the sweep resumes:

> If I need more pages, or on the next time I need to free

---

## Working Set Cache

TODO

---

## Levels of memory cache

*Figure: a stacked pyramid of memory levels with access latencies on the left. Inside a box labelled "Core" (which also contains an "MMU" block) sit Registers (0.3ns, drawn as a red triangle at the apex), L1 Cache (1ns) and L2 Cache memory (3-10ns). Below the core: L3 Cache memory — "(faster than physical, slower than upper levels)" — at 10-20ns, then Main Memory at 50-100ns, then Disk at 20-40$\mu$s.*

*Figure: on the left side, four downward arrows step through the hierarchy — L1 Cache → L2 Cache memory ("fallback"), L2 Cache memory → L3 Cache memory ("fallback"), L3 Cache memory → Main Memory (unlabelled), and a red "Evict" arrow from Main Memory down to Disk. On the right side, four upward arrows mirror them — a red "Page fault" arrow from Disk up to Main Memory, then Main Memory → L3 ("Cache Fill"), L3 → L2 ("Cache Fill") and L2 → L1 ("Cache Fill").*

| Level | Access time |
|---|---|
| Registers | 0.3ns |
| L1 Cache | 1ns |
| L2 Cache memory | 3-10ns |
| L3 Cache memory | 10-20ns |
| Main Memory | 50-100ns |
| Disk | 20-40$\mu$s |

### Multi-core view

*Figure: the same hierarchy with three Core boxes side by side, each containing its own MMU, Registers, L1 Cache and L2 Cache memory. All three cores share a single L3 Cache memory ("faster than physical, slower than upper levels"), which sits above the shared Main Memory, which sits above the Disk.*

---

## Levels of CAM TLB

Some systems provide multi-level TLB (usually two levels)

Differences between layers (usually two layer):

- Closeness to the MMU
- Runs at different frequency rate

For example:

- Intel core i9 / AMD Ryzen 9
  - ~128-256 entries TLB L1
  - ~1024-1536 entries TLB L2
- ARM Cortex-A53, Intel Pentium
  - 32-64 entries TLB L1
  - 128-256 entries TLB L2

---

## Optimization – don't alloc on malloc

When a program allocates space, we don't have to REALLY allocate memory, we can just allocate in the virtual memory.

Reduces the chance for evictions and future page faults.

Allocate only on real access to the memory

---

# File Systems

> Study notes converted from the lecture slide deck `OS 6 - Filesystems - 1 Slide.pdf` (102 pages) — Operating Systems, Reichman University, Spring Semester 2026.
> Lecturer: Tsvi Cherny-Shahar · TAs: Daniel Karalnik, Liam Tal.
> Faithful conversion: every substantive item on the source pages appears here, and nothing has been added from outside them. Figures are rendered as italic *Figure:* descriptions.
> `<!-- source pages a-b -->` comments mark the original page ranges.

---

<!-- source pages 1-15 -->

## Presentation 6 — File System

*Figure: Title slide showing a Windows "System Restore" dialog with an error popup — "The … Files (H:) has errors. Windows has detected file system corruption on … (H:). You must check the disk for errors before it can be restored." with a "Check the disk for errors" link — motivating the topic of file systems and their failure modes.*

### Acknowledgments

Thanks for material and support of:

- Prof. Anat Bremler-Barr
- Prof. Gadi Taubenfeld

Also, resources from:

- OS/161 Instructional Operating System (os161.org)
- ops-class.org

---

## Hard Disk Drive (HDD)

- Non-volatile
- Data stored on magnetic plats
- Data maintained without electrical outsource
- Moving Parts – Slower and less durable
- Cheaper (compared to flash memory)
- Larger memory
- Longer life-span (compared to flash memory)

### HDD components

**Platters** – layers of magnetic material on a metal disk

**Spindel** – Spins the platter 4200-15,000 RPM

**Head** – Read/Write the data from the platter

**Actuator** – Moves the head

**IDE connector** – IDE cable connects the data to the motherboard transferring 16 bits at a time

*Figure: Cutaway drawing of a hard disk labelling Platter, Spindle, Head, Actuator Arm, Actuator Axis, Actuator, IDE Connector, Jumper Block and Power Connector — showing where each component sits inside the drive enclosure. Alongside are photographs of a flat grey IDE ribbon cable and of the IDE connectors on a motherboard.*

**SATA connector (newer disks)** – Connects disk to motherboard and transfers bits **serially** but in much **higher transfer rate**

**Power connector** – Supplies power to the disk

**Jumper block** – Configure master/slave configuration for priority when multiple disks used a single IDE cable.
Newer disks can use jumper to specify disk configuration and backward compatibility

*Figure: Photograph of a red SATA data + power cable, and a photograph of a hand placing a small white jumper onto the pin block at the rear of a drive's circuit board.*

### HDD geometry

**Track** – a circle of magnetic layer on the platter. This is where the data is written

**Sector** – a Slice of the track in the size of 512-4096 bytes. The **head** read/write sectors. HDD minimum read/write size if a sector size

**Cylinder** – The vertical platter

*Note: "size if a sector size" appears in the original; presumably "is a sector size".*

*Figure: Two diagrams. Left: a top view of one platter with concentric rings labelled "Track/Cylinder" and a highlighted wedge labelled "Sector". Right: a side view of a stack of platters on a rotating spindle, labelling track $t$, sector $s$, cylinder $c$ (the same track across all platters), platter, arm, read-write head, actuator and the direction of rotation.*

---

## NAND/NOR Flash Memory

- Non-volatile
- Data stored on Floating Gate Transistors
  - By storing the electric charge in the transistor
- Data maintained without electrical outsource
- Limited read/write operations until wears off

---

## Solid State Disk (SSD)

- Uses *flash memory* to store data
- No moving parts
  - Faster random-access time (compared to HDD)
  - More durable
- Smaller compared to HDD
- Shorter life-span due to flash memory
  - Algorithms are used to "distribute" to increase life span
- Smaller capacity (compared to HDD)
- Expensive (compared to HDD)

### SSD organization

Has no "physical sectors"

The Flash memory is organized as into **pages** and **blocks** of pages
Typical **page size** is **4KB to 16KB** and **block size** at least **128 KB**

For compatibility, **SSD emulates sectors** which is translated to pages

This means, that **writing 512 bytes (sector) affects 4KB** (i.e. page size)

This incurs a **performance hit** (disk reads the page, modifies and writes again)

---

## File System

- Abstraction layer over the underlying disk hardware
- Instead of reading/writing in **sectors** create **abstraction** of **files and directories**
- Some file systems:
  FAT, FAT32, NTFS, EX4, XFS etc.

*Figure: Photograph of an open filing cabinet full of coloured hanging folders — the metaphor for the file/directory abstraction.*

---

## Sector? Block?! Cluster?!?!

- **Sector** is the smallest **physical storage** unit on disk
  - usually, **512B** or **4KB**
- **Block** (or **Cluster**) is the smallest **allocation unit** used by a file system — usually 1 or more sectors
- File systems **allocate space** in **whole blocks**, not bytes
- A file of size 1KB stored in a 4KB block wastes 3KB **(internal fragmentation)**

---

## File

- Stores persistently an array of bytes
- The file shouldn't change even due to power outage (not so easy…)

*Figure: Four screenshots of real-world corruption reports — "Major Windows 10 corruption after power outage" (JakeWolf1, created on April 22, 2020); "Greyed Out Files and Folders on Mac" (Owen Woods, September 25, 2024, MACOS); "Debian Bug report logs - #1057843 — linux: ext4 data corruption in 6.1.64-1"; and "massive filesystem corruption and failure after accessing windows 10 drive from windows 8.1" (Cybot, created on August 11, 2015).*

---

## File Allocation — Contiguous allocation

Each file occupies a run of consecutive blocks, described by a starting block and a length.

*Figure: A linear strip of blocks (each block = a "Block/Cluster of sectors") showing File A at the start, then File B (darker, blocks 4-6), then File C, then a gap marked "5 free blocks", then File E, then a gap marked "6 free blocks", then File G near the end.*

| file | start | length |
|------|-------|--------|
| A | 0 | 4 |
| B | 4 | 3 |
| C | 7 | 6 |
| E | 18 | 12 |
| G | 36 | 3 |

*Figure: The same block strip, now with a separate **7 blocks file** drawn below it and a thought bubble containing "?" — posing the open question of where such a file could be placed.*

### Pros and cons

Pros:

- Simple – File at starting index + length
- Small number of "seek" to find a file on disk
- No internal fragmentation

Cons:

- Change file size
- External fragmentation

**Used in:** CD-ROM, IBM OS/360

---

<!-- source pages 16-28 -->

## File Allocation — Linked List

*Figure: two linked chains of blocks. The upper chain has five blocks labelled "File block 0" … "File block 4", each with a pointer field at the top pointing to the next block; their physical blocks are 4, 7, 2, 10, 12, and the last block's pointer field holds 0. The lower chain, labelled "File B", has four blocks "File block 0" … "File block 3" at physical blocks 6, 3, 11, 14, with the last pointer field holding 0. Each file block stores the address of the next block, so 0 marks the end of the chain.*

**Pros:**

- Simple – File at starting index
- No external fragmentation
- File size can change easily

**Cons:**

- Random access is slow, requires multiple disk reading

**Used in:** IBM OS/360, MS-DOS

---

## File Allocation — File Allocation Table (FAT)

*Figure: the same two files as in the linked-list slide, but the "next block" pointers are pulled out into a separate **File Allocation Table** with 16 entries (0–15). File A's blocks 0–4 sit at physical blocks 4, 7, 2, 10, 12; File B's blocks 0–3 sit at physical blocks 6, 3, 11, 14. Table contents: entry 2 → 10, entry 3 → 11, entry 4 → 7 (marked "file A starts here"), entry 6 → 3 (marked "file B starts here"), entry 7 → 2, entry 10 → 12, entry 11 → 14, entry 12 → -1, entry 14 → -1. Entries 0, 1, 5, 8, 9, 13, 15 are empty ("Unused block"). The chains are therefore A: 4→7→2→10→12→-1 and B: 6→3→11→14→-1.*

**Pros:**

- Similar to linked list
- Random access is faster than linked list
  (FAT is in-memory, vs. Link List where pointers are on disk)

**Cons:**

- FAT must be kept in-memory

**Used in:** MS-DOS, IBM OS/2, Windows 9x

### Exercise — following a FAT chain

Given a simplified FAT table:

```
Index:    3    4    5    6    7    8
Value:    4    5    6   -1    8   -1
```

File A starts at index 3. What are the data blocks of File A?

**Answer:**

- $3 \rightarrow 4 \rightarrow 5 \rightarrow 6 \rightarrow -1$
  **Answer: Blocks 3, 4, 5, 6**

### FAT32 entry format

In **FAT32**:

- Each **table entry** size **is 4 bytes** (hence the 32…)
- **28 bits** are used for addressing
- Last 4 bits are:
  - End-of-chain (on of file)
  - (Physically) Bad cluster bit
  - Reserved

FAT32 supports up to $2^{28} = 268{,}435{,}456$ blocks (not files!)

(In Microsoft/FAT/NTFS terminology block=cluster=allocation unit)

*Note: the slide writes "End-of-chain (on of file)"; presumably "end of file".*

### Block size trade-off

Block size is configurable.

Usually between 4k $\rightarrow$ 32k

Big block size $\rightarrow$ bigger supported volume $\rightarrow$ worse internal fragmentation

Smaller block size $\rightarrow$ smaller volume $\rightarrow$ better internal fragmentation

### Exercise — size of a FAT32 table

**What is the size of the following FAT32 table?**

- 1TiB HDD disk
- 4KiB block size

$$\text{Number of entries in the table} = \frac{1_{TiB}}{4_{KiB}} = \frac{2^{40}}{2^{12}} = 2^{28} = 268{,}435{,}456$$

$$\text{Size of table} = 32_{bit} \times 2^{28} = 4_{B} \times 2^{28} = 2^{30}_{B} = 1 GiB \text{ (in memory!)}$$

### Exercise — can FAT32 span an 8TiB disk?

**Are we able to use this disk and configuration in FAT32, and using the whole disk?**

- 8TiB HDD disk
- 4KiB block size
- (recall 28bits are used for addressing)

$$\text{Number of entries in the table} = \frac{8_{TiB}}{4_{KiB}} = \frac{2^{43}}{2^{12}} = \mathbf{2^{31}} \geq \mathbf{2^{28}_{bits\ for\ addressing}}$$

**We don't have enough addressing bits to span the entire address space!**

### FAT32 maximum disk sizes

FAT32 supports up to 64KB block/cluster size

So, FAT32 can support disks up to:
$$2^{28} \times 2^{16} = 2^{44} = 16_{TiB}$$

But usually block size was 32KB, meaning top disk was:
$$2^{28} \times 2^{15} = 2^{43} = 8_{TiB}$$

In real life MS format tool limited to $\mathbf{25_{bits}}$ **addressing** and **32KB block size**, due to compatibility issues. Therefore, maximum real-world disk was:
$$2^{25} \times 2^{15} = 2^{40} = 2_{TiB}$$

### Fragmentation and defrag

As the system is being used (new file, deleted files etc.), files blocks get scattered throughout the disk. Literally.

The **external fragmentation** impacts the performance on HDD due to "extra seeks" on the disk

To improve performance, users used to run "defrag" software

*Figure: screenshot of the classic Windows "Defragmenting Drive C" tool, showing a grid of blue and cyan block cells with scattered gaps, a progress bar at 89% complete, and Stop / Pause / Legend / Hide Details buttons.*

### Defragmentation on SSD

SSD offers **fast random access** (no mechanical seek time)

**SSD** intentionally spreads out the file blocks, due to its **limited write/erase cycles per cell**

Therefore, defragmentation on SSD is not helpful, and even **harmful**

---

## File Allocation — New Technology File System (NTFS)

NTFS uses Master File Table (MFT) instead of FAT

MFT is not just a table, but a **database** with advanced caching system. Small files data are kept inside MFT where large has a pointer to the file.

| File Name | Attribute | Location |
| --- | --- | --- |
| report.docx | Data Pointer | Cluster 100 |
| report.docx | Data Pointer | Cluster 101 |
| report.docx | Data Pointer | Cluster 102 |

---

<!-- source pages 29-44 -->

## File Allocation — Index Nodes (inode), Used in \*Nix OSes

A **predefined location** on the disk, **determined during `format`**, is allocated for inodes.

1 inode per file.

*Figure: A disk cylinder with 32 numbered blocks (0–31); a directory entry maps the file `jeep` to index block `19`. Block 19 is blown up to show its contents — the list 9, 16, 1, 10, 25, −1, −1, −1 — whose non-negative entries point at the shaded data blocks (1, 9, 10, 16, 25) scattered on the disk. Alongside, the inode layout is drawn as a stack of fields: File Attributes, then Address of disk block 0 through Address of disk block 7, then Address of block of pointers, each with an arrow leaving to the right.*

### Problem and solution: multi-level inode

**Problem:**
Files can be huge (i.e. huge inodes), locating a random location can be expensive.

**Solution:**
Multi-level inode

### Content blocks and indirection blocks

**Content Block:** block that holds data

**Indirection Block**: block (not inode) that holds **addresses** to **another indirection block** or **content block**

Assuming **4k block size** and **32bit address** an indirect block pointing to data block holds:

$$\frac{4_{KiB}}{4_{B}} = 1_{KiB} = 1024_{addresses}$$

### Splitting the indirection

**Now there's a new problem:**
Indirection causes a performance hit, especially for small files…

**Solution:**
Split the indirection based on the size of the file

### EXT4 with 4k page size & 32bit address

| Pointer Type | Max Blocks | Max File Size |
|---|---|---|
| 12 Direct Pointers | 12 | $12 \times 4_{KiB} = 48_{KiB}$ |
| Single-Indirect | $\frac{4_{KiB}}{32_{bit}} = 1_{KiB}$ | 4MB |
| Double-Indirect | 1,048,576 | 4GB |
| Triple-Indirect | 1,073,741,824 | 4TB |

Max file supported – 4TB

**15 pointers in total (in inode)!**

*Figure: The classic UNIX inode diagram — a box listing mode, owners (2), timestamps (3), size block, count, then a shaded region of direct blocks, then single indirect, double indirect and triple indirect entries. The direct blocks point straight at "data" boxes (one of them labelled Content block); single indirect points to one unlabelled pointer block that points to two data boxes; double indirect points to a pointer block whose entries point to two further pointer blocks that each reach data boxes, and the upper of those second-level pointer blocks is labelled Indirection block; the triple indirect entry is drawn with no outgoing arrows.*

### Exercise: which pointer covers offset 300KiB?

Assume an EXT4 file system with:

- **4KiB** block size
- inode contains:
  - 12 direct block pointers
  - 1 single-indirect
  - 1 double-indirect
- Each block pointer = **4 bytes**

For accessing offset 300KiB, which inode block pointer do we need?

**Answer:**

Block # containing offset 300KB $\rightarrow \left\lfloor \frac{300_{KiB}}{4_{KiB}} \right\rfloor = 75$

Direct pointers cover blocks: 0–11

Single-indirect, next 1024 blocks: covers 12–1035

We are looking for block 75, so we use **single-indirect** node!

### Static inode tables in EXT4

EXT4, **preallocate/reserve** the disk space for the inodes.
The dedicated inode tables are **ready to use** right after format!

This means, in EXT4, inode tables are static!
Meaning… there's a maximum number of inodes (i.e. files)!

This is **regardless** to disk space!

(there are inode implementations with dynamic allocation, but flexibility comes with a cost…)

*Figure: A terminal screenshot illustrating the point — the highlighted error `mkdir: cannot create directory 'test': No space left on device`, followed by `df -TH` output whose highlighted row shows `/dev/vdc1  ext4  106G  37G  64G  37%  /home/data`, i.e. plenty of free space yet no room for a new file. Other rows listed: `/dev/vda2 ext4 42G 4.5G 35G 12% /`, `devtmpfs devtmpfs 17G 0 17G 0% /dev`, `tmpfs tmpfs 17G 4.1k 17G 1% /dev/shm`, `tmpfs tmpfs 17G 9.0M 17G 1% /run`, `tmpfs tmpfs 17G 0 17G 0% /sys/fs/cgroup`, `/dev/vda1 ext4 1.1G 153M 801M 16% /boot`, `tmpfs tmpfs 3.4G 0 3.4G 0% /run/user/0`, `/dev/vdb1 ext4 106G 990M 100G 1% /home/gis`, `tmpfs tmpfs 3.4G 0 3.4G 0% /run/user/1000`.*

### Inode-to-disk ratio

During EXT4 format, we set the inode disk ratio (in powers of 2). This sets for how much space is used for inodes…

For example, **1TiB disk**, **1 inode per 4k**, **inode size 256bytes**:

$$\text{Total possible inodes} = \frac{2^{40}}{2^{12}} = 2^{28}_{inodes}$$

$$\text{Size reserved for inodes} = 2^{28}_{inodes} \times 2^{8}_{inode\ size} = 2^{36}_{bytes} = \mathbf{64_{GiB}}\text{!!!!}$$

Therefore, the balance is between **max inode #** and **reserved space**

If we are going to have lots of small files – prefer lower ratio
If we are going to have fewer large files – prefer higher ratio

### Exercise: internal fragmentation

Remember, we still have internal fragmentation on top of that…

EXT4 with **4 KiB block size**
A file is **50 KiB** in size

How many **blocks** will this file use?
What is the **internal fragmentation**, in bytes?

**Answer:**

How many blocks do we need to 50KiB file?

$$\left\lceil \frac{50_{KiB}}{4_{KiB}} \right\rceil = \lceil 12.5 \rceil = 13_{blocks} = 52_{KiB}$$

So, the internal fragmentation is:

$$52_{KiB} - 50_{KiB} = 2_{KiB} = 2048_{bytes}$$

---

## Partitions

A disk is separated to different *partitions*. Each partition can be **formatted** to a different file system.

(can also use the tool `diskpart` from command prompt)

*Figure: Windows Disk Management screenshot. The volume list shows (Disk 0 partition 1) Simple/Basic, Healthy (E…), 260 MB, 260 MB free, 100 %; (Disk 0 partition 4) Simple/Basic, Healthy (R…), 1.95 GB, 1.95 GB, 100 %; ESD-USB (D:) Simple/Basic FAT32, Healthy (A…), 31.99 GB, 27.35 GB, 86 %; Windows-SSD (C:) Simple/Basic NTFS, Healthy (B…), 951.65 GB, 283.20 GB, 30 %. Disk 0 (Basic, 953.85 GB, Online) is laid out as a 260 MB "Healthy (EFI System Partition)" annotated "Boot partition (GPT using FAT32)", then Windows-SSD (C:) 951.65 GB NTFS "Healthy (Boot, Page File, Crash Dump, Basic Data Partition)" annotated "Main OS partition (NTFS)", then 1.95 GB "Healthy (Recovery Partition)" annotated "OS recovery (WinRE)". Disk 1 (Basic, 698.60 GB, Online) shows ESD-USB (D:) 32.00 GB FAT32 "Healthy (Active, Primary Partition)" annotated "32GB partition of FAT32 (today exFAT is popular on removable devices)", and 666.60 GB Unallocated annotated "666GB Unallocated disk (not usable, until formatted)".*

In "real" Ubuntu you would see `/dev/sda1/` as FAT32 which holds EFI…

*Figure: Terminal output of `sudo parted -l` on a VM, with three disks annotated. `/dev/sda` (Msft Virtual Disk (scsi), 407MB, sector size 512B/512B, Partition Table: loop) with partition 1 spanning 0.00B–407MB, 407MB, ext2 — annotated ""loop" as it is a VM. "real" computer would usually say "GPT" — ext2 file system". `/dev/sdb` (4295MB, sector size 512B/4096B, Partition Table: loop) with partition 1 spanning 0.00B–4295MB, 4295MB, linux-swap(v1) — annotated "SWAP partition (in windows it is a file), Not formatted as "traditional filesystem"". `/dev/sdc` (1100GB, sector size 512B/4096B, Partition Table: loop) with partition 1 spanning 0.00B–1100GB, 1100GB, ext4 — annotated "Main disk, emulated as ext4". A warning line at the top reads "Unable to open /dev/sda read-write (Read-only file system). /dev/sda has been opened read-only."*

*Note: the word "inode" appears as a stray leftover label at the bottom of this slide.*

---

## FORMAT

`format` command, mainly know for erasing a partition, is not just erasing…

It **creates the filesystem structure** on the partition

---

## MOUNT

In \*nix OSs, a **partition (or device)** must be **mounted** to be accessible in the filesystem.

Mount means, attaching a partition to a directory:

```bash
sudo mount /dev/sdb1 /mnt/usb
```

Mounts the partition `/dev/sdb1` to `/mnt/usb/`
Now you can access it via `/mnt/usb/notes.txt`

In Windows, each "partition" gets a drive letter (e.g. "c:\", "d:\" etc.)

---

<!-- source pages 45-62 -->

## Locating a file

**How to find a file on the file system?**

We don't want users to memorize the location in the FAT, NTFS, inode…

Using a file **name**!

But how to *translate* the file name to the proper entity of the FS?

First, we need to talk about **directories**…

---

## Directory

A **directory**, which has a name (like a file), contains files and other directories.

Old OS like MS-DOS 1.0 did not support directories.

There was one directory – the root directory. The root directory held a list of all the files.

*Figure: Screenshot of an IBM Personal Computer DOS Version 1.00 (Copyright IBM Corp 1981) session — after "Enter today's date (m-d-y): 08-04-81", the command `A>dir *.com` lists a single flat root directory: IBMBIO.COM 1920 07-23-81, IBMDOS.COM 6400 08-13-81, COMMAND.COM 3231 08-04-81, FORMAT.COM 2560 08-04-81, CHKDSK.COM 1395 08-04-81, SYS.COM 896 08-04-81, DISKCOPY.COM 1216 08-04-81, DISKCOMP.COM 1124 08-04-81, COMP.COM 1620 08-04-81, DATE.COM 252 08-04-81, TIME.COM 250 08-04-81, MODE.COM 860 08-04-81, EDLIN.COM 2392 08-04-81, DEBUG.COM 6049 08-04-81, BASIC.COM 10880 08-04-81, BASICA.COM 16256 08-04-81 — illustrating a filesystem with no sub-directories.*

### Sub-directory

**Sub-directories** (nested directories) allow placing more files "inside it", which creates a **tree structure** of **all the files** in the partition.

Each <u>sub</u>-directories contains two "special" dirs: `.` and `..`

- `.` – current directory
- `..` – parent directory

### Current directory & relative/absolute

**Each process** has a *current directory* (written in the PCB).

Current directory can be changed using the OS-API:

- **Windows** – `GetCurrentDirectory`/`SetCurrentDirectory`
- **\*nix** – `getcwd`/`chdir`

**Relative path** – path **relative** to the process current directory

**Absolute path** – path from the root directory of the fs

**Assume the absolute path:**

```
/dir1/dir2/Rome
```

**Assume process current directory:** `/dir1/dir3`

**All the following paths, in the process, leads to Rome:**

```
/dir1/dir2/Rome
./../dir2/Rome
./.././dir2/././Rome
```

---

## Directory – FAT

Directory is a **special <u>file</u>** that contains a *directory table*.

*Directory table* contains the list of files (which can be dirs) within it, having **at the least** the files "." and ".."

A directory entry takes **32bytes** and contains:

- File name
- First file block of the file → can be looked for in FAT for the next block
- Several Attributes
- File size (0 for directory)

The partition **root directory table** is written in the partitions table.

### Locating a file – FAT

Opens the directory table:

- Relative path – of the current directory (from PCB)
- Absolute path – of the root directory (loaded from partition table)

**Search** the next file (or directory) **by name** in the *directory table* and get the file block index to search in FAT.

Continue until going over the whole path.

---

## Directory – NTFS

Directory is a **special file** that holds the files (or dirs) in it, therefore, a directory has an MFT record.

Unlike FAT which stores directories as flat tables, NTFS stores directory entries in a sorted B⁺-Tree (variant of B-Tree):

- ***Key*** - file name
- ***Value*** – pointer to the file MFT record

*Figure: Tree sketch — "Directory File" contains a "B⁺-Tree" whose leaves map names to MFT records: `ex3.pdf → MFT #27`, `srcdir → MFT #29`, `...`.*

The root directory B-Tree file of the partition can be found by searching the MFT.

### Master File Table (MFT) – NTFS (simplified)

| MFT Entry # | Description |
| --- | --- |
| 0 | $MFT (MFT itself) |
| 1 | $MFTMirr (MFT mirror) |
| 2 | $LogFile (journaling) |
| 3 | $Volume (volume label) |
| 4 | $AttrDef (attribute definitions) |
| 5 | . (root directory) |
| 6 | \Users\me |
| 7 | \Users\me\ex2\ |
| 8 | \Users\me\ex2\uthreads.c |

Contents of one entry:

```
MFT Entry #8: uthreads.c
```

- `Standard Info` – Timestamp, flags
- `File Name` – Unicode name, parent ID
- `Data Attribute` – Blocks or inline data (small files)

### Locating a file – NTFS

Search the file (or dir) name in the MFT database.

If directory, open the B-Tree and search by the filename the next entity in the path. From the B-Tree extract the next MFT entry.

Continue until going over the whole path.

---

## Directory – EXT4 (inode)

Directory is a **special file** that contains a map that maps filename to inode number.

As a directory is a file, it has an inode.

The partition **root directory inode** is usually inode with index 2, but it is also written in the partitions table.

### Locating a file – EXT4

Opens the directory inode:

- Relative path – of the current directory (from PCB)
- Absolute path – of the root directory (loaded from partition table)

**Search** the next file (or directory) **by name** in the *directory file* and get the inode index to search in inodes allocation on disk.

Continue until going over the whole path.

---

## Linking

**File systems** allow *linking* files together.

### Hard link

<u>Hard link</u> – two different paths **are the same file**

- Use case: `/var/app/app.log` and `~/app/logfile.log` are the same
- Hard links can be done only **within the same partition**, as both paths are the same file (in the FS):
  - Hard links for files work by having multiple directory entries point to the **same inode, FAT entry, or MFT record**
- Windows - `mklink /H [target] [source]`
- \*nix – `ln [source] [target]`
- Hard links for directories is **not** allowed – can create loops in the file system tree
  - The "." and ".." directories are "special" hard links FS creates a manages
- To delete the hard linked file – you must delete all its instances

Windows example:

```
C:\Users\green\test>mklink /H 2.txt 1.txt
Hardlink created for 2.txt <<===>> 1.txt

C:\Users\green\test>dir
 Volume in drive C is Windows-SSD
 Volume Serial Number is F258-6CB9

 Directory of C:\Users\green\test

07/02/2025  10:11    <DIR>          .
07/02/2025  10:05    <DIR>          ..
07/02/2025  10:10                 4 1.txt
07/02/2025  10:10                 4 2.txt
               2 File(s)              8 bytes
               2 Dir(s)  300,475,314,176 bytes free
```

\*nix example:

```
tcs@tc-pc:~/test$ ln 1.txt 2.txt
tcs@tc-pc:~/test$ ll
total 16
drwxr-xr-x  2 tcs tcs 4096 Feb  7 10:10 ./
drwxr-x--- 23 tcs tcs 4096 Feb  7 10:09 ../
-rw-r--r--  2 tcs tcs    5 Feb  7 10:09 1.txt
-rw-r--r--  2 tcs tcs    5 Feb  7 10:09 2.txt
```

### Symbolic link – \*Nix

<u>Symbolic link</u> – a file that **points** to another file

- Symbolic linked file contains the **absolute path of the linked file**
- Deleting the file (not the link) **makes the link file invalid**
- **Very** popular in \*nix OSes
- Symbolic links works with directories
  - As it is just a file pointing to another file…

```
tcs@tc-pc:~/test$ ln -s 1.txt 2.txt
tcs@tc-pc:~/test$ ll
total 12
drwxr-xr-x  2 tcs tcs 4096 Feb  7 10:43 ./
drwxr-x--- 23 tcs tcs 4096 Feb  7 10:30 ../
-rw-r--r--  1 tcs tcs    5 Feb  7 10:30 1.txt
lrwxrwxrwx  1 tcs tcs    5 Feb  7 10:43 2.txt -> 1.txt
```

*Figure: A second terminal screenshot — a cropped long-format file listing showing sizes, dates and `->` link targets: `pygettext3 -> pygettext3.10*` (13 Aug 8 2024), `pygettext3.10*` (24235 Jun 7 2023), `pygettext3.11*` (24225 Sep 7 04:03), `pygettext3.12*` (24225 Sep 6 22:03), `python3 -> python3.10*` (10 Aug 8 2024), `python3-config -> python3.10-config*` (17 Aug 8 2024), `python3.10*` (5909000 Sep 11 18:47), `python3.10-config -> x86_64-linux-gnu-python3.10-config*` (34 Sep 11 18:47), `python3.11*` (6724632 Sep 7 21:35), `python3.11-config -> x86_64-linux-gnu-python3.11-config*` (34 Sep 7 21:35), `python3.12*` (7903536 Sep 10 03:05), `python3.12-config -> x86_64-linux-gnu-python3.12-config*` (34 Sep 10 03:05), `qrcode-terminal -> ../share/nodejs/qrcode-terminal/bin/qrcode-t` (54 Feb 18 2022).*

### Symbolic link & Junction – Win

<u>Symbolic link</u> – a special file using NTFS **reparse points** (not supported in FAT32):

- File = MFT entry. Reparse point = Redirects FS operations to "custom logic"

<u>Junction point</u> – a directory that **points** to another directory

Due to *historical reasons* \*nix symbolic link concept was **split into two in Windows**. Microsoft **first** implemented a **junction point** for directories.

Later, Windows added symbolic links for **both files and directories**, matching \*nix behavior. Only in Windows 10 they finally matched the feature…

However, **junction points** (for directories only) remain for backward compatibility.

The first two `mklink` commands below (symbolic links) **require admin rights**; the junction does not:

```
C:\Users\green\test>mklink symbolicLinked1.txt 1.txt
symbolic link created for symbolicLinked1.txt <<===>> 1.txt

C:\Users\green\test>mklink /D symbolicLinkedTest2 test2
symbolic link created for symbolicLinkedTest2 <<===>> test2

C:\Users\green\test>mklink /J junctionTest2 test2
Junction created for junctionTest2 <<===>> test2

C:\Users\green\test>dir
 Volume in drive C is Windows-SSD
 Volume Serial Number is F258-6CB9

 Directory of C:\Users\green\test

07/02/2025  11:02    <DIR>          .
07/02/2025  10:05    <DIR>          ..
07/02/2025  10:58                 4 1.txt
07/02/2025  11:02    <JUNCTION>     junctionTest2 [C:\Users\green\test\test2]
07/02/2025  11:00    <SYMLINK>      symbolicLinked1.txt [1.txt]
07/02/2025  11:01    <SYMLINKD>     symbolicLinkedTest2 [test2]
07/02/2025  10:58    <DIR>          test2
               2 File(s)              4 bytes
               5 Dir(s)  300,303,745,024 bytes free
```

---

<!-- source pages 63-80 -->

## File Name to Specify Attribute

OSes use **file name** to **indicate attributes** to the file.

**Hidden file in \*nix OSes** – "." at the beginning of the file name

**Executable file extension** ([filename].exe or [filename].com) – indicates file is executable

**Custom Extensions** are <u>widely used</u> in **many OSes** to **associate** a **file type** to a **program**:

- .bat – command prompt batch commands
- .dll (win), .so (linux), .dylib (macos) – dynamic library
- .py – to be executed by Python interpreter
- .txt – text file

---

## File Attributes

### FAT

Using file name for attributes is… nice, but we need something better.

In FAT, attributes are stored within **<u>directory</u> entries** and include:

- File name
- Size
- Read Only
- Hidden (like "." in \*nix OSes)
- System file
- Is Directory
- Archive (used by backup programs)
- Creation date/time
- Last modified date/time
- Last Access date/time

> Notice, file permissions and ownership are not there…

### EXT4

Attributes are stored within the **inode** and include:

- Size
- Permission (r/w/x bits)
- Creation date/time
- Last modified date/time
- Last Access date/time
- Supports custom attributes

### NTFS

Attributes are stored within the **MFT record**:

- Size
- Owner (user)
- FAT attributes
- Timestamps (created, access, modified)
- Permissions list – ACL
- Custom attribute key/value
- Alternate data streams – NTFS supports multiple "datas" for a file

### Access Control List

While in \*nix access control is specified by a byte (r/w/x and u/r/a), in windows access control is more comprehensive.

ACL is constructed of **A**ccess **C**ontrol **E**ntries (ACE) each contain:

- **Who** - the permission is applied using Security Identifier (SID)
- **What** - Permission allowed/denied
- **Inheritance** – if directory, does ACL applies to files and sub-dirs.
- **Audit** – logging of access attempts

*Figure: screenshot of the Windows "Advanced Security Settings for Windows" dialog for Name `C:\Windows`, Owner `TrustedInstaller` (with a "Change" link), tabs Permissions / Auditing / Effective Access, and a "Permission entries" list — showing the ACL as a concrete list of ACEs.*

| Principal | Type | Access | Inherited from | Applies to |
|---|---|---|---|---|
| TrustedInstaller | Allow | Full control | None | This folder and subfolders |
| SYSTEM | Allow | Modify | None | This folder only |
| SYSTEM | Allow | Full control | None | Subfolders and files only |
| Administrators (tc-pc\Administ… | Allow | Modify | None | This folder only |
| Administrators (tc-pc\Administ… | Allow | Full control | None | Subfolders and files only |
| Users (tc-pc\Users) | Allow | Read & execute | None | This folder, subfolders and files |
| CREATOR OWNER | Allow | Full control | None | Subfolders and files only |
| ALL APPLICATION PACKAGES | Allow | Read & execute | None | This folder, subfolders and files |

The dialog also has buttons: "Change permissions", "View", "Enable inheritance", and OK / Cancel / Apply.

---

## File Descriptor / HANDLE

**A process** holds a list of all the files it opens (in PCB)

When a process opens a file, the OS returns a:

- File descriptor (Unix)
- Handle (Windows)

both serve as identifiers used to access the file and its metadata.

**File descriptor** is a common wording for both

*File descriptor* holds data about the files the **process** needs about the file

*Figure: screenshot of Process Explorer (Sysinternals) for POWERPNT.EXE, "Handles" tab, listing open handle Type/Name pairs (ALPC Port, Desktop, Directory, Event — e.g. `\RPC Control\OLE25F3C7D5F94D8FEBC39BBD960D88`, `\KnownDlls`, `\Sessions\1\BaseNamedObjects`) — showing a real process's list of open handles.*

### Ubuntu kernel space file descriptor

```c
struct file {
	file_ref_t			f_ref;
	spinlock_t			f_lock;
	fmode_t				f_mode;
	const struct file_operations	*f_op;
	struct address_space		*f_mapping;
	void				*private_data;
	struct inode			*f_inode;
	unsigned int			f_flags;
	unsigned int			f_iocb_flags;
	const struct cred		*f_cred;
	/* --- cacheline 1 boundary (64 bytes) --- */
	struct path			f_path;
	union {
		/* regular files (with FMODE_ATOMIC_POS) and directories */
		struct mutex		f_pos_lock;
		/* pipes */
		u64			f_pipe;
	};
	loff_t				f_pos;
#ifdef CONFIG_SECURITY
	void				*f_security;
#endif
	/* --- cacheline 2 boundary (128 bytes) --- */
	struct fown_struct		*f_owner;
	errseq_t			f_wb_err;
	errseq_t			f_sb_err;
#ifdef CONFIG_EPOLL
	struct hlist_head		*f_ep;
#endif
	union {
		struct callback_head	f_task_work;
		struct llist_node	f_llist;
		struct file_ra_state	f_ra;
		freeptr_t		f_freeptr;
	};
	/* --- cacheline 3 boundary (192 bytes) --- */
} __randomize_layout
```

*Note: the last line of the struct is cut off at the bottom edge of the slide.*

### Windows user space file descriptor

```c
typedef struct _FILEDESCRIPTORW {
  DWORD    dwFlags;
  CLSID    clsid;
  SIZEL    sizel;
  POINTL   pointl;
  DWORD    dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  DWORD    nFileSizeHigh;
  DWORD    nFileSizeLow;
  WCHAR    cFileName[MAX_PATH];
} FILEDESCRIPTORW, *LPFILEDESCRIPTORW;
```

---

## Common Files Operations (Unix)

`FILE*` vs. file descriptor (type `int`) — **HW**

From C course, you are probably aware of FILE\* for fopen/fclose etc.

`FILE*` is a "higher-level" wrapper for the file descriptor.

In the following slides we will look into some of the "lower-level" functions.

You can convert between the two using `fdopen` and `fileno` functions

### open

```c
int open(const char *pathname, int flags, mode_t mode)
```

- **pathname** – path to file
- **flags** - O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, O_APPEND and more…
- **mode** - "a", "r", "w"
- **Return** – file descriptor
- **Notice** – miss use of flags and mode can create unexpected results

Successful open **appends** the **descriptor** to the **list of open files** in the **PCB**

The file will successfully open only after **kernel checks** the **running thread user** has the appropriate permissions.

The **Cursor** is (usually) set to the beginning of the file. The cursor is **stored in the file descriptor**.

#### What open does internally (code from OS/161)

`open` makes a system call

```c
case SYS_open:
    err = sys_open((const char *)tf->tf_a0, (int)tf->tf_a1, &retval);
    break;
```

Allocate space for the descriptor

```c
curproc->file_table[i] = (struct file_handle *)kmalloc(sizeof(struct file_handle));
```

Call for the underlying hardware (`vfs_open` would actually be implemented in FS driver)

```c
err = vfs_open(cin_filename, flags, 0, &curproc->file_table[i]->vnode);
```

#### A word about windows….

Windows also has the concept of file "sharing" and "locking".

Open a file in windows (`CreateFile` function) allows specifying if the file can be opened by **other** processes, and with which permissions while the file is open:

- `0` – exclusive (no other process can touch the file)
- `FILE_SHARE_READ` and `FILE_SHARE_WRITE`

### close

```c
int close(int fd)
```

- **fd** – file descriptor to close
- **Return** – error, or 0 for success

Closes the handle and removes the descriptor from the PCB

### read

```c
ssize_t read(int fd, void* buf, size_t count)
```

- **fd** – file descriptor
- **buf** – buffer to read into
- **count** – amount of bytes to read
- **Return** – bytes read

Reads *count* bytes into *buf* starting at the **current cursor** index

### write

```c
ssize_t write(int fd, void* buf, size_t count)
```

- **fd** – file descriptor
- **buf** – buffer to write from
- **count** – amount of bytes to write
- **Return** – bytes written

Writes *count* bytes from *buf* starting at the **current cursor** index

### lseek

```c
off_t lseek(int fd, off_t offset, int whence)
```

- **fd** – file descriptor
- **offset** – bytes to seek/move
- **whence** – where to start the offset?
  - SEEK_SET – offset is an index from the beginning
  - SEEK_END – offset is an index from the end
  - SEEK_CUR – offset is from the current cursor position
- **Return** – new file absolute offset

Changes the cursor index (or offset)

---

## Named Pipes (in \*nix)

**Files** API in OSes provides functionality to read/write/open etc. which can fit too **other technologies** besides file system files.

PIPE is a *special temporary file* (has an inode) that allows multiple processes to send message between them.

- Once all the processes close the file, it is automatically deleted
- Once a data is read, it is removed
- Kernel manages read/write synchronization with the file

Make FIFO pipe using:

- `mkfifo mypipe` – bash command
- `mkfifo("mypipe")` function

**Writer:**

```c
int main() {
    int fd;
    char message[] = "Hello from writer!";

    // Open the named pipe for writing
    fd = open("mypipe", O_WRONLY);
    if (fd == -1) {
        perror("Error opening pipe");
        exit(1);
    }

    // Write messages every 2 seconds
    for (int i = 0; i < 5; i++) {
        write(fd, message, strlen(message) + 1);
        printf("Writer: Sent message %d\n", i + 1);
        sleep(2);
    }

    close(fd);
    return 0;
}
```

**Reader:**

```c
int main() {
    int fd;
    char buffer[100];

    // Open the named pipe for reading
    fd = open("mypipe", O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        perror("Error opening pipe");
        exit(1);
    }

    fd_set read_fds;
    struct timeval timeout;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        timeout.tv_sec = 1;  // Wait up to 1 second
        timeout.tv_usec = 0;

        int ret = select(fd + 1, &read_fds, NULL, NULL, &timeout);

        if (ret > 0 && FD_ISSET(fd, &read_fds)) {
            read(fd, buffer, sizeof(buffer));
            printf("Reader: Received message: %s\n", buffer);
        } else {
            printf("Reader: No new data, checking again...\n");
        }
    }

    close(fd);
    return 0;
}
```

---

<!-- source pages 81-102 -->

## Redundant Arrays of Independent Disks (RAID)

Data centers use huge number of disks (from 10k+ to 100k+).

With time… disks **wear out** and **fail**. But, **what about their data?!**

Researchers try to determine the **A**nnual **F**ailure **R**ate (AFR) of disks.

Based on research from 2023:

- HDD AFR – 1.64% (failure less predictable)
- SSD AFR – 0.98% (failure is predictable)

Meaning, in large data centers, at least one disk fail each day.

Source: `https://www.techspot.com/news/97909-ssd-reliability-only-slightly-better-than-hdd-backblaze.html`

### Motivation

What can we do to **prevent data loss**?

No one wants to hear that their e-mails are lost due to disk failure…

When purchasing so many disks, can we use "inexpensive", smaller, disks to create partitions with larger volumes?

This can be done using RAID.

### RAID implementations

- **Software RAID** – implemented in the OS
- **Hardware RAID** – implemented within a dedicated RAID controller

### Evaluation criteria

For each RAID setup, we will look at the:

- **Speed/Performance** – read/write overhead of the setup
- **Redundancy** – how many disks can fail without data loss
- **Capacity** – capacity of partition using "cheaper" hardware

---

## RAID 0

Data is stripped on multiple disks.

- The **capacity** is the **sum** of the disks in the RAID.
- Read/Write **performance increase**, as accessing data from different disks lowers the overhead of the hardware.
- **No redundancy** – a disk failure leads to data loss.

*Figure: RAID 0 over two disks — DISK 1 holds Block 1, Block 3, Block 5, Block 7 and DISK 2 holds Block 2, Block 4, Block 6, Block 8, both hanging off a single RAID 0 controller link; consecutive blocks alternate between disks (striping).*

*Images from `https://www.booleanworld.com/raid-levels-explained/`*

## RAID 1

Data is mirrored.

- **Disk redundancy** as the data is mirrored.
- We pay in **capacity** with a loss of 50%.
- **Read performance** <u>increase</u> as data is available on two disks, but **write performance** <u>decrease</u> as writing need to be done twice.

*Figure: RAID 1 over two disks — DISK 1 and DISK 2 each hold the identical sequence Block 1, Block 2, Block 3, Block 4, showing full mirroring.*

## RAID 10 (1+0)

RAID 10 uses RAID 0 to stripe the disk and increase partition volume, but also uses RAID 1 mirroring to achieve redundancy.

- **Overall performance** are high, although the mirroring, as the partition is spread over multiple disks.
- **Overall capacity** increase due to RAID 0, but 50% of the disks are used for mirroring. This is an **expensive** setup!
- **Overall Redundancy** improves unless both **disk, and its mirror** don't fail.

*Figure: RAID 10 over four disks — DISK 1 and DISK 2 are a mirrored pair both holding Block 1, Block 3, Block 5, Block 7; DISK 3 and DISK 4 are a mirrored pair both holding Block 2, Block 4, Block 6, Block 8; the two mirrored pairs are joined by a striping link at the top.*

## RAID – Data recovery using XOR Parity

Using parity bit we can recover data of a lost disk from N disks in RAID 0.

Parity is calculated by XOR ($\oplus$).

For example, assume the following data from 3 disks:

- Disk 1 – `1101`
- Disk 2 – `1100`
- Disk 3 – `0110`

$$\text{Parity} = 1101\ (\text{disk1}) \oplus 1100\ (\text{disk2}) \oplus 0110\ (\text{disk3}) = 0001$$

$$\text{Disk 2 recovery} = 1101\ (\text{disk1}) \oplus 0110\ (\text{disk3}) \oplus 0001\ (\text{parity}) = 1100$$

## RAID 4

Use RAID 0 on multiple disks.

**One** disk stores parity information (P).

- **Capacity** increase with the cost of 1 disk
- **Redundancy** of **up to 1 disk** (if another is lost, data is lost!!!)
- **Read Performance** increase due to increased data availability
- **Write Performance** decrease due to of parity calculation and extra write, also **parity disk is a bottleneck** as every write creates write on parity disk.

*Figure: RAID 4 over four disks — DISK 1 holds Block A1, B1, C1, D1; DISK 2 holds Block A2, B2, C2, D2; DISK 3 holds Block A3, B3, C3, D3; DISK 4 is the dedicated parity disk holding Block A$_p$, B$_p$, C$_p$, D$_p$.*

## RAID 5

RAID 5 approaches the RAID 4 parity disk bottleneck.

RAID 5 distributes the parity disk between all other disks.

The parity block is placed on a disk that its corresponding data isn't there.

- **Capacity** and **Redundancy** are like RAID4
- **Speed** increases due to the distribution of the parity disk

*Figure: RAID 5 over four disks — parity rotates across disks: DISK 1 holds A1, B1, C1, D$_p$; DISK 2 holds A2, B2, C$_p$, D1; DISK 3 holds A3, B$_p$, C2, D2; DISK 4 holds A$_p$, B3, C3, D3.*

## RAID 6

RAID 6 uses **double parity** to increase the redundancy to **two disk failures**.

- Parity P – XOR
- Parity Q – Weighted XOR

- **Write operations** are slow due to the double parity calculation
- **Capacity** is also reduced due to the storage of the second parity

*Figure: RAID 6 over five disks — two parity blocks per stripe rotate across the array: DISK 1 holds A1, B1, C1, D$_p$, E$_q$; DISK 2 holds A2, B2, C$_p$, D$_q$, E1; DISK 3 holds A3, B$_p$, C$_q$, D1, E2; DISK 4 holds A$_p$, Bq, C2, D2, E3; DISK 5 holds A$_q$, B3, C3, D3, E$_p$.*

---

## RAID 6 – Data Recovery Weighted XOR

Weighted XOR is calculated on the <u>finite binary</u> **Galois Field** (GF).

GF operates within **modulo** of a **prime number** or **power of prime**.

GF Supports arithmetic operations.

*Figure: Portrait sketch of Évariste Galois with the caption — Évariste Galois, 1811-1832 (age 20); Political activists and spent time in prison; Died in a duel (**probably** to defend the honor of lady *Stéphanie-Félicie Poterin du Motel* which he had romantic relationship).*

### Field sizes

$GF(2^1)$ – All numbers in the field are 1 bit:

**Add** - $1 \oplus 1 = (10)\bmod 2 = 0$   $1 \oplus 0 = 1$   $0 \oplus 1 = 1$   $0 \oplus 0 = 0$

In $GF(2^8)$ – All numbers in the field are 8 bits (a byte!)

### Definition of Q

Q is defined as following (over $GF(2^8)$):

$$Q = D_1 \oplus (D_2 \times 2^1) \oplus (D_3 \times 2^2) \oplus \cdots \oplus (D_{n+1} \times 2^n)$$

### Worked example

Example:

$$D_1 = 1010\ 1010$$
$$D_2 = 1100\ 1100$$
$$D_3 = 1111\ 0000$$

$$Q = D_1 \oplus (D_2 \times 2^1) \oplus (D_3 \times 2^2)$$

**Step 1 — first attempt at $D_2 \times 2$:**

$$D_2 \times 2 = 11001100 \ll 1 = 1\ 1001\ 1000$$

The result is larger than $2^8$ (i.e. 8 bits), therefore we need to use modulo, but $(1\ 1001\ 1000)\bmod 255$ is **hard to calculate** for the CPU…

**Step 2 — the irreducible polynomial gives a cheap modulo:**

As GF is a finite field, it has ***irreducible polynomial***.

***Irreducible polynomial*** is minimal primal polynomial constant that can generate all the elements in the field using a series of arithmetic operations.

As GF is binary and finite, XOR with the irreducible polynomial acts as modulo for the field!

For $GF(2^8)$, the irreducible polynomial is $00011011 = \text{0x11B}$

$GF(2^8)$ and 0x11B are popular in many fields in computer science…

**Step 3 — back to the example, compute $D_2 \times 2^1$:**

$$Q = D_1 \oplus (D_2 \times 2^1) \oplus (D_3 \times 2^2)$$

$$D_2 \times 2 = 1100\ 1100 \ll 1 = 1001\ 1000 \quad (\text{9}^{\text{th}}\text{ bit is deleted})$$

As the MSB of $D_2$ **before** the shift is 1, the result is larger than 8 bits, therefore we need to use modulo (XOR in GF(2)):

$$1001\ 1000 \oplus 0001\ 1011 = 1000\ 0011$$

**Step 4 — compute $D_3 \times 2^2$:**

$$D_3 \times 4 = (1111\ 0000 \ll 2) \oplus 0001\ 1011 = 1100\ 0000 \oplus 0001\ 1011 = 1101\ 1011$$

**Step 5 — combine:**

$$Q = 1010\ 1010 \oplus (1000\ 0011) \oplus (1101\ 1011) = 1111\ 0010$$

---

## Other FS & FS features

### Alternate data stream

**NTFS** support multiple data streams on a single file.

ADS file does **not** have its own MFT entry, it is in the main file entry.

The original file (pointer to file block) is stored under `$DATA`

ADS is stored under `[stream name]:$DATA`

*Figure: Windows console screenshot demonstrating ADS — `C:\Users\green>dir /R *.txt`, "Volume in drive C is Windows-SSD", "Volume Serial Number is F258-6CB9", "Directory of C:\Users\green"; the listing shows `09/02/2025  17:24   13 1.txt` followed by an indented `67 1.txt:alternate.txt:$DATA`, then `1 File(s)  13 bytes` and `0 Dir(s)  289,382,924,288 bytes free`.*

### Distributed File System

**DFS** is a concept of distributing a file system over multiple computers.

It "feels" like regular file system, but the blocks are stored on multiple computers.

**Popular DFS:**

- Microsoft DFS (MSDFS)
- Google File System (GFS)
- Hadoop Distributed File System (HDFS)

### Remote file system (Google Drive)

Implementing **file system driver** does not have to abstract a disk.

Good example is "Google drive" desktop, which its driver reflects Google Drive. The file system driver implements the OS calls to files/directories and redirects them to API calls to google drive.

### Further features

- **Security** – anti-virus, disk encryption
- **Compression** – store files compressed
- **Backup** – Hot backup of files
- **Extra features** – Open a file only if…, Perform operation on file if…

And many more…

---

# Executables, Linking and Loading

> Study notes converted from the lecture slide deck `Presentation 7 - Executables - 1 Slide-2.pdf` (90 pages) — Operating Systems, Reichman University, Spring Semester 2026.
> Lecturer: Tsvi Cherny-Shahar · TAs: Daniel Karalnik, Liam Tal.
> Faithful conversion: every substantive item on the source pages appears here, and nothing has been added from outside them. Figures are rendered as italic *Figure:* descriptions.
> `<!-- source pages a-b -->` comments mark the original page ranges.

---

<!-- source pages 1-17 -->

## Executable & Friends

*Figure: Title slide showing two side-by-side hexdumps of binary files. The upper one starts at offset `00000000` with bytes `7f 45 4c 46 02 01 01 00` and the ASCII column `.ELF............`, later showing `48 65 6c 6c 6f 2c 20 57 6f 72 6c 64 21 0a` = `Hello, World!..` at offset `00002000`. The lower one starts with `4d 5a 90 00 03 00 00 00` = `MZ...........`, contains the DOS-stub string `This program cannot be run in DOS mode....$`, the `Rich` marker, and `50 45 00 00 4c 01 05 00` = `PE..L...` at offset `000000f0` — contrasting the Linux ELF format with the Windows PE format.*

### Acknowledgments

Thanks to:

- Prof. Anat Bremler-Barr
- Prof. Gadi Taubenfeld
- Dr. Guy Katz and Prof. David Hay

Also, resources from:

- OS/161 Instructional Operating System (os161.org)
- ops-class.org

---

## Finally, we can understand… what happens when we run a program

We've learned about:

- **Threads** – running a program
- **Kernel/user modes** (ring0/3) – access the "low level hardware"
- **Scheduling** – How we manage multiple threads & context switch
- **Virtual memory** – How we access RAM
- **Filesystem** – How to access files

All of these are used when we **execute a new application**.

---

## Hello World Without main()

Does the OS require a `main()` function?

What if the programming language doesn't have `main`?!

Let's write a program WITHOUT `main`!

### Linux Assembly Example

```nasm
section .data
    msg db 'Hello, World!', 0xA ; 0xA = \n
    len equ $ - msg

section .text
    global _start    ; declares global label (entrypoint)
                     ; linux looks for it
_start:
    mov rax, 1       ; syscall: write
    mov rdi, 1       ; file descriptor: stdout
    mov rsi, msg     ; message to write
    mov rdx, len     ; message length
    syscall          ; invoke syscall

    mov rax, 60      ; syscall: exit
    xor rdi, rdi     ; exit code 0
    syscall          ; invoke syscall
```

```bash
nasm -f elf64 hello.asm -o hello.o # compiles ELF64 object
ld hello.o -o hello_elf # Links complete ELF
```

A callout points at the `_start:` label:

> ELF (Linux executable) is looking for an entrypoint to start. `_start` is the default one (can be set with `–entry=<lable>`)

*Note: the slide spells it "lable"; the option is written with an en-dash on the slide.*

### Why do we need the linker?

Object files don't have the "final" virtual addresses.

- "entities" **inside** the object - are **position independent** (relative)
- "entities" **outside** the object – are **unresolved**!

**In our case all entities are known, but nasm does not create ELF around the code!**

The linker (**ld**):

- ***Resolves*** the virtual addresses of external entities
- ***Merges*** several objects (its sections – next slide)
- **Wraps** the whole code in ELF file structure, making it an ***executable***

Terminal transcript on the slide:

```console
tcs@tc-pc:~$ nasm -f elf64 hello.asm -o hello.o
tcs@tc-pc:~$ ld hello.o -o hello_elf
tcs@tc-pc:~$ ./hello_elf
Hello, World!
```

---

## Linker Undefined Reference

`gcc` compiles and links using `ld`.

```console
tcs@tc-pc:~$ cat test.c
void func();

int main(){ func(); }
tcs@tc-pc:~$ gcc test.c
/usr/bin/ld: /tmp/cckRG265.o: in function `main':
test.c:(.text+0xe): undefined reference to `func'
collect2: error: ld returned 1 exit status
```

*Figure: the slide circles `undefined reference to `func'` in red and `ld` inside "collect2: error: ld returned 1 exit status" in green — the error comes from the linker, not the compiler.*

But we can separate the calls:

```console
tcs@tc-pc:~$ gcc -c test.c -o test.o
tcs@tc-pc:~$ ll test.*
-rw-r--r-- 1 tcs tcs   37 Jun 13 10:52 test.c
-rw-r--r-- 1 tcs tcs 1360 Jun 13 14:19 test.o
tcs@tc-pc:~$ ld test.o -o test
ld: warning: cannot find entry symbol _start; defaulting to 0000000000401000
ld: test.o: in function `main':
test.c:(.text+0xe): undefined reference to `func'
```

*Figure: `undefined reference to `func'` is circled in red; a callout arrow points at the `cannot find entry symbol _start` warning line labelled "Ignore this for now".*

---

## Let's fix test.c and try to build

Compile + Link with `gcc` – works…

```console
tcs@tc-pc:~$ cat test.c
void func(){}

int main(){ func(); }
tcs@tc-pc:~$ gcc test.c -o test_with_gcc
tcs@tc-pc:~$ ./test_with_gcc
tcs@tc-pc:~$
```

But separately, we get segmentation fault.
Why? Where is `_start` in the C code? We'll get into that later…

```console
tcs@tc-pc:~$ gcc -c test.c -o test_without_gcc.o
tcs@tc-pc:~$ ld test_without_gcc.o -o test_without_gcc
ld: warning: cannot find entry symbol _start; defaulting to 0000000000401000
tcs@tc-pc:~$ ./test_without_gcc
Segmentation fault
```

*Figure: the `ld: warning: cannot find entry symbol _start; defaulting to 0000000000401000` line is boxed in red.*

---

## Going back to hello

```console
tcs@tc-pc:~/stuff$ gcc -c hello_main.c -o hello_main.o
tcs@tc-pc:~/stuff$ ld hello_main.o libhello.a -o hello_main
ld: warning: cannot find entry symbol _start; defaulting to 0000000000401000
ld: libhello.a(hello.o): in function `hello_world':
hello.c:(.text+0x13): undefined reference to `puts'
```

- `_start` in not in our code! It is part of C-Runtime (**CRT**)
- CRT `_start` entrypoint is calling to `main()`
- Also, `libc.so` contains `puts`

```console
tcs@tc-pc:~/stuff$ ld \
  /usr/lib/x86_64-linux-gnu/crt1.o \
  /usr/lib/x86_64-linux-gnu/crti.o \
  /usr/lib/x86_64-linux-gnu/crtn.o \
  hello_main.o libhello.a \
  -dynamic-linker /lib64/ld-linux-x86-64.so.2 \
  -lc \
  -o hello_main
tcs@tc-pc:~/stuff$ ./hello_main
hello world
```

Annotations on the command:

- the three `crt1.o` / `crti.o` / `crtn.o` lines → **Link to CRT**
- `hello_main.o libhello.a` → **Link to static library**
- `-dynamic-linker /lib64/ld-linux-x86-64.so.2` → **Choose dynamic linker**
- `-lc` → **Depend on `libc.so`**

---

## ELF File Structure

**ELF (Executable and Linkable Format)** is the standard binary format for \*nix systems (not MacOS, which uses Mach-O).

It consists of several sections, each serving a specific purpose:

- **.text** - Contains executable machine code.
- **.data** - Stores initialized global and static variables.
- **.bss** - Holds uninitialized global variables.
- **.rodata** - Read-only data like string constants.
- **.interp** - Specifies the dynamic linker (for dynamically linked executables).
- **.symtab** and **.strtab** - Symbol table and string table for debugging.

(and more… on some we will discuss!)

---

## What happens when we run the executable (in Linux) in 10 steps \<simplified\>

0. Parent process calls `execve()` to create new process

1. Make a system call to `execve`:

```nasm
mov rax, 59        ; syscall number for execve on x86_64
mov rdi, filename  ; const char *filename
mov rsi, argv      ; char *const argv[]
mov rdx, envp      ; char *const envp[]
syscall
```

2. **Locate** the file via the filesystem (recurse the path from caller PCB/Root)

3. Open the file (`open()`) – read the ELF header

4. Verifies the "magic number" to make sure it's really an ELF file - **\x7FELF**

5. Reads the program header – to determine the **where** are the different sections and where in the VA they should be loaded

6. `mmap()` the file, and loads different sections to its expected VAs (backed-file mem)

7. Creates new process PCB

8. Allocate a stack, and copy the `envp[]` & `argv[]` & `argc` on the stack

9. Create a thread (new main thread) & setting its **RIP** to the **entry point** (`e_entry`), **RSP** to the top of the copied data (after argc)

    This essentially sets the `main()` you all know and love:

```c
int main(int argc, char *argv[], char *envp[])
```

10. Returns from syscall, when the "new main thread" is scheduled, it starts running the entry point

### Stack at startup

```text
[rsp]     → argc
[rsp+8]   → argv[0]
[rsp+16]  → argv[1]
  ...
[null]    → end of argv
[rsp+...] → envp[0]
  ...
[null]    → end of envp
[rsp+...] → auxv[0]
```

### WAIT!!! Where is RBP?

It is not set…!

First set on the first **function call prologue** !

---

## RECAP! Function Prologue and Epilogue

*Figure: A debugger disassembly view with three source functions and a call-stack panel, illustrating which instructions form the prologue, the call, and the epilogue. A large red brace spans the entire listing (`push rbp` … `ret`) and points at the source line `void h(){ k(); }` — the disassembled function is `h()`, whose prologue/epilogue bracket the `call 0x7ff6c7b317d0 <k>`.*

Disassembly of `h()` (address, byte, instruction):

| Address | Bytes | Instruction | Region |
|---|---|---|---|
| `0x00007ff6c7b317ee` | `55` | `push  rbp` | Prologue |
| `0x00007ff6c7b317ef` | | `mov   rbp,rsp` | Prologue |
| `0x00007ff6c7b317f2` | | `sub   rsp,0x20` | Prologue |
| `0x00007ff6c7b317f6` | | `call  0x7ff6c7b317d0 <k>` | `k()` |
| `0x00007ff6c7b317fb` | `90` | `nop` | |
| `0x00007ff6c7b317fc` | | `add   rsp,0x20` | Epilogue |
| `0x00007ff6c7b31800` | `5d` | `pop   rbp` | Epilogue |
| `0x00007ff6c7b31801` | `c3` | `ret` | Epilogue |

The corresponding source:

```c
void k(){ printf("k\n"); }

void h(){ k(); }

void g(){ h(); }
```

*Figure: CALL STACK panel — thread [1] "PAUSED ON BREAKPOINT" shows the frames `k()` (thestack.c 3:1), `h()` (thestack.c 5:1, highlighted/selected), `g()` (thestack.c 7:1), `main()` (thestack.c 18:1); thread [2] "PAUSED" shows `ntdll.dll!ntdll!ZwWaitForWorkViaWorkerFactory`, `ntdll.dll!ntdll!RtlClearThreadWorkOnBehalfTicket`, `kernel32.dll!KERNEL32!BaseThreadInitThunk`, `ntdll.dll!ntdll!RtlUserThreadStart`, `[Unknown/Just-In-Time compiled code]`.*

---

<!-- source pages 18-33 -->

## What happens when we run the executable (in Linux) in 10 steps

If we don't set RBP at the beginning, than…. how do we return from the `entrypoint` ?!

**We don't!** To exit the program, we syscall to terminate!

What happens if we `ret` at the end of the entrypoint code? 🤔

---

## ELF Header Hex Analysis

```sh
hexdump -C hello_elf | head -n 20
```

```text
tcs@tc-pc:~$ hexdump -C hello_elf -n 64 hello_elf
00000000  7f 45 4c 46 02 01 01 00  00 00 00 00 00 00 00 00  |.ELF............|
00000010  02 00 3e 00 01 00 00 00  00 10 40 00 00 00 00 00  |..>.......@.....|
00000020  40 00 00 00 00 00 00 00  28 21 00 00 00 00 00 00  |@.......(!......|
00000030  00 00 00 00 40 00 38 00  03 00 40 00 06 00 05 00  |....@.8...@.....|
00000040
```

- `7f 45 4c 46 ...` : ELF Magic Number
- `02 00`: Executable file type
- `3e 00`: Architecture (x86_64)
- `01 00 00 00`: ELF version (1)
- `00 10 40 00 00 00 00 00` – Entrypoint (`0x00401000000000`)
  (little endian – reverse byte order)

The rest are offsets to the ELF sections

### Sections in the dump

```text
tcs@tc-pc:~$ readelf --hex-dump=.text hello_elf

Hex dump of section '.text':
  0x00401000 b8010000 00bf0100 000048be 00204000 .........H.. @.
  0x00401010 00000000 ba0e0000 000f05b8 3c000000 ............<...
  0x00401020 4831ff0f 05                         H1...

tcs@tc-pc:~$ readelf --hex-dump=.data hello_elf

Hex dump of section '.data':
  0x00402000 48656c6c 6f2c2057 6f726c64 210a     Hello, World!.
```

- **`.text`** - The code !
  - (r/x permission)
  - **RIP** is set there
  - (in the mmap memory…)
- **`.data`** known at compile time
  - i.e. initialized variables
  - (r/w permission)

*Figure: braces link the first `readelf` dump (addresses `0x00401000`–`0x00401020`) to the `.text` annotation, and the second dump (`0x00402000`, ASCII "Hello, World!.") to the `.data` annotation.*

---

## So why oh why can't we run executable between OSes?

So, if the **compiled code** is for a specific CPU architecture (e.g., x86_64, ARM64).
And if the only thing that differs is how each OS loads and links executables…

WHY CAN'T WE **EASILY** run executable between OSes?!

**Just** implement \*nix with PE, and Windows with ELF…!

WINE tries to do just that!

### WINE Is Not an Emulator

WINE runs Windows programs on Linux — but it's not an emulator!

It works by understanding and loading Windows PE executables natively.

So, why is it not **that simple** and more common?

One of the main reasons is that **much of the code is not in the executables, but in Dynamic Libraries.**

---

## Static Library

***Static library*** is an **archive of compiled code** (i.e. object files)

Static library are used **during link-time** and **become part** of the executable.
This increases the executable size (just like using object file)

The linker picks only the necessary objects files during linking

- **Windows:** `.lib` files
- **\*nix:** `.a` files (a for archive)

```text
tcs@tc-pc:~/stuff$ cat hello.c
#include <stdio.h>

void hello_world(){printf("hello world\n");}

tcs@tc-pc:~/stuff$ gcc -c hello.c -o hello.o
tcs@tc-pc:~/stuff$ ar rcs libhello.a hello.o
tcs@tc-pc:~/stuff$ ll libhello.a
-rw-r--r-- 1 tcs tcs 1652 Jun 13 15:00 libhello.a
```

### Linking against the static library

`gcc` doesn't find `hello_world()` function….

```text
tcs@tc-pc:~/stuff$ cat hello_main.c
void hello_world();

int main(){ hello_world(); return 0; }
tcs@tc-pc:~/stuff$ gcc hello_main.c -o hello_main
/usr/bin/ld: /tmp/ccNmupZm.o: in function `main':
hello_main.c:(.text+0xe): undefined reference to `hello_world'
collect2: error: ld returned 1 exit status
```

So, link with `libhello.a`

```text
tcs@tc-pc:~/stuff$ gcc hello_main.c libhello.a -o hello_main
tcs@tc-pc:~/stuff$ ./hello_main
hello world
```

```text
tcs@tc-pc:~/stuff$ gcc hello_main.c -L. -lhello -o hello_main
tcs@tc-pc:~/stuff$ ./hello_main
hello world
```

### What gcc hides — and where `puts` lives

Just a reminder `gcc` does some "extra stuff" behind the scenes…

If we use `ld` manually, we're getting that `puts` is not found.
Why `puts` and not `printf`?

The compiler optimizes our code to use `puts` in this scenario.

```text
tcs@tc-pc:~/stuff$ gcc -c hello_main.c -o hello_main.o
tcs@tc-pc:~/stuff$ ld hello_main.o libhello.a -o hello_main
ld: warning: cannot find entry symbol _start; defaulting to 0000000000401000
ld: libhello.a(hello.o): in function `hello_world':
hello.c:(.text+0x13): undefined reference to `puts'
```

But why is it not found???
Because the code doesn't contain `printf` (or `puts` in our case)!
Where is it? It is in a **dynamic library**.

---

## Dynamic Library

Dynamic libraries are not simply archives of object files. But are fully **runtime-linkable binaries** that **may** include **initialization code executed automatically when** the **library is loaded**.

Dynamic libraries are loaded into the process **during runtime**

- **Reduced overall memory** - Dynamic library **virtual memory is shared** - loaded **once**
- **Reduced executable size** – code is **not** part of the executable
- **Modularity** – Application split into "modules"
  - Reduces build (compile+link) time, built individually
  - Updating an application does not require changing all the modules
    - Update during Runtime (e.g. hot patching)
- Implemented differently between Windows and \*nix (and MacOS)
- **Can be complex to maintain**

- **Windows:** `.dll` files (Dynamic Link Library)
- **MacOS:** `.dylib` files
- **\*nix:** `.so` files (Shared Object)

### Static Lib vs. Dynamic Lib

| Feature | Static Library (.lib / .a) | Dynamic Library (.dll / .so / .dylib) |
|---|---|---|
| Linking Type | Linked at compile time | Linked at runtime |
| File Extension | .lib (Windows), .a (Linux/macOS) | .dll (Windows), .so (Linux), .dylib (macOS) |
| Executable Size | Larger (library is copied into each executable) | Smaller or Unchanged (library remains separate) |
| Memory Usage | More memory (each program has its own copy) | Less memory (shared memory across programs) |
| Performance | Faster (no runtime lookup) | Slightly slower (due to runtime symbol resolution) |
| Updating Libraries | Requires recompilation of the executable | Can be updated independently of the executable |
| Portability | No external dependencies (self-contained) | Dependency and versioning — Requires correct library version to be present on the system |
| Code Sharing | Not shared — each program contains a copy | Shared across multiple programs |
| Security Risks | Lower risk (code is fixed in the binary) | Higher risk (More vulnerable to code injection) |

---

## Static vs. Dynamic Linking of Dynamic Libraries

**Static Link (to dynamic library)**:

- During compile time → write loading code into binary to load dynamic lib
- During start up →
  - OS (i.e. PE/ELF loader) loads the dynamic library
  - OS (i.e. PE/ELF loader) may load/resolve the entities the library provides

**Dynamic Link (manual loading):**

- Library is loaded by explicit user code (`LoadLibrary`/`dlopen`)
- Entities are loaded by explicit user code (`GetProcAddress`/`dlsym`)

---

## What happens when we load .so (in Linux) in 10 steps \<simplified\>

| Step | Action |
|---|---|
| 0 | process calls `dlopen()` – No special system call for this action |
| 2 | **Locate** the file via the filesystem (using *search order*) |
| 3 | **Open the file** (`open()`) – read the ELF header |
| 4 | **Verifies** the "magic number" to make sure **it's really an ELF** file - **\x7FELF** |
| 5 | **Reads the program header** – to determine **where** the different sections are and where in the VM they should be loaded |
| 6 | `mmap()` segments of the file into its VA (backed-file mem) and apply permissions (r/x, r/w) |
| 7 | **Relocate entries** – if needed, they are in the expected place in the VA |
| 8 | **Initialize symbols table** – so we can find entry points! |
| 9 | **Runs initialization code** – if any are set |
| 10 | **Returns handle** to dynamic library |

---

## Using exported function/variable (dynamic link)

**Exported function** or **exported variable**:

- Entities dynamic library set their **address in a data structure**
- Application **search** the entity within this data structure to use them

Windows:

```c
#include <windows.h>
#include <stdio.h>

typedef void (*hello_func)();

int main()
{
    HMODULE hLib = LoadLibrary("hello.dll");
    if (!hLib) {/* error */}

    hello_func hello_world =
        (hello_func)GetProcAddress(hLib,
                                "hello_world");
    if (!hello_world) {/* error */}

    hello_world();  // Call the function

    FreeLibrary(hLib);  // Cleanup
    return 0;
}
```

Linux:

```c
#include <dlfcn.h>
#include <stdio.h>

typedef void (*hello_func)();

int main()
{
    void *handle = dlopen("libhello.so",
                          RTLD_LAZY);
    if (!handle) { /* error */}

    hello_func hello_world =
        (hello_func)dlsym(handle, "hello_world");
    if (!hello_world) { /* error */}

    hello_world();  // Call the function

    dlclose(handle);  // Cleanup
    return 0;
}
```

---

## Windows DLL – LoadLibrary Search Order

```c
LoadLibrary("hello.dll")
```

How to find the DLL? Windows searches and returns the **first match**:

If given **relative/absolute path** – searches DLL at that path

If given **only filename** – there are multiple places Windows looks for the DLL:

- Directory where the process executable resides
- System directory
  - `$windir\system32` for 64bit process
  - `$windir\SysWOW64` for 32bit process
  - `$windir` is the windows installation path
- Current directory
- Paths list in PATH environment variable
- Paths list in `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths`
  - This is a place in the `registry` (Windows database we don't discuss)
- More…

Two callout questions raised on this slide:

- What if we place *malicious* hello.dll, in the application's path ? 😱
- What is I have *multiple* versions of hello.dll? 🥴

---

<!-- source pages 34-49 -->

## Windows DLL – Export Table

*Export tables are the Windows approach for dynamic libraries.*

- `LoadLibrary()` returned handle is the **DLL's BASE virtual address**
- DLLs are loaded **continuously** in the virtual memory

**Exported functions/variables** are stored in the **Export Table**. This data structure holds all the **exported entities**:

- **Ordinal** – Index in the table
- **Name** – Function/Variable name to be used during lookup
- **Relative Virtual Address** (RVA) – of the entity (offset of module base address)

`LoadLibraryA` is implemented in `kernel32.dll`.

*Figure: An export-table viewer listing kernel32.dll exports — columns E / Ordinal / Hint / Function / VirtualAddress / Demangler, with rows 1007 (0x03ef) LoadLibraryA 0x00042d80, 1008 (0x03f0) LoadLibraryExA 0x0003fb90, 1009 (0x03f1) LoadLibraryExW 0x0003c600, 1010 (0x03f2) LoadLibraryW 0x0003f7c0, 1011 (0x03f3) LoadModule 0x000402c0, 1012 (0x03f4) LoadPackagedLibrary 0x00057710; all Hint = N/A and Demangler = None. It shows that exports are identified by ordinal + name and carry an RVA.*

---

## Windows DLL – Build library

### Exporting with `__declspec(dllexport)`

`__declspec(dllexport)` tells the compiler to add the function to the export table.

```c
__declspec(dllexport) void hello_world(void){
    printf("Hello from C DLL with declspec!\n");
}
```

Build using visual studio compiler (we can also use gcc of course):

```shell
cl /LD hello_win_declspec.c /Fe:hello_win_declspec.dll
```

The export table:

| E | Ordinal | Hint | Function | VirtualAddress | Demangler |
|---|---------|------|----------|----------------|-----------|
| IC | 1 (0x0001) | N/A | `hello_world` | 0x00001000 | None |

When loading, `hLib` holds the **base VA** of the module in the process:
`hello_world` is at `hLib+0x1000`. (it is not always that simple…)

```c
HMODULE hLib = LoadLibrary("hello_win_declspec.dll");
```

### Exporting with a DEF file

We can also use a DEF file instead of `__declspec`.

```c
void hello_world(void) {
    printf("Hello from C DLL with DEF file!\n");
}
```

```text
LIBRARY hello_win_def
EXPORTS
    hello_world
```

Build using visual studio compiler (we can also use `gcc` of course):

```shell
cl /LD hello_win_def.c /Fe:hello_win_def.dll /link /DEF:hello_win_def.def
```

The export table:

| E | Ordinal | Hint | Function | VirtualAddress | Demangler |
|---|---------|------|----------|----------------|-----------|
| IC | 1 (0x0001) | N/A | `hello_world` | 0x00001000 | None |

### Compiling with C++ – Name Mangling

Now let's try to compile with C++ (not C!)

```cpp
__declspec(dllexport) void hello_world() {
    std::cout << "Hello from C++ DLL with mangling!\n";
}
```

Build using visual studio compiler (we can also use `gcc` of course):

```shell
cl /LD hello_win_cpp.cpp /Fe:hello_win_cpp.dll
```

The export table:

| E | Ordinal | Hint | Function | VirtualAddress | Demangler |
|---|---------|------|----------|----------------|-----------|
| IC | 1 (0x0001) | N/A | `?hello_world@@YAXXZ` | 0x000010d0 | Microsoft |

*Figure: The exported function name `?hello_world@@YAXXZ` is circled in red in the export-table view, highlighting the mangled name.*

```c
void* phello_world = GetProcAddress(hDll, "?hello_world@@YAXXZ")
```

- C++, unlike C, supports **overloading**, **objects and more…**
- To overcome the limitation of "Function Name", C++ compiler use **Name Mangling** which encodes entity name.
- **Mangler** states which algorithm is used to *mangle* or *demangle*

### C++ with `extern "C"`

But we can still make C++ function more compliant with C using **extern "C"**:

```cpp
extern "C" __declspec(dllexport) void hello_world(void) {
    std::cout << "Hello from C++ DLL with extern C!\n";
}
```

Build using visual studio compiler (we can also use `gcc` of course):

```shell
cl /LD hello_win_cpp_c.cpp /Fe:hello_win_cpp_c.dll
```

The export table:

| E | Ordinal | Hint | Function | VirtualAddress | Demangler |
|---|---------|------|----------|----------------|-----------|
| IC | 1 (0x0001) | N/A | `hello_world` | 0x000010d0 | None |

C++ compiler will make "`hello_world`" entry in the export table.

---

## Windows DLL – Finding the Symbol (`GetProcAddress`)

`GetProcAddress` searches the symbol `hello_world` in `hDll` export table, and if it exists, it returns a pointer to the entity (function/variable).

```c
void* phello_world = GetProcAddress(hDll, "hello_world")
```

---

## Windows DLL – Export Table Lookup

So how does `GetProcAddress` works? It parses the **export table in the PE**.

```c
HMODULE hModule = LoadLibraryA("hello_win_cpp_c.dll");
```

`hModule` is the base VA of the module (where it was loaded to).

Now, parse the PE to find the export table:

```c
// Get DOS header
PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) { /* error */ }
```

```c
// Get NT headers using DOS header
PIMAGE_NT_HEADERS ntHeaders =
        (PIMAGE_NT_HEADERS)((BYTE*)hModule + dosHeader->e_lfanew);
if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) { /* error */ }
```

```c
// Get export directory RVA from NT headers
DWORD exportDirRVA = ntHeaders->OptionalHeader.
        DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
if (exportDirRVA == 0) { /* error */ }
```

`exportDirRVA` is an RVA (not VA!). To find its address we need to add the base address.

`exportDirRVA` VA from RVA:

```c
PIMAGE_EXPORT_DIRECTORY exportDir =
    (PIMAGE_EXPORT_DIRECTORY)((BYTE*)hModule + exportDirRVA);
```

```c
// Get arrays of functions, names, and ordinals from their RVAs
DWORD* functions = (DWORD*)((BYTE*)hModule + exportDir->AddressOfFunctions);
DWORD* names = (DWORD*)((BYTE*)hModule + exportDir->AddressOfNames);
WORD* ordinals = (WORD*)((BYTE*)hModule + exportDir->AddressOfNameOrdinals);
```

```c
// Search for hello_world
for (DWORD i = 0; i < exportDir->NumberOfNames; i++)
{
    const char* currentName = (const char*)((BYTE*)hModule + names[i]);
    if(strcmp(currentName, "hello_world") == 0)
    {
        // Found it! Get the function address
        DWORD functionRVA = functions[ordinals[i]];
        return (HelloWorldFunc)((BYTE*)hModule + functionRVA);
    }
}
```

---

## Using exported function/variable (static link)

Dynamic linking works well, **but** it has its limitations:

- Manually load everything!
- No dependency management
- Runtime overhead - searching for the function
- Manual error handling - function not found, missing dynamic library
- No type checking – simple cast of the loaded pointer

By **linking to code** that automatically **loads** the dynamic library, **finds/loads** the entities and manages dependencies – things can get easier!

The generated code, along with the **PE/ELF** loader (the module we use during new process) in the OS, the above is being executed.

**While static linking solves some things, it (as always) introduces new challenges…**

---

## Windows DLL – Import library

Declare `hello_world()` to import from DLL.
It does not matter if `hello_world` in C or C++ (exported with name mangling).

```c
__declspec(dllimport) void hello_world(void);
```

Build using visual studio compiler (we can also use `gcc` of course):

```shell
cl main_win_static_c.c hello_win_declspec.lib
cl main_win_static_cpp.cpp hello_win_cpp.lib
```

*Figure: The two `.lib` names in the build commands are boxed in red, marking them as the import libraries.*

- The `.lib` files are **static libraries**.
- The static library is generated by the compiler (by default in VS compiler) when building the dynamic lib (when using gcc, `--out-implib` is also required to create the libs).
- The lib contains **indirect call** to `hello_world` via **Import Table**.
- Import table is updated during runtime by the **Windows PE Loader**.
- The lib file **does not contain** `hello_world` code and **does not call** it directly.

*Figure: A vertical call chain — "EXE (or another .dll)" → "Import table" → "hello.dll" → "hello_world()" — showing that the executable reaches the DLL function only through the import table.*

---

## PE Import Address Table (IAT)

Just like the *export table* is the PE, the **import table** is also part of the PE.

When the compiler "sees" `__declspec(dllimport) hello_world`, it adds hello world to the import table.
This means, the PE becomes a **dependent** on the imported DLL.

Meaning PE/DLLs (same in Linux) have dependency tree! (and a whole new set of challenges…)

`main_win_static_cpp.exe` import table (notice the name mangling):

| PI | Ordinal | Hint | Function | Module |
|----|---------|------|----------|--------|
| C++ | N/A | 0 (0x0000) | `?hello_world@@YAXXZ` | `C:\Users\green\OneDrive\Desktop\tests\hello_win_cpp.dll` |

`main_win_static_cpp.exe` dependency tree:

*Figure: A tree view — `C:\Users\green\OneDrive\Desktop\tests\main_win_static_cpp.exe` → `C:\Users\green\OneDrive\Desktop\tests\hello_win_cpp.dll` → `C:\WINDOWS\SysWOW64\kernel32.dll`, plus a direct `C:\WINDOWS\SysWOW64\kernel32.dll` child of the exe; a brace on the right is labelled "Goes deeper…".*

---

## Running Statically Linked DLL

When the **PE (.exe or .dll)** is loaded, **Windows PE loader** (user-mode module) **loads all the dependencies** and resolve their functions **before** the *executable* starts, or *dynamic library* is loaded.

*Figure: End-to-end flow diagram. Left: "Lib code" (.CPP) → compiler → DLL and .LIB; "main code" (.CPP) + LIB → compiler → EXE; the EXE is "execute"d.*

The stages of the flow:

1. **`CreateProcess()`** — kernel side:
   - Creates PCB
   - Allocates VM
   - Memory maps exe
   - Creates "main" thread
2. **User mode Win PE loader**:
   - Iterates import table
   - Loads DLLs (like `LoadLibrary`) with dll name (not full path)
   - Loads function/variable (like `GetProcAddress`) and **updates Import Address Table (IAT)**
   - Relocation fixes (talk about it in a bit…)

   A failure is an SEH error!
3. Execute `_start`
4. Call C-Runtime initialize In `msvcrt.dll` (like `argc`,`argv`)
5. Executes **main**
6. `hello_world` is called
7. Code in lib redirect to IAT
8. IAT now has the correct VA. Calls it
9. **Hello world**

---

## Relocation in DLL

- **Export table** contains RVAs
- Location of the <u>function</u> is $= base_{VA} + function_{RVA}$

Calling a function **within** the DLL uses a **relative jump**.
**But not everything can use relative VA**, like **access global variables** which requires <u>absolute</u> **virtual address.** *(Because this is **data** not code.)*

- The compiler chooses a ***preferred base address*** that is written in the PE
- The compiler **uses** the **preferred base address** to generate the code
- **If** the DLL is loaded to a **different** base address than the preferred address, the code must be fixed – i.e. **absolute addresses must be relocated (i.e. fixed)**.

**IMPORTANT – modifying the VM pages makes them private (r/w) and not shared (r) anymore.**

**Windows PE loader** is performing the relocations.

### Relocation Table

Relocation table is store under the `.reloc` section in the PE.

The relocation table **records all places** in the code where **absolute addresses exist** in the code (the `.text` section):

`.reloc` is split into pages aligned with 4KB (`0x1000`) covering `.text` section.
For each page, a relocation is described as:

- The **offset** of the instruction that needs to be updated
- The **type** of relocation (e.g., absolute address needs base address adjustment).

To relocate, the Windows PE loader calculates the Δ ***delta*** between the preferred VA and actual VA and **applies the delta to the code!**

```asm
Preferred base:  0x10000000
Original code:   mov rax, 0x10002000     ; absolute VA (uses preferred base)
New base:        0x15000000              ; loaded to different base
Patch:           0x10002000 → 0x15002000
```

### Example

Assume:

- Preferred Base Address: `0x10000000`
- Global Variable in `.data` section at offset: `0x2000`
- In `.code`, at `0x6000`, the instruction uses absolute address: `mov rax, 0x2000`

Linker's preferred absolute address for the variable:

$$Address_{Pref\;Base} + Offset_{Var} = 0x10000000 + 0x2000 = 0x10002000$$

In `.text` section, the code generated for the instruction at offset `0x6000`:

```asm
0x6000: mov rax, 0x10002000
```

In `.reloc`, page `0x6000` → `0x6FFF` (4k) the entry contains:

- Offset `0x000`
- `0xA` (`IMAGE_REL_BASED_DIR64`)

When the DLL is loaded:

- Preferred base address: `0x10000000`
- Actual base address: `0x15000000`

`0x10000000` ≠ `0x15000000` → Relocation required!

$$\Delta = 0x15000000 - 0x10000000 = 0x5000000$$

PE Loader iterates `.reloc` and finds `0x6000` entry and updates the code:

```asm
Before: 0x6000: mov rax, 0x10002000
```

New address $0x10002000 + \Delta = 0x10002000 + 0x5000000 = 0x15002000$

```asm
New: 0x6000: mov rax, 0x15002000
```

---

<!-- source pages 50-60 -->

## DLL — Relocation and COW

**WAIT!**

We said that dynamic library memory is **shared** among multiple processes!

So how can PE loader modify it? Won't it affect all the processes?!

Luckly — WE HAVE **COW (copy-on-write)!**
(remember from virtual memory lecture?)

If the PE loader needs to relocate addresses,
the **write operation** to the VM page **might** create a *private copy* of the VM page.

---

## Copy-On-Write (COW) — revisit, now that we have a good example

**Instead** of immediately copying memory, when a process shares memory, the OS allows **multiple processes to map their VA to the same physical memory** — sharing memory!

The same process happens when using *fork*. **The process memories are shared.**
This is why I wrote "it's not true" when we talked about *fork*!

Copies are created **only** when a process **modifies (i.e. writes)** to the shared data.

OS marks the shared memory pages as *read only*

- If a process **writes to a shared memory**:
    - **MMU** detects a **write to a *read only* page** and raises a **page fault**
    - OS creates a **new private copy** of the page
    - OS **Updates** process **page table** to **refer to the new page**
- **Write** is done to the new page
- **Other processes** continue to refer to the original page

---

## DLL — Relocation and COW: Example 1

**P1** loads **a.dll** to **preferred** memory:

- **Relocation? No**
- **COW? No**

**P2** loads **a.dll** to **non-preferred** memory (preferred is taken):

- **Relocation? Yes**
- **New Page Loaded? Yes**
- **COW? No**

The relocated pages of P2 are private!

---

## DLL — Relocation and COW: Example 2

**P1** loads **a.dll** to **non-preferred** memory:

- **Relocation? Yes**
- **COW? No**

**P2** loads **a.dll** to **preferred** memory:

- **Relocation? No**
- **New pages loaded? Yes**
    - Pages modified by P1 are no longer backed by the file
    - P2 maps the original clean version from the DLL file
- **COW? No**

**P2** alters a shared page from a.dll

- **COW? Yes, to <u>P2</u>**

---

## DLL — Relocation and COW: Example 3

**P1, P2** loads **a.dll** to **preferred** memory:

- **Relocation? No**
- **COW? No**

**P3 loads to non-preferred** memory:

- **Relocation? Yes**
- **COW? No**

**P2** modifies the global variable `int myX` in a.dll → **COW? Yes**

For each process, **state** if `myX` is **shared/private**, its **permissions** and page **type** (i.e. anonymous (**A**) or file-backed (**F/B**)):

| Process | Answer |
|---|---|
| P1 | FileBack shared (read) |
| P2 | Anon private (read/write) |
| P3 | FileBack shared (read) |

---

## Lazy & Eager Load (or bind) `<static link of dynamic lib>`

**Eager load** — The OS resolves the exported functions/variables when module (executable or dynamic library) **is loaded.**

**Lazy load** — The OS resolves the exported function/variable upon its **first use.**

Both Windows and *nix support both and it can be configured during link-time

**Windows** is **Eager** by default
**Linux** is **Lazy** by default

---

## Dynamic Library Entrypoint

In many cases, we need to execute code on events of the dynamic library, like:

- **Process loads** - first `LoadLibrary/dlopen` call
- **Thread loads** - new thread created (since dynamic lib was loaded)
- **Thread detaches** - thread exits
- **Process detaches** — Process terminated or complete detaches dynamic lib

The entrypoint is called on these events for initializations/cleanup.
It can also tell the loader if the dynamic library has been loaded/initialized successful

Windows and *nix are taking different approaches (of course….)

### DLL Entrypoint — `DLLMain`

```c
BOOL DllMain(HMODULE hModule,
             DWORD  ul_reason_for_call,
             LPVOID lpReserved)
```

Every `LoadLibrary/FreeLibrary` in the same process counts the number of times the DLL has been loaded or freed.

`DllMain`'s address is stored in the PE header under `OptionalHeader.AddressOfEntryPoint`

- **`hModule`** — handle to the loaded module (loaded base address)
- **`ul_reason_for_call`**:
    - **`DLL_PROCESS_ATTACH`** — DLL loaded into process (first LoadLibrary)
    - **`DLL_PROCESS_DETACH`** — DLL detached from process (last FreeLibrary)
    - **`DLL_THREAD_ATTACH`** — New thread created **after** loading the DLL
    - **`DLL_THREAD_DETACH`** — A thread exits
- **`lpReserved`** - Non-NULL if called by process termination

---

<!-- source pages 61-73 -->

## DLL Hell

*Figure: a small image of Elmo with raised arms standing in flames, illustrating the "hell" of conflicting DLL versions.*

- Multiple applications depend on **different versions** of the same DLL
- Overwriting or upgrading a DLL can **break** multiple applications
- Caused years of **deployment nightmares** on Windows
- WinSxS (Side-By-Side) tries to mitigate this problem by adding DLLs "version number"

**Still** a serious pain point

**Linux**, on the other hand, avoids **hardcoded global search paths** and **shared binary overwrites…**

---

## *nix (not Mac) Shared Object

In Linux, dynamic libraries are called Shared Objects (.so).
(not the be confused with Mac's .dylib, which are Mach-O format, and not ELF)

They behave similarly to Windows DLLs, but with key differences in **loading, lookup, and initialization.**

**Like DLLs:**

- Can be loaded at startup or dynamically (`dlopen`)
- Symbols exported for runtime lookup (`dlsym`)

**But Unlike DLLs:**

- Prefers per-binary search rules (`RPATH`, `RUNPATH` stored ELF)
- Can support **multiple versions** side by side

It avoids DLL Hell, but, as always, introduces other challenges…

### dlopen Search order

```c
dlopen("hello.so", RTLD_LAZY)
```

How to find the SO? Linux searches and returns the **first match**:

If given **relative/absolute path** – searches SO at that path

If given **file name**:

- `DT_RPATH` entry (a.k.a RPATH) – Path stored in `.dynamic` section in ELF
  - `$ORIGIN` variable signifies SO directory
  - Ignored if `DT_RUNPATH` exists
- `LD_LIBRARY_PATH` path list environment variable
- `DT_RUNPATH` – like RPATH, but comes **after** LD_LIBRARY_PATH
- `/etc/ld.so.conf` – file(s) containing list of paths
- `/lib/`
- `/usr/lib/`

- **NOTICE! It does not look (by default) in the current/.so directory!**

---

## *nix (not Mac) SO — Symbol Resolution

**Symbol resolution** is the process of finding the actual **memory (virtual) address** of an entity (function or variable) during program **startup** or **runtime**.

Once a symbol is resolved, it gets patched into the `GOT` (Global Offset Table), and function calls go directly.

Once a symbol is resolved, it is **not** resolved again.

**Symbol resolution can occur at:**

- During start up
- First call to unresolved entity (function or variable) – if using lazy initialization (`RTLD_LAZY`)
- Calling `dlsym()`

---

## Procedure Linkage Table (PLT) — Jump Table for Functions

PLT is a part of the **.text (code) section**

Each external function call (like `printf`, `dlopen`, etc.) gets a PLT stub
(stub = same signature, different implementation)

On **first call**, this stub **jumps into the dynamic linker** to resolve the function

After resolution, the stub **jumps directly to the resolved address** (like a **trampoline**)

This indirection enables **lazy resolution** and avoids patching actual code

This "trick" is also called *trampoline function*

```asm
.text section:
call printf@plt         ; your code
  ↓
PLT entry:
jmp [printf@got]        ; indirect call via GOT
```

```text
On first call:
  [printf@got] → points to dynamic linker resolver (_dl_runtime_resolve)
                 which updates printf@got

After resolution:
  [printf@got] → updated to point to the real printf()
```

---

## Global Offset Table (GOT) — Shared Address Table

GOT is a part of the **.data section (read/write)**

- Holds **pointers to resolved addresses** — functions or global variables
- At startup: each entry points to the next PLT instruction (i.e., resolution stub)
- After resolution: GOT entry is **patched** to point directly to the function
- Updated by the dynamic linker

The GOT provides a **single place** to patch addresses of imported symbols (unlike windows)
It's a cleaner separation between **code (.text)** and **data (.got)**

**NOTICE:**
GOT pages start as **shared file-backed memory,**
but become **private** when patched **(COW)** during relocation or lazy resolution

---

## PLT + GOT: First Call (Lazy Initialization) — a bit simplified

*Figure: a three-box diagram — `hello_world()` in user code arrows into the `.text` box holding `call hello_world@plt`; a green **GOT (data section)** box on the right; and a **PLT (executable section)** box below holding the four-instruction stub. An orange arrow marks the currently executing instruction and advances one step per frame.*

The PLT stub for `hello_world` contains:

```asm
jmp [hello_world@got]
pushq <index of hello_world in .dynsym>
jmp _dl_runtime_resolve_x86_64
jmp [hello_world@got]
```

The trace, step by step:

1. **Call site (.text).** `hello_world()` reaches the `.text` instruction `call hello_world@plt`. At this moment the GOT (data section) entry holds *"Address to next instruction in PLT"*.
2. **PLT instruction 1 — `jmp [hello_world@got]`.** The jump goes indirectly through the GOT entry, which points back to the **next instruction in the PLT** (the resolution stub).
3. **PLT instruction 2 — `pushq <index of hello_world in .dynsym>`.** The symbol index is **pushed on the stack**.
4. **PLT instruction 3 — `jmp _dl_runtime_resolve_x86_64`.** Function in ld.so (must be) loaded **without** lazy load!
5. **Inside `_dl_runtime_resolve_x86_64`:**
   - Loads SO (if not loaded) like `dlopen([so name])` – **name only**
   - Performs relocation if needed. Similar to windows → with slight changes:
     - Instead of .reloc like in windows, it uses .rela.dyn and .rela.plt
     - Access to global variable is via GOT → **relocation patches GOT!**
     - <u>Pro:</u> no code patching, <u>Con:</u> more steps on each access to variable
       - NOTICE: in some cases, Linux needs similar code patching like Win
   - Loads `hello_world` like `dlsym()`
   - Updates GOT with `hello_world` VA in the correct index
6. **PLT instruction 4 — `jmp [hello_world@got]`.** The GOT entry now holds *"Address of hello_world"*, so this jump goes straight to the real function.

*Note: the `.text` box reads `jmp hello_world@plt` in the first frames of this sequence and `call hello_world@plt` in the later ones — an inconsistency in the original slides.*

---

<!-- source pages 74-90 -->

## \*nix (not Mac) SO — `dlopen` flags & effect

`dlopen(PATH, FLAGS)` loads a shared object, but the FLAGS also control **when symbols are resolved** and **whether they are visible to other libraries**.

**Visibility:**

- `RTLD_LOCAL` (default) — Symbols in .so **are not available** to other shared objects
- `RTLD_GLOBAL` — Symbols in .so **are** available are resolvable from other shared objects

**Timing:**

- `RTLD_LAZY` (default) — Delay symbol resolution until first use
- `RTLD_NOW` — Resolves all symbol resolution immediately

There are additional flags, but these are the most common.

---

## \*nix (not Mac) SO — Global symbol search

```c
dlsym(RTLD_DEFAULT, "hello_world");
```

- `RTLD_DEFAULT` — **iterate** through the Symbol Tables and load the **first occurrence** of `hello_world`.

In case of multiple occurrences of `hello_world`:

- `RTLD_NEXT` — check only the next shared object in the list. Doesn't tell when we have iterated all the list.

`dl_iterate_phdr()` — For each loaded SO: callback the given function:

```c
int callback(struct dl_phdr_info *info, size_t size, void *data)
```

To print the list of SO in the process:

```bash
cat /proc/[pid or self]/maps | grep '\.so'
```

---

## \*nix (not Mac) SO — Dynamic Symbols Table

Instead of export tables, shared object uses **Symbol Table**.

Stored at `.dynsym` section in ELF.

`dlopen` implemented in libc.so.6:

```bash
tcs@tc-pc:~$ readelf -s /lib/x86_64-linux-gnu/libc.so.6 | grep dlopen
  1887: 0000000000090680   154 FUNC    GLOBAL DEFAULT   15 dlopen@GLIBC_2.2.5
```

*Figure: the readelf output line is annotated field-by-field — `0000000000090680` = RVA, `154` = Size (based on type), `FUNC` = Type, `GLOBAL` = Bind, `DEFAULT` = Visibility, `15` = Index, `dlopen@GLIBC_2.2.5` = Name. For the Size field: FUNC – function size, Object – size of data.*

---

## \*nix (not Mac) SO — Visibility

Each exported entity has a **visibility** scope which provides access control to the shared object.

Visibility can be determined for **all** entities in the SO:

- **Compiler** flag: `-fvisibility=[visibility]`
- **Code** attribute: `__attribute__((visibility("default")))`
- **Linker** flags `-Bsymbolic, -Bsymbolic-functions` Can determine symbolic timing

**Visibilities:**

- `default`:
  - similar to `__declspec(dllexport)`
  - can be **overridden** by same symbol by other shared objects!
- `hidden`: symbol not in symbol table
- `Protected`:
  - similar to `default`, but **cannot be overridden** from within the same SO
- `internal`: like `hidden`, but state to the compile that it is used only within the shared object for optimizations (including callbacks)

**Notice:** `dlopen` *can* override some of the flags set at compile time.

### `default`

- similar to `__declspec(dllexport)`
- can be **overridden** by same symbol by another shared objects!
- Default in older `gcc`

hello1.so:

```c
__attribute__((visibility("default")))
void hello_world() { printf("Hello\n"); }
```

hello2.so:

```c
__attribute__((visibility("default")))
void hello_world() { printf("bye\n"); }
```

Executable or another .so:

```c
void* lib1 = dlopen("hello1.so", RTLD_LAZY|RTLD_GLOBAL);
void* lib2 = dlopen("hello2.so", RTLD_LAZY|RTLD_GLOBAL);

void* phw = dlsym(RTLD_DEFAULT, "hello_world"); // load from symbol table
((void(*)())phw)(); // what does it print? bye
```

### `hidden`

- Symbol not exported
- `dlopen` **cannot** override this, as the binary is missing the information to export entity

hello1.so:

```c
__attribute__((visibility("hidden")))
void hello_world() { printf("Hello\n"); }
```

Executable or another .so:

```c
void* lib1 = dlopen("hello1.so", RTLD_LAZY|RTLD_GLOBAL);

// load from hello1.so
void* phw = dlsym(lib1, "hello_world"); // will return NULL!
```

### `protected`

- similar to `__declspec(dllexport)`
- **cannot be overridden** by same symbol by another shared objects!

hello1.so:

```c
__attribute__((visibility("protected")))
void hello_world() { printf("Hello\n"); }
```

hello2.so:

```c
__attribute__((visibility("protected")))
void hello_world() { printf("bye\n"); }
```

Executable or another .so:

```c
void* lib1 = dlopen("hello1.so", RTLD_LAZY|RTLD_GLOBAL);
void* lib2 = dlopen("hello2.so", RTLD_LAZY|RTLD_GLOBAL);

void* phw = dlsym(RTLD_DEFAULT, "hello_world"); // load from symbol table
((void(*)())phw)(); // what does it print? Hello
```

### `internal`

- Symbol not exported
- `dlopen` **cannot** override this, as the binary is missing the information to export entity
- Like `hidden`, but signals the compiler for more aggressive optimizations (e.g. inline)

hello1.so:

```c
__attribute__((visibility("internal")))
void hello_world() { printf("Hello\n"); }
```

Executable or another .so:

```c
void* lib1 = dlopen("hello1.so", RTLD_LAZY|RTLD_GLOBAL);

// load from hello1.so
void* phw = dlsym(lib1, "hello_world"); // will return NULL!
```

---

## \*nix SO — Build library

### C function with `default` visibility

```c
__attribute__((visibility("default")))
void hello_world(void) {
    printf("Hello from Linux C shared library!\n");
}
```

Build using gcc:

```bash
gcc -shared -fPIC -o libhello_linux.so hello_linux.c
```

The dynamic symbols table:

```bash
tcs@tc-pc:/mnt/c/Users/green/OneDrive/Desktop/tests$ readelf --dyn-syms libhello_linux.so | grep hello_world
     6: 0000000000001119    26 FUNC    GLOBAL DEFAULT   14 hello_world
```

### C++ function with `default` visibility

```cpp
__attribute__((visibility("default")))
void hello_world() {
    std::cout << "Hello from Linux C++ shared library with mangling!\n";
}
```

Build using g++ (C++ gcc compiler):

```bash
g++ -shared -fPIC -o libhello_linux_cpp.so hello_linux_cpp.cpp
```

The dynamic symbols table:

```bash
tcs@tc-pc:/mnt/c/Users/green/OneDrive/Desktop/tests$ readelf --dyn-syms libhello_linux_cpp.so | grep hello_world
    10: 0000000000001159    36 FUNC    GLOBAL DEFAULT_   14 _Z11hello_worldv
```

### C++ function with C compatibility and `default` visibility

```cpp
extern "C" __attribute__((visibility("default")))
void hello_world(void) {
    std::cout << "Hello from Linux C++ shared library with extern C!\n";
}
```

Build using g++ (C++ gcc compiler):

```bash
g++ -shared -fPIC -o libhello_linux_cpp_c.so hello_linux_cpp_c.cpp
```

The dynamic symbols table:

```bash
tcs@tc-pc:/mnt/c/Users/green/OneDrive/Desktop/tests$ readelf --dyn-syms libhello_linux_cpp_c.so | grep hello_world
    10: 0000000000001159    36 FUNC    GLOBAL DEFAULT   14 hello_world
```

---

## Using exported function/variable (static link)

Dynamic linking works well, **but** it has its limitations:

- Manually load everything!
- No dependency management
- Runtime overhead - searching for the function
- Manual error handling - function not found, missing dynamic library
- No type checking – simple cast of the loaded pointer

By **linking to code** that automatically **loads** the dynamic library, **finds/loads** the entities and manages dependencies – things can get easier!

The generated code, along with the **PE/ELF** loader (the module we use during new process) in the OS, the above is being executed.

**While static linking solves some things, it (as always) introduces new challenges…**

---

## \*nix SO — Import library

Unlike DLL (which are PE), SO (which are ELF) do not require special declaration.

Linux takes a different approach, the linkage information always exists within the ELF file structure. (no need for separate static library like windows)

Here, linking just adds the SO name as a dependency. symbols are resolved at runtime! (no static resolution like in .lib)

```bash
gcc main_linux_static_c.c -L. -lhello_linux -o main_linux_static_c
g++ main_linux_static_cpp.cpp -L. -lhello_linux_cpp -o main_linux_static_cpp
```

*Figure: the `-lhello_linux` and `-lhello_linux_cpp` flags are boxed in red; an arrow from `-lhello_linux_cpp` shows it resolves to the file `libhello_linux_cpp.so`.*

Instead of DLL's import table, ELF has a `.dynamic` section. The linker automatically detects and registers "external dependency" that is located in the shared object.

The linker records the shared object dependency in the `.dynamic` section. The symbol remains undefined (UND) and is resolved at runtime.

`.dynamic`:

```bash
tcs@tc-pc:/mnt/c/Users/green/OneDrive/Desktop/tests$ readelf -d ./main_linux_static_cpp

Dynamic section at offset 0x2d88 contains 29 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libhello_linux_cpp.so]
```

`.dynsym`:

```bash
tcs@tc-pc:/mnt/c/Users/green/OneDrive/Desktop/tests$ readelf -s main_linux_static_cpp | grep 'hello'
     4: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND _Z11hello_worldv
```

---

## Dynamic Library Entrypoint — `DT_INIT(_ARRAY)` / `DT_FINI(_ARRAY)`

```c
__attribute__((constructor)) void my_init() {
    printf("Library loaded\n");
}

__attribute__((destructor)) void my_fini() {
    printf("Library unloaded\n");
}
```

In the ELF, `.dynamic` section contains:

- **DT_INIT** – a function pointer when SO is loaded (legacy)
- **DT_INIT_ARRAY** – array of function pointers when SO is loaded (modern)
- **DT_FINI** – a function pointer when SO is completely freed (legacy)
- **DT_FINI_ARRAY** – array of function pointers when SO is freed (modern)

Function signature is `void(void)`.

There is **no support** for thread init/fini.
There is **no support** to tell if initialization/finalization failed.

---

## PIC – Position Independent Code

```bash
gcc -shared -fPIC -o libhello_linux.so hello_linux.c
```

In DLL, function addresses are **relative** to the base address (RVA).

In Linux, we can control if the shared object uses relative addresses or not.

*Position Independent Code* (**PIC**) – The SO uses relative addresses.

Without `-fPIC`, the generated code uses **absolute addresses** even for functions, increasing **relocations**.

PIC (or RVA) comes with a cost in runtime (need to calculate absolute address).

Executables can safely use absolute addresses — they are loaded at a fixed location.

If there is a scenario where absolute addresses are all known at the compile-time, it is better **not to choose PIC**. Like in executables…

---

## Going back to hello

```bash
tcs@tc-pc:~/stuff$ gcc -c hello_main.c -o hello_main.o
tcs@tc-pc:~/stuff$ ld hello_main.o libhello.a -o hello_main
ld: warning: cannot find entry symbol _start; defaulting to 0000000000401000
ld: libhello.a(hello.o): in function `hello_world':
hello.c:(.text+0x13): undefined reference to `puts'
```

- `_start` in not in our code! It is part of C-Runtime (CRT)
- CRT `_start` entrypoint is calling to `main()`
- Also, libc.so contains puts

```bash
tcs@tc-pc:~/stuff$ ld \
  /usr/lib/x86_64-linux-gnu/crt1.o \
  /usr/lib/x86_64-linux-gnu/crti.o \
  /usr/lib/x86_64-linux-gnu/crtn.o \
  hello_main.o libhello.a \
  -dynamic-linker /lib64/ld-linux-x86-64.so.2 \
  -lc \
  -o hello_main
tcs@tc-pc:~/stuff$ ./hello_main
hello world
```

*Figure: annotations on the `ld` command — the three `crt*.o` files are braced as "Link to CRT", `hello_main.o libhello.a` is "Link to static library", `-dynamic-linker /lib64/ld-linux-x86-64.so.2` is "Choose dynamic linker", and `-lc` is "Depend on libc.so".*
