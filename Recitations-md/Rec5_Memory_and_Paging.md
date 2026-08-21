# Recitation 5 — Memory, Virtual Memory and Paging

> Study notes converted from the recitation slide deck `Rec5_OS.pdf` (90 pages) — Operating Systems, Reichman University, Spring Semester 2026.
> Lecturer: Tsvi Cherny-Shahar · TAs: Daniel Karalnik, Liam Tal.
> Faithful conversion: every substantive item on the source pages appears here, and nothing has been added from outside them. Figures are rendered as italic *Figure:* descriptions.
> `<!-- source pages a-b -->` comments mark the original page ranges.

---

<!-- source pages 1-16 -->

## Table of Contents

1. Intro to Physical Memory
2. The Evolution of Memory Management
3. Virtual Memory
4. Virtual Memory to Physical Memory Mapping
5. The Memory Management Unit (MMU)
6. Paging
7. Page Swapping

---

## Intro to Physical Memory

### What is Physical Memory?

**Physical Memory**
The actual RAM installed in a computer, organized as a **contiguous address space** accessible by the CPU.

- Managed by the **OS** to allocate space for processes and kernel
- Divided into **fixed-size frames** or **variable partitions**
- Addresses are **physical locations** in the RAM chip

*Figure: A vertical stack representing hardware RAM, from top to bottom: Kernel (blue), Process A, Process B, Process C (green), Free (red), HW Memory (gray) — showing physical memory as one contiguous space carved up between the kernel, processes and free space.*

### External vs Internal Fragmentation

**External Fragmentation**

*Figure: A memory column alternating Process A, Free, Process B, Free, Process C — the free regions are scattered between allocated blocks.*

Warning: Gaps between blocks. Total free space is enough, but no single gap fits a large request.

**Internal Fragmentation**

*Figure: A memory column of Process A, Process B, Process C, Process D, Process E where each block extends into a gray unused strip on its right — the waste sits inside each allocated block, not between blocks.*

Warning: Wasted space within blocks. Fixed-size allocation wastes the unused portion.

### Why Not Direct Physical Addressing?

Problems with Direct Access: If each process accessed physical memory directly, we would face serious issues.

- **No Process Isolation**: Process A could overwrite Process B's memory
- **No Relocation**: Programs must know exact physical addresses at compile time
- **Fragmentation**: Memory becomes scattered and unusable over time
- **Limited Multiprogramming**: Hard to run multiple programs efficiently

---

## The Evolution of Memory Management

### What are Virtual and Physical Addresses?

**Physical Address**
The actual location in RAM hardware. Example: byte `0x5000` refers to a specific cell in the RAM chip.

**Virtual Address**
An address used by programs. The hardware translates it to a physical address before accessing RAM.

- Programs **only see virtual addresses** — they never access physical RAM directly
- Hardware (MMU) **translates** every memory access automatically
- Each process has its **own virtual address space** starting at 0
- The **same virtual address** in different processes maps to **different physical locations**

### How Virtual Addresses Enable Isolation

*Figure: Process A holds VA: 0x1000 and Process B holds VA: 0x1000 (annotated "Same VA!"); both arrows feed into a central "MMU Translates" box, which emits arrows into Physical RAM to PA: 0x5000 and PA: 0x8000 respectively (annotated "Different PA!").*

Benefits of Virtual Addresses:

- **Isolation**: Processes cannot access each other's memory
- **Flexibility**: OS can place data anywhere in physical RAM
- **Simplicity**: Every process thinks it has memory starting at address 0

### Why do we bulk translate memory addresses?

Translation Efficiency: If we translated each virtual address individually, it would require frequent lookups, significantly increasing memory access time.

- Individual translation introduces considerable overhead
- Excessive context switches and performance degradation
- **Bulk translation** returns a **set of relevant addresses** (concept of locality)
- Reduces translation frequency and minimizes CPU stalls

Key Insight: Bulk translation allows each translation to cover an entire page (bulk of address), not just a single address!

### Base and Bound Addressing

**Base and Bound**
Early technique using two registers to define a process's memory region.

- **Base**: Where process starts
- **Bound**: Max allowed address
- **Translation**: $\text{VA} + \text{Base} = \text{PA}$

*Figure: Physical memory column — Kernel, Proc A (Base=1000), Proc B (Base=5000), Free Space, HW Mapped. Two "Base+Offset" arrows point from a side box into Proc A and Proc B: VA 50 → PA 1050, and VA 500 → PA 5500.*

### Limitations of Base and Bound

- **External Frag.**: Small gaps unusable
- **Internal Frag.**: Wasted space in blocks
- **Contiguous**: Process in one block
- **Compaction**: Costly to defragment
- **No Sharing**: Can't share libraries
- **No Growth**: Can't expand in place

*Figure: Memory column — Kernel, Process A, a thin "Int. Frag." strip labelled Internal, Process B, an "Ext. Frag." strip labelled External, Process C, another "Ext. Frag." strip, then Free — illustrating both waste types coexisting under base-and-bound allocation.*

### Memory Segmentation: Reducing External Fragmentation

**Segmentation**
Segments allow better memory utilization by fitting into available free space.

- Heap and Stack grow independently
- Segments can fit into gaps
- Process split into logical parts

*Figure: A memory column of Code, a thin Gap strip (labelled "Small gaps"), Data, a thin Unused strip (labelled "Wasted"), Heap, Stack, Gap — showing segments packed into free space with small leftover gaps and internal waste.*

Warning: External fragmentation reduced but internal fragmentation still exists within segments!

### How Segmentation Works

**Segment Table**
Each process has a segment table with entries for each segment:

- **Base**: Starting physical address
- **Limit**: Segment size (bounds check)
- **Permissions**: R/W/X bits

*Figure: A Virtual Address split into "Seg #" and "Offset". The Seg # indexes a Segment Table with entries `0: Base=1000, Lim=500`, `1: Base=4000, Lim=800`, `2: Base=6000, Lim=200`, `...`; a "Base+Off" arrow leads into Physical Memory containing Seg 0 @ 1000, Seg 1 @ 4000, Seg 2 @ 6000.*

Translation:

$$\text{Virtual Address} = \text{Segment \#} + \text{Offset}$$
$$\text{Physical} = \text{Base}[\text{Seg\#}] + \text{Offset}$$

(If Offset > Limit → Seg Fault)

### Why We Need Paging

Segmentation Still Has Problems: Even with segmentation, we face issues that motivate a better solution.

- **Variable-sized segments** still cause external fragmentation
- **Entire segment** must be in memory (no partial loading)
- **Segment table** grows with number of segments
- **Sharing** is complex (need matching segment sizes)

The Solution: Paging — Fixed-size pages eliminate external fragmentation entirely and enable partial loading, sharing, and efficient memory management!

---

<!-- source pages 17-34 -->

## Virtual Memory

### What is Virtual Memory?

**Virtual Memory — Expanding upon Virtual Addresses**

Virtual memory is an abstraction that allows processes to **use more memory than physically available**, providing each process its own isolated address space.

- The OS uses **paging** to map virtual addresses to physical memory
- Enables **efficient memory management**, **process isolation**, and **swapping**

*Figure: Three stacked boxes labelled Process A (Virtual), Process B (Virtual), Process C (Virtual) with a "Mapping" arrow pointing to a second stack labelled Process A (Physical), Process B (Physical), Swapped to Disk.*

---

### Paging and Swapping

**Paging** — Dividing memory into fixed-size blocks called **pages** (typically 4KB).

- Virtual memory $\rightarrow$ **Pages**
- Physical memory $\rightarrow$ **Frames**
- **Page table** maps pages to frames
- Eliminates **external fragmentation**

**Swapping** — Moving pages between RAM and disk when physical memory is full.

- **Page out**: RAM $\rightarrow$ Disk
- **Page in**: Disk $\rightarrow$ RAM
- Triggered by **page faults**
- Enables using more memory than available RAM

---

### Page States: Clean and Dirty

**Clean Page** — Unmodified; matches its backing store.

- Can be **discarded** anytime
- Just reload from backing store if needed

**Dirty Page** — Modified in memory; differs from backing store.

- **Must be saved** before discard
- Written to swap or file

**Anonymous Pages (heap, stack)**

- `malloc()` $\rightarrow$ **Clean** (no backing store yet) $\rightarrow$ Write $\rightarrow$ **Dirty**
- Evict dirty $\rightarrow$ Write to **swap**, allocate swap slot
- Load from swap $\rightarrow$ **Dirty** + swap slot **freed** (must re-swap if evicted again)

---

### 32-bit vs 64-bit Systems

**32-bit CPUs**

- Address up to **4GB** RAM
- 32-bit OS runs **only** 32-bit programs

**64-bit CPUs**

- Theoretically **16 exabytes**
- **Wider registers**, better multi-threading
- Runs **both** 32-bit and 64-bit programs

**Performance:** 64-bit CPUs support wider registers, more memory, and better multi-threading capabilities.

#### 32-bit vs 64-bit Virtual Memory

*Figure: Left, a "32-bit (4GB max)" stack holding Thread A, Thread B, Thread C, with a red cut-off line and a greyed "Beyond 4GB" band labelled "Cannot address!". An "Upgrade" arrow points right to a "64-bit (16 EB theoretical)" stack holding Thread A, Thread B, Thread C, Thread D plus a large remaining "Vast Address Space" band.*

---

### Types of Virtual Memory

**Anonymous Memory**

- **Not associated with a file**
- Used for **heap, shared memory, stack**
- Allocated via `mmap(MAP_ANONYMOUS)`
- When paged out, stored in **swap space**

**File-Backed Memory**

- Mapped from a **file on disk**
- Used for **executables, shared libraries**
- When paged out, can be **reloaded from file**

#### Virtual Memory Diagram

*Figure: Under the label "Anonymous", a stack of Heap (Anonymous), Shared Memory (Anonymous), Stack (OS Managed) has an "Evict to Swap" arrow to "Swap Space (No backing file)". Below, under the label "File-Backed", a "File Mapped Page" box has a "Reload from File" arrow to "Disk File (Original source)".*

---

### What is mmap()?

**Memory Mapping** — `mmap()` maps files or memory directly into a process's virtual address space.

Instead of `read()`/`write()`:

- File contents appear as **memory**
- Read/write using **pointers**
- No explicit I/O syscalls after mapping

```c
ptr = mmap(fd, ...);
char c = ptr[0];   // reads file
ptr[1] = 'A';      // writes file
```

*Figure: A "Virtual Memory" box stacking Code, Heap, mmap region, Stack; an arrow labelled "mapped" links the mmap region to a "File on Disk" box beside it.*

**Key Insight:** `mmap()` returns a pointer—use it like any memory!

---

### The Page Cache

**Page Cache** — Kernel memory that **caches file data** in RAM for faster access.

Why cache file data?

- Disk access: **milliseconds**
- RAM access: **nanoseconds**
- Avoid repeated disk reads

*Figure: A vertical chain — User Process (marked "Fast (ns)") linked by "copy or map" to Page Cache (Kernel RAM), linked by "read/write" to Disk (marked "Slow (ms)").*

**Key Insight:** Both `read()`/`write()` and `mmap()` use the **same page cache**!

**Dirty pages** in cache must be written back to disk (writeback).

---

### Buffered I/O vs. Memory-Mapped I/O: Read & Write

**Buffered I/O**

- **READ** — `read(fd, buf, n)`
  - Disk $\rightarrow$ Page Cache
  - Page Cache $\rightarrow$ **copy** to user buffer
- **WRITE** — `write(fd, buf, n)`
  - User buffer $\rightarrow$ **copy** to Page Cache
  - Page marked **dirty**
  - `fsync()` forces write to disk
- **2 copies per operation**

**Memory-Mapped I/O**

- **READ** — access `ptr[i]`
  - Page fault $\rightarrow$ load to Page Cache
  - **Direct access** (no copy)
- **WRITE** — `ptr[i] = x`
  - Modify Page Cache **directly**
  - Page marked **dirty**
  - `msync()` forces write to disk
- **Zero-copy access**

#### Buffered I/O vs. Memory-Mapped I/O (Diagram)

*Figure: Two parallel three-layer stacks. Buffered I/O — Disk File $\leftrightarrow$ (read/write) $\leftrightarrow$ Page Cache $\leftrightarrow$ (copy) $\leftrightarrow$ User Process, annotated "2 copies per I/O". Memory-Mapped I/O — Disk File $\leftrightarrow$ (writeback) $\leftrightarrow$ Page Cache $\leftrightarrow$ (direct) $\leftrightarrow$ User (Mapped), annotated "Zero-copy access".*

**Writeback:** Dirty pages are written to disk by kernel **periodically** or when explicitly requested (`fsync`/`msync`).

---

### Private vs. Shared Memory in Virtual Memory

**Private Memory**

- Exclusive to a process
- Heap (`malloc`)
- Stack

**Shared Memory**

- Dynamic libraries
- `mmap()` regions
- IPC mechanisms

**Copy-on-Write (COW):** Optimizes `fork()`: pages are **shared until modified**, saving memory and time!

*Figure: Process 1 has a stack of regions bracketed as Private — Heap (`malloc()`), Stack (Function Calls) — and as Shared — Shared Library (`.so`), Shared Memory (`mmap()`), COW Page (Forked). A solid arrow labelled "Same physical frame" links the Shared Library (`.so`) row to Process 2's Shared Library in P2, which sits above Shared Memory in P2; a dashed arrow labelled "Copy on write" links the COW Page (Forked) to Process 2's New Page (COW Write).*

---

## Virtual Memory to Physical Memory Mapping

### Virtual to Physical Address Translation

**Address Translation** — The OS and MMU (Memory Management Unit) work together to translate virtual addresses to physical addresses.

- The **Page Table** stores mappings of **VPNs** to **PFNs**
- A **TLB** (Translation Lookaside Buffer) caches recent translations
- If a page is **not in memory**, a **page fault** occurs

#### Walking a translation, step by step

**Step 1: Lookup** — The virtual address space contains Virtual Page 0, Virtual Page 1, Virtual Page 2, and Virtual Page 3 (On Disk). An arrow labelled "Step 1: Lookup" goes from the virtual address space into the **Page Table**, whose entries are:

| Page Table entry |
| --- |
| VPN 0 $\rightarrow$ PF 2 |
| VPN 1 $\rightarrow$ PF 5 |
| VPN 2 $\rightarrow$ PF 8 |
| VPN 3 $\rightarrow$ Disk |
| Empty |

Each entry maps VPN to PFN.

**Step 2: Access** — An arrow labelled "Step 2: Access" goes from the same page table into **Physical Memory**, whose contents are:

| Physical Memory |
| --- |
| Frame 2 |
| Frame 5 |
| Frame 8 |
| Not in RAM $\rightarrow$ **Page Fault!** |

The first three entries resolve to Frame 2, Frame 5 and Frame 8 respectively; the VPN 3 $\rightarrow$ Disk entry is "Not in RAM" and raises a **Page Fault!**

---

<!-- source pages 35-49 -->

## The Memory Management Unit (MMU)

### What is the MMU?

**Memory Management Unit**: The **MMU** is a hardware component (part of the CPU) that translates virtual addresses to physical addresses on every memory access.

- Performs address translation **automatically in hardware**
- Contains the **TLB** (Translation Lookaside Buffer) for caching
- Enforces **memory protection** (R/W/X permissions)
- Triggers **page faults** when pages are not present

**Key Insight:** The OS sets up page tables, but the MMU does the actual translation on every memory access!

### CPU Cache Levels

**CPU Cache**: Small, fast memory built into the CPU that stores recently accessed data to avoid slow RAM access. (chunks that are smaller than pages usually)

- **L1**: Per-core, fastest ($\sim$1ns), smallest (KB)
- **L2**: Per-core, fast ($\sim$3–10ns), larger (MB)
- **L3**: Shared across cores ($\sim$10–20ns), largest (MB)

*Figure: Cache hierarchy — Core 0 and Core 1 each have their own per-core L1 ($\sim$1ns) and L2 ($\sim$3–10ns); both feed into a single shared L3 Cache ($\sim$10–20ns), which sits above RAM ($\sim$50–100ns).*

### MMU in the Memory Hierarchy

*Figure: Data path — CPU Core sends a Virtual Address (VA) to "MMU + TLB", which emits a Physical Address (PA) into L1/L2/L3; on a cache Miss the request goes to RAM. A dashed arrow labelled "TLB miss: walk page table" leaves the bottom of the MMU + TLB box, runs across, and points back up into the L1/L2/L3 box — the walk is re-issued through the cache hierarchy. A separate double-headed dashed arrow links RAM with the Page Table (stored in RAM) below it.*

**Key Insight:** The MMU translates every VA to PA before it reaches the cache. On TLB miss, the page table walk **also goes through the cache**—so cached page table entries speed up translation!

### TLB and Context Switches

**The Problem:** When the OS switches from Process A to Process B, the TLB contains A's translations - these are **invalid** for B!

**TLB Flush**

- Clear entire TLB on switch
- Simple but expensive
- Many TLB misses after switch

**ASID/PCID Tags**

- Tag entries with process ID
- No flush needed
- TLB can hold multiple processes

---

## Paging

### What Are Pages?

**Paging**: Paging divides memory into fixed-size **pages** (virtual memory) and **frames** (physical memory), eliminating external fragmentation.

- Allows **non-contiguous allocation**
- **Page Table** maps virtual pages to physical frames
- Enables **sharing of common memory pages** (e.g., shared libraries)

**Key Insight:** Paging is the foundation of modern virtual memory systems!

### Page Table Structure & PTBR

**Page Table**: An array in RAM, indexed by VPN. Each entry (PTE) holds the PFN and metadata.

- **PTBR**: register pointing to table base in RAM
- **VPN** = index into the array
- **OS** creates the table
- **MMU** reads it

*Figure: The PTBR (a CPU register) points at the base of the Page Table (in RAM); the VPN indexes into its rows. Entry 2 is annotated "not in RAM".*

| Index | PFN | V | RWX | D |
|---|---|---|---|---|
| 0 | 2 | 1 | RW- | 0 |
| 1 | 5 | 1 | R-- | 1 |
| 2 | – | 0 | — | 0 |
| ... | | | | |

### What is in a Page Table Entry (PTE)?

**Page Table Entry**: A PTE stores the physical frame number and metadata for a virtual page. The MMU reads these fields on every address translation.

| Field | Purpose |
|---|---|
| Page Frame Number (PFN) | Physical frame where the page is stored |
| Valid Bit | 1 if the mapping exists (OS allocated it); 0 $\rightarrow$ page fault |
| Present Bit | 1 if page is currently in RAM; 0 $\rightarrow$ page fault (may be on disk) |
| RWX Bits | Access permissions: read, write, execute |
| Dirty Bit | 1 if page was modified in memory; must be written back before eviction |
| Accessed Bit | Set by MMU on access; helps OS decide which pages to evict |
| User/Supervisor Bit | Controls user-mode vs. kernel-mode access |

### Page Sizes and Virtual Address Lookup (64-bit)

**Address Splitting**: Virtual addresses are split into VPN (Virtual Page Number) and Offset.

- **Larger pages** $\rightarrow$ Smaller page tables, but more internal fragmentation
- **Smaller pages** $\rightarrow$ More entries, increasing TLB pressure

**4KB Pages**: VPN = 52 bits, Offset = 12 bits

**2MB Pages**: VPN = 43 bits, Offset = 21 bits

*Figure: A 64-bit Virtual Address splits into VPN (52 bits) and Offset (12 bits).*

### Page Table as an Array

**Array-Based Page Table**: Each process has an array-based page table mapping VPN to PFN. Direct indexing makes lookups fast.

*Figure: A VPN box feeds into a table listing "VPN 0 → PFN 12", "VPN 1 → PFN 34", "VPN 2 → PFN 56", "...", which outputs a PFN.*

**Scalability:** Table size is too large in 64-bit systems! This leads to multi-level page tables.

### Multi-Level Page Tables

**Why Multi-Level?** Single-level tables too large for 64-bit. Multi-level breaks them into chunks—only **needed parts** are stored in memory.

*Figure: The Virtual Address splits into L1 Idx, L2 Idx, L3 Idx and Offset. PTBR points to the L1 Table (one per process); the L1 entry selected by L1 Idx is a ptr to the L2 Table, whose selected entry is a ptr to the L3 Table, whose entry gives the PFN; PFN + Offset = PA.*

### Page Table Walk Example (2-Level)

**Given:** Virtual Address: `0x00403004`, Page Size: 4KB, 32-bit system, 10-10-12 split

**Step 1: Split the Address**

`0x00403004` in binary (32 bits):

| Field | Bits | Value |
|---|---|---|
| L1 Index (10 bits) | `0000000001` | = 1 |
| L2 Index (10 bits) | `0000000011` | = 3 |
| Offset (12 bits) | `000000000100` | = `0x004` |

**Step 2: Walk the Page Tables**

1. PTBR points to L1 table at physical address `0x1000`
2. `L1[1] = 0x5000` (points to L2 table)
3. `L2[3] = 0x8000` (physical frame number)
4. Physical Address = `0x8000` + `0x004` = `0x8004`

*Figure: PTBR (`0x1000`) points into the L1 Table where entry `[1] 0x5000` is highlighted; a "ptr" arrow leads to the L2 Table where entry `[3] 0x8000` is highlighted; a "PFN + Offset" arrow leads to `0x8004`, the Physical Addr.*

---

## Page Swapping

---

<!-- source pages 50-67 -->

## What is Swap Space?

**Swap space** is a dedicated area on disk (partition or file) used to store pages that have been evicted from RAM. It extends virtual memory beyond physical RAM limits.

- Located on disk (much slower than RAM)
- Used for **anonymous pages** (heap, stack) when evicted
- File-backed pages don't need swap - they reload from original file
- Size typically 1-2x physical RAM (varies by system)

**Performance:** Excessive swapping indicates memory pressure. If swap is constantly used, consider adding more RAM!

---

## Demand Paging

**Demand Paging:** Pages are loaded into memory **only when accessed**, not when the program starts. This is also called **lazy loading**.

- Process starts with **no pages in memory**
- First access to a page triggers a **page fault**
- OS loads the page from disk and resumes execution
- Enables running programs **larger than physical memory**

**Benefit:** Programs often don't use all their code/data. Why load pages that may never be accessed?

---

## Swapping Pages In and Out of Memory

**Page Swapping:** When RAM is full, inactive pages are swapped to disk. Page faults trigger swapping pages back into RAM.

- OS selects a **victim page** using an algorithm (LRU, FIFO, Clock)
- **Dirty pages** must be written back to disk before eviction

*Figure: A RAM frame list holding Page A, Page B, Page C (Victim) and Page D, with "New Page E" arriving at the top. A "Swap Out" arrow carries Page C to disk, where it appears as "Page C (Stored)"; a "Swap In" arrow points from disk back into RAM.*

---

## FIFO Page Replacement

**First-In-First-Out:** The oldest page in memory is evicted first, regardless of how recently it was used.

- Simple to implement (just a queue)
- Does not consider page usage
- Can evict frequently-used pages

**Example: 3 frames.** Reference: A, B, C, D

*Figure: "Before" shows frames A, B, C with A labelled oldest and C labelled newest. Request page D (not in memory) arrives. "After" shows A greyed out and evicted, leaving B, C, D, with B now oldest and D newest.*

---

## Belady's Anomaly

**Belady's Anomaly:** With FIFO, **more frames can lead to more page faults**! This counterintuitive behavior doesn't occur with LRU or Clock.

**The Counterintuitive Result** — Reference string: **1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5**

### A Runtime Example

**3 Frames**

| Ref | F1 | F2 | F3 | Fault? |
|---|---|---|---|---|
| 1 | 1 | - | - | ✓ |
| 2 | 1 | 2 | - | ✓ |
| 3 | 1 | 2 | 3 | ✓ |
| 4 | 4 | 2 | 3 | ✓ |
| 1 | 4 | 1 | 3 | ✓ |
| 2 | 4 | 1 | 2 | ✓ |
| 5 | 5 | 1 | 2 | ✓ |
| 1 | 5 | 1 | 2 | ✗ |
| 2 | 5 | 1 | 2 | ✗ |
| 3 | 5 | 3 | 2 | ✓ |
| 4 | 5 | 3 | 4 | ✓ |
| 5 | 5 | 3 | 4 | ✗ |

**9 Page Faults**

**4 Frames**

| Ref | F1 | F2 | F3 | F4 | Fault? |
|---|---|---|---|---|---|
| 1 | 1 | - | - | - | ✓ |
| 2 | 1 | 2 | - | - | ✓ |
| 3 | 1 | 2 | 3 | - | ✓ |
| 4 | 1 | 2 | 3 | 4 | ✓ |
| 1 | 1 | 2 | 3 | 4 | ✗ |
| 2 | 1 | 2 | 3 | 4 | ✗ |
| 5 | 5 | 2 | 3 | 4 | ✓ |
| 1 | 5 | 1 | 3 | 4 | ✓ |
| 2 | 5 | 1 | 2 | 4 | ✓ |
| 3 | 5 | 1 | 2 | 3 | ✓ |
| 4 | 4 | 1 | 2 | 3 | ✓ |
| 5 | 4 | 5 | 2 | 3 | ✓ |

**10 Page Faults!**

---

## LRU Page Replacement (Linked List Implementation)

**LRU Algorithm:** LRU evicts the least recently used page using a doubly linked list.

- **Front** $\rightarrow$ Most recently used page
- **Tail** $\rightarrow$ Least recently used page (evicted on fault)
- Accessed pages are moved to the front

*Figure: A left-to-right chain Page 3 (MRU) → Page 5 → Page 8 → Page 2 (LRU); an arrow drops from the tail element down to a box labelled "Page 2 Evicted".*

---

## Clock Algorithm (Approximate LRU)

**Clock Algorithm** approximates LRU with lower overhead using a circular queue with reference bits.

1. Check the page at the clock hand
2. If **reference bit = 1**, set it to 0 and move the hand
3. If **reference bit = 0**, replace the page

**Why Clock over LRU?**

- LRU requires updating a linked list on **every memory access** - too expensive!
- Clock only updates on **page faults** - much lower overhead
- Clock approximates LRU behavior with near-constant time operations

### Clock Algorithm Example

*Figure: Eight pages A–H arranged clockwise around a circle, each labelled with its reference bit in parentheses; a "Clock" hand in the centre points at the current page.*

**Key Insight:** Numbers in parentheses are reference bits. Clock hand moves until it finds a 0!

**Part 1 — initial state.** Ring contents, clockwise from the top: Page A (1), Page B (0), Page C (1), Page D (0), Page E (1), Page F (1), Page G (1), Page H (0). The clock hand points at **Page A**.

**Part 2 — Step 1.** Page A had reference bit = 1, so we set it to 0 and move to Page B. Ring: Page A (0), Page B (0), Page C (1), Page D (0), Page E (1), Page F (1), Page G (1), Page H (0). Hand points at **Page B**.

**Part 3 — Step 2.** Page B had reference bit = 0, so it was **evicted**! Hand moves to Page C. Ring: Page A (0), Page C (1), Page D (0), Page E (1), Page F (1), Page G (1), Page H (0) — Page B is gone. Hand points at **Page C**.

---

## Working Set Model

**Working Set:** The **working set** of a process is the set of pages it has referenced in the last $\Delta$ time units (the working set window).

- Based on **locality of reference**: programs tend to access the same pages repeatedly
- If working set fits in available frames $\rightarrow$ few page faults
- If working set exceeds available frames $\rightarrow$ thrashing

---

## Thrashing

**Thrashing** occurs when the system spends more time handling page faults than executing useful work.

**Symptoms:**

- High page fault rate
- Low CPU utilization (waiting on disk I/O)
- Disk constantly busy
- System becomes unresponsive

---

## Effective Access Time (EAT)

The **EAT** is the average time required to access memory, accounting for fast and slow cases:

- TLB hits vs. TLB misses (requiring page table lookup)
- Page faults (requiring disk access)

### General Formula

$$\text{EAT} = (1 - p) \times \left(\alpha\, T_{\text{hit}} + (1 - \alpha)\, T_{\text{miss}}\right) + p \times T_{\text{fault}}$$

**Variables**

| Symbol | Meaning |
|---|---|
| $\alpha$ | TLB hit ratio |
| $p$ | Page fault rate |
| $T_{\text{hit}}$ | Access time on TLB hit |
| $T_{\text{miss}}$ | Access time on TLB miss |
| $T_{\text{fault}}$ | Service time on page fault |

### Solution Part 1: TLB Hits and Misses

Calculating access times:

$$T_{\text{hit}} = T_{\text{TLB}} + T_{\text{mem}} = 20 + 100 = 120 \text{ ns}$$
$$T_{\text{miss}} = T_{\text{TLB}} + 2 \times T_{\text{mem}} = 20 + 2 \times 100 = 220 \text{ ns}$$
$$\text{EAT}_{\neg\text{fault}} = 0.9 \times 120 + 0.1 \times 220 = 130 \text{ ns}$$

### Solution Part 2: Incorporating Page Faults

**Given:** Using $p = 0.001$ and $T_{\text{fault}} = 10 \text{ ms} = 10 \times 10^6 \text{ ns}$

**Final Calculation:**

$$\text{EAT} = (1 - p) \times 130 + p \times 10 \times 10^6$$
$$= 0.999 \times 130 + 0.001 \times 10{,}000{,}000$$
$$\approx 10{,}129.87 \text{ ns} \approx 10.13\ \mu\text{s}$$
$$\text{Total time} = 10{,}000 \times 10{,}129.87 \text{ ns}$$
$$= 1.012987 \times 10^{8} \text{ ns} \approx 0.1013 \text{ s}$$

### Impact of Page Fault Rate

**Small Changes, Big Impact:** Even tiny increases in page fault rate dramatically increase effective access time.

| Page Fault Rate (p) | EAT | Slowdown |
|---|---|---|
| 0 (no faults) | 130 ns | 1x |
| 0.0001 (1 in 10,000) | 1,130 ns | 9x |
| 0.001 (1 in 1,000) | 10,130 ns | 78x |
| 0.01 (1 in 100) | 100,130 ns | 770x |

---

<!-- source pages 68-90 -->

## Exercises

### Exercise 1: Page Table Size Calculation

Given a system with:

- 32-bit virtual address space
- 4KB page size
- Each PTE is 4 bytes

Calculate:

1. How many pages in the virtual address space?
2. Size of a single-level page table?
3. Why is this problematic?

#### Solution

- Page size $= 4\text{KB} = 2^{12}$ bytes $\rightarrow$ Offset $= 12$ bits
- VPN $= 32 - 12 = 20$ bits
- Number of pages $= 2^{20} = 1{,}048{,}576$ pages
- Page Table Size $= 2^{20} \times 4$ bytes $=$ **4MB per process**

**Problem:** Every process needs 4MB just for its page table! With 100 processes, that's 400MB of page tables alone. This is why we use **multi-level page tables**.

---

### Exercise 2: VPN and Offset Calculation

Given:

- 32-bit virtual address: `0x00005A4B`
- Page size: 4KB

Calculate the VPN and Offset.

#### Solution

- Page size $= 4\text{KB} = 2^{12}$ bytes $\rightarrow$ Offset $= 12$ bits
- `0x00005A4B` = 0000 0000 0000 0000 0101 1010 0100 1011
- Offset = lower 12 bits = `0xA4B` = 2635
- VPN = upper 20 bits = `0x00005` = 5

---

### Exercise 3: Multi-Level Address Splitting

Given:

- 32-bit virtual address: `0x05307100`
- Page size: 4KB
- 3-level page table: L1 has 256 entries, L2 has 64 entries, L3 has 64 entries

Calculate the number of index bits per level, and split the address into L1, L2, L3 indices and page offset.

#### Solution

- Index bits: L1 $= \log_2(256) = 8$, L2 $= \log_2(64) = 6$, L3 $= \log_2(64) = 6$
- Offset $= \log_2(4\text{KB}) = 12$ bits. Verify: $8 + 6 + 6 + 12 = 32$ ✓

`0x05307100` = 0000 0101 0011 0000 0111 0001 0000 0000

| Field | L1 (8 bits) | L2 (6 bits) | L3 (6 bits) | Offset (12 bits) |
|---|---|---|---|---|
| Bits | `00000101` | `001100` | `000111` | `000100000000` |
| Value | = 5 | = 12 | = 7 | = `0x100` |

---

### Exercise 4: 4D Array Memory Access Patterns

Consider a 4D array declared as: `int A[8][8][8][8];`

**System Parameters**

- `sizeof(int)` = 4 bytes
- Page size = 64 bytes (small for illustration)
- Array stored in **row-major order** (rightmost index varies fastest)
- Array starts at page boundary
- Process has **4 page frames** available
- LRU page replacement

#### Access Patterns

Pattern A: Sequential

```c
// Rightmost varies fastest
for(i=0; i<8; i++)
 for(j=0; j<8; j++)
  for(k=0; k<8; k++)
   for(l=0; l<8; l++)
    sum += A[i][j][k][l];
```

Pattern B: Strided

```c
// Leftmost varies fastest
for(l=0; l<8; l++)
 for(k=0; k<8; k++)
  for(j=0; j<8; j++)
   for(i=0; i<8; i++)
    sum += A[i][j][k][l];
```

**Think About It:** Which pattern matches how the array is stored in memory? How does this affect page faults?

#### Solution — Memory Layout Analysis

- Total array size: $8 \times 8 \times 8 \times 8 \times 4 = 16{,}384$ bytes
- Elements per page: $64/4 = 16$ integers
- Total pages needed: $16{,}384/64 = 256$ pages
- Row-major: `A[i][j][k][l]` and `A[i][j][k][l+1]` are adjacent

| | Result | Why |
|---|---|---|
| **Pattern A** | $\sim 256$ faults | Sequential access: each page loaded once, 16 elements accessed before next page. |
| **Pattern B** | $\sim 4096$ faults | Strided access: jumps 512 bytes between accesses. Every access causes a fault! |

---

### Exercise 5: LRU Page Replacement Trace

Given page reference string: **7, 0, 1, 2, 0, 3, 0, 4, 2, 3**
With **3 frames**, trace LRU and count page faults.

#### Solution — LRU Trace

| Ref | Frame 1 | Frame 2 | Frame 3 | Fault? |
|---|---|---|---|---|
| 7 | 7 | - | - | Yes |
| 0 | 7 | 0 | - | Yes |
| 1 | 7 | 0 | 1 | Yes |
| 2 | 2 | 0 | 1 | Yes (evict 7) |
| 0 | 2 | 0 | 1 | No |
| 3 | 2 | 0 | 3 | Yes (evict 1) |
| 0 | 2 | 0 | 3 | No |
| 4 | 4 | 0 | 3 | Yes (evict 2) |
| 2 | 4 | 0 | 2 | Yes (evict 3) |
| 3 | 4 | 3 | 2 | Yes (evict 0) |

**Total page faults: 8**

---

### Exercise 6: FIFO Page Replacement Trace

Same reference string: **7, 0, 1, 2, 0, 3, 0, 4, 2, 3**
With **3 frames**, trace FIFO and count page faults.

#### Solution — FIFO Trace

| Ref | Frame 1 | Frame 2 | Frame 3 | Fault? |
|---|---|---|---|---|
| 7 | 7 | - | - | Yes |
| 0 | 7 | 0 | - | Yes |
| 1 | 7 | 0 | 1 | Yes |
| 2 | 2 | 0 | 1 | Yes (evict 7) |
| 0 | 2 | 0 | 1 | No |
| 3 | 2 | 3 | 1 | Yes (evict 0) |
| 0 | 2 | 3 | 0 | Yes (evict 1) |
| 4 | 4 | 3 | 0 | Yes (evict 2) |
| 2 | 4 | 2 | 0 | Yes (evict 3) |
| 3 | 4 | 2 | 3 | Yes (evict 0) |

**Total page faults: 9** (vs 8 for LRU!)

---

### Exercise 7: Clock Algorithm Trace

Reference string: **1, 2, 3, 4, 1, 2, 5, 1, 2, 3**
With **3 frames**, trace Clock algorithm. Show reference bits.

#### Solution — Clock Trace (abbreviated)

- **1, 2, 3:** Load pages, all ref bits = 1
- **4:** Hand at 1 (ref=1), clear to 0, move; at 2 (ref=1), clear to 0, move; at 3 (ref=1), clear to 0, move; back at 1 (ref=0), **evict 1**, load 4
- **1:** Hand at 2 (ref=0), **evict 2**, load 1
- **2:** Hand at 3 (ref=0), **evict 3**, load 2
- **5:** Hand at 4 (ref=1), clear, move; at 1 (ref=1), clear, move; at 2 (ref=1), clear, move; back at 4 (ref=0), **evict 4**, load 5
- **1:** Already in frame $\rightarrow$ **hit**, set ref bit = 1
- **2:** Already in frame $\rightarrow$ **hit**, set ref bit = 1
- **3:** Hand at 1 (ref=1), clear, move; at 2 (ref=1), clear, move; at 5 (ref=1), clear, move; back at 1 (ref=0), **evict 1**, load 3

**Total page faults: 8**

---

### Exercise 8: Page Table Walk with TLB

Reference string (VPNs): **1, 2, 3, 1, 4, 1, 5, 1**

| Structure | Configuration |
|---|---|
| **TLB** | 2 entries, **LRU** replacement |
| **Physical Frames** | 4 frames, **LRU** replacement |
| **Page Table** | 2-level (L1 PT pinned in RAM, L2 PT is a regular page) |

All structures start **empty** (including L2 PT—not yet loaded).

For each access:

1. TLB hit or miss? (miss $\rightarrow$ page table walk)
2. During walk: is the L2 PT page in a frame? If not $\rightarrow$ **page fault (PT)**
3. Is the data page in a frame? If not $\rightarrow$ **page fault (data)**

#### Solution

**Important:** L2 PT is a **regular page** that competes with data pages for frames. A single access can cause up to 2 page faults (L2 PT + data).

| Ref | TLB | PT Fault? | Data Fault? | TLB State | Frames (MRU $\rightarrow$ LRU) |
|---|---|---|---|---|---|
| 1 | Miss | Yes (load L2PT) | Yes (load Pg1) | {1} | [Pg1, L2PT] |
| 2 | Miss | No (L2PT in F0) | Yes (load Pg2) | {1,2} | [Pg2, L2PT, Pg1] |
| 3 | Miss | No | Yes (load Pg3) | {2,3} | [Pg3, L2PT, Pg2, Pg1] |
| 1 | Miss | No | **No** (Pg1 in frame) | {3,1} | [Pg1, L2PT, Pg3, Pg2] |
| 4 | Miss | No | Yes (evict Pg2) | {1,4} | [Pg4, L2PT, Pg1, Pg3] |
| 1 | Hit | — | **No** | {4,1} | [Pg1, Pg4, L2PT, Pg3] |
| 5 | Miss | No | Yes (evict Pg3) | {1,5} | [Pg5, L2PT, Pg1, Pg4] |
| 1 | Hit | — | **No** | {5,1} | [Pg1, Pg5, L2PT, Pg4] |

- **Access 1:** Cold start—L2 PT itself is not in memory! Double page fault.
- **Access 4 (VPN 1):** TLB miss (VPN 1 was evicted from TLB at access 3), but Pg1 is still in a frame $\rightarrow$ **no page fault**. This shows **TLB miss $\neq$ page fault**.

**Totals**

- Total page faults: **6** — 1 PT fault + 5 data faults
- TLB misses: **6**
- TLB hits: **2** (accesses 6, 8)

**Key Insight**

- L2 PT stays resident—every TLB miss walks the PT, refreshing L2PT's LRU position.
- Accesses 6 & 8: **best case**—TLB hit, no walk, no fault.

---

## Summary

### Memory Hierarchy Pyramid

*Figure: A pyramid of storage levels with access times on the left — Registers (0.3 ns) at the apex, then L1 Cache (1 ns), L2 Cache Memory (3–10 ns), L3 Cache Memory (10–20 ns), Main Memory (50–100 ns), and Disk (20–40 µs) at the base. An "MMU" label sits at the top right, with an arrow cascading down the right side of the pyramid: "Evict" beside the cache/main-memory levels and "Page Fault" at the Disk level.*

---

## See You Next Week!

Questions?
