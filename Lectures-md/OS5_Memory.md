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
