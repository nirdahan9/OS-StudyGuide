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
