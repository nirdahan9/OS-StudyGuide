# Recitation 6 — File Systems, Pipes and RAID

> Study notes converted from the recitation slide deck `Rec6_OS.pdf` (113 pages) — Operating Systems, Reichman University, Spring Semester 2026.
> Lecturer: Tsvi Cherny-Shahar · TAs: Daniel Karalnik, Liam Tal.
> Faithful conversion: every substantive item on the source pages appears here, and nothing has been added from outside them. Figures are rendered as italic *Figure:* descriptions.
> `<!-- source pages a-b -->` comments mark the original page ranges.

---

<!-- source pages 1-14 -->

## Recitation 6 — File Systems

### Table of Contents

1. File System Hardware
2. File System Abstraction Layer
3. Linking
4. File Descriptors
5. Pipes
6. RAID

---

## File System Hardware

### What Does a Hard Disk Do?

> **Hard Disk Drive (HDD)**
> A **non-volatile** storage device that stores and retrieves digital data using **magnetic storage** on rapidly spinning platters.

- Data **persists** even without power (unlike RAM)
- Read and written using a **mechanical arm with a read/write head**
- HDDs are used in:
  - Operating systems and application storage
  - File storage (documents, images, videos)
  - Databases and large-scale data management

### What is an SSD?

> **Solid-State Drive (SSD)**
> A storage device that uses **flash memory** instead of mechanical parts. Much faster than HDDs due to the lack of moving components.

- Uses NAND flash memory for data storage
- Found in laptops, desktops, servers, and high-performance storage systems

### SSD Flash Memory Organization

> **Flash Memory Organization**
> Data is stored in flash memory cells, grouped into hierarchical units.

- **Pages** – Smallest writable unit (typically 4KB)
- **Blocks** – Group of pages (e.g., 128 pages per block)

> **Warning**
> Data **cannot be overwritten directly** – a block must be erased before rewriting.

### Limited Write Ability in SSDs

> **Write Wear**
> Flash memory cells **wear out** after a certain number of write cycles.

- **Wear Leveling Algorithms** help distribute writes evenly to extend lifespan
- SSD controllers use **garbage collection** to manage writes efficiently

---

## File System Abstraction Layer

### File System as an Abstraction Layer

> **File System**
> A **software layer** that abstracts raw hardware into structured storage, organizing and managing **files** and directories.

- Provides an interface for users and applications to access data
- Hides the complexities of hardware (HDD, SSD, etc.)

**Why is it Needed?**

- Without a file system, data would be stored as **raw bytes**
- Enables features like **naming, permissions, and metadata**
- Supports different storage media (HDD, SSD, etc) with a **common interface**

### What is a File?

> **File**
> A **named collection of data** stored on a disk, representing **logical storage** rather than physical locations.

- Files can contain:
  - **Text** (documents, source code)
  - **Binary Data** (executables, images, videos)
  - **Metadata** (permissions, timestamps, size)
- The **file system keeps track** of file locations and access permissions

**Example File**

- **Name:** `report.pdf`
- **Path:** `/home/user/documents/`
- **Size:** 1.5MB
- **Permissions:** `rw-r--r--`

### File System Abstraction

*Figure: A top-to-bottom stack of five boxes connected by downward arrows, each arrow labelled with what crosses that boundary. Applications → (`open(), read(), write()`) → System Calls (open, read, write, etc.) → (File System Interface) → File System (NTFS, ext4, FAT32, etc.) → (File Operations) → Files and Directories → (Raw Data Blocks) → Storage Device (HDD, SSD, Flash).*

### Contiguous File Allocation

> **Contiguous Allocation**
> A file is stored in **consecutive disk blocks**. Requires pre-allocating space; the file system tracks **starting block and file length**.

**Example:**

*Figure: A "Disk:" row of ten empty block cells; four adjacent cells in the middle are shaded and bracketed under the label "File A (Contiguous Blocks)", showing the file occupying one unbroken run of blocks.*

### Fragmentation Problem in Contiguous Allocation

> **External Fragmentation**
> When files are deleted, small **free gaps** appear between allocated blocks. New files may not fit if they require **larger** contiguous space, leading to **wasted storage**.

**Example:**

*Figure: A "Disk:" row of ten block cells. File A occupies two shaded blocks at the left, File B two shaded blocks in the middle, File C two shaded blocks toward the right, with empty cells scattered between them; an arrow points at the gap between File A and File B labelled "Free Blocks (Fragmented)".*

### Pros and Cons of Contiguous Allocation

| Advantages | Disadvantages |
| --- | --- |
| **Fast access** – Sequential reading is efficient | **External fragmentation** – Small free spaces may become unusable |
| **Simple management** – Only starting block and length needed | **Difficult resizing** – Expanding a file is hard if next blocks are occupied |
| | **Pre-allocation issue** – Requires knowing file size in advance |

---

<!-- source pages 15-33 -->

## Linked List File Allocation

**Linked List Allocation**: A file is stored as **scattered blocks** on disk. Each block contains **data** and a **pointer** to the next block. The file system tracks the **first block (head)**.

Example:

*Figure: A row of disk blocks with three shaded (blue) blocks scattered among free ones; curved arrows lead from the first shaded block to the second and from the second to the third. The first is labelled "Start", the last "End" — showing that the blocks of one file may sit anywhere on disk and are chained by pointers.*

### Pros and Cons of Linked List Allocation

| Advantages | Disadvantages |
| --- | --- |
| **No External Fragmentation** – Blocks can be anywhere | **Slow Random Access** – Must traverse the list sequentially with the risk of multiple disk reads |
| **Efficient Use of Space** – No need for pre-allocation | **Pointer Overhead** – Each block stores a pointer, reducing usable space |
| **Easy File Growth** – Just add new blocks | |
| **Simple** – File at starting index | |

### Exercise: I/O in Linked List Allocation

Parameters:

- Block size: 1 KiB (1024 bytes)
- Pointer size: 4 bytes (stored at the end of each block)
- Usable data per block: 1020 bytes
- File `data.bin` occupies 10 blocks in linked list allocation
- **Assume** all file system metadata (directory entries, etc.) is **cached in RAM** – only data block reads count as disk I/O

Questions:

1. How many disk I/O operations to read the **first byte** of the file?
2. How many disk I/O operations to read the byte at **offset 5000**?
3. How many disk I/O operations to read the **last byte** of the file?
4. If **contiguous allocation** were used instead, how many I/O operations would question 2 require?

#### Solution (Q1–Q2)

**Q1:** The starting block number is already in RAM (directory entry). Read that one block from disk.

$$\text{I/O operations} = \mathbf{1}$$

**Q2:** Block index $= \lfloor 5000 \div 1020 \rfloor = 4$ (zero-indexed). Must traverse blocks $0 \to 1 \to 2 \to 3 \to 4$:

$$\text{I/O operations} = \mathbf{5}$$

#### Solution (Q3–Q4)

**Q3:** Total data $= 10 \times 1020 = 10{,}200$ bytes. Last byte at offset $10{,}199 \to$ block 9. Must traverse all 10 blocks:

$$\text{I/O operations} = \mathbf{10}$$

**Q4:** Starting block is in RAM. Block index $= \lfloor 5000 \div 1024 \rfloor = 4$. Jump directly to `start_block + 4`:

$$\text{I/O operations} = \mathbf{1} \quad \text{(no traversal needed)}$$

---

## File Allocation Table (FAT)

**FAT Allocation**: A table-based version of **linked list allocation**. The FAT stores file block links, enabling **random access**.

- Each file has a **starting block**, and FAT entries point to the next block
- FAT table tracks file block links
- Allows **random access** compared to standard linked list allocation

### FAT Directory Table

**Directory Entry**: In FAT, each directory contains a table of **32-byte entries** – one per file or subdirectory. The directory entry is the **starting point** for locating a file's data on disk.

What a Directory Entry Stores:

- **Filename** (8 bytes) + **Extension** (3 bytes)
- **Attributes** – read-only, hidden, system, directory, archive
- **Timestamps** – creation, last access, last modification
- **Starting cluster number** – entry point into the FAT chain
- **File size** (4 bytes)

### FAT File Access Flow

Complete Access Path — to read a file (e.g., `/docs/report.txt`):

1. **Root directory** – look up the entry for `docs` $\to$ get its starting cluster
2. **FAT chain for `docs`** – read directory data blocks to find `report.txt` entry
3. **Directory entry** – extract the **starting cluster** of `report.txt`
4. **FAT chain** – follow cluster $\to$ next cluster links until EOF
5. **Data blocks** – read each cluster's data from disk

### A Note about Directories

**Directories Are Files**: In FAT, a directory is just a file whose data blocks contain directory entries. The root directory is special – in FAT12/FAT16 it has a fixed location and size; in FAT32 it is a regular cluster chain.

### FAT Example

*Figure: A "Directory Table" (Filename / Start Block) with rows `File A → 0` (blue) and `File B → 3` (green); arrows point from each start block into the FAT table (columns "Block" and "Next"). A legend maps blue = File A, green = File B, white = Free.*

Directory Table:

| Filename | Start Block |
| --- | --- |
| File A | 0 |
| File B | 3 |

FAT table (block colours from the legend given in brackets):

| Block | Next |
| --- | --- |
| 0 (File A) | 2 |
| 1 (Free) | - |
| 2 (File A) | 5 |
| 3 (File B) | 4 |
| 4 (File B) | EOF |
| 5 (File A) | EOF |

### Pros and Cons of FAT Allocation

| Advantages | Disadvantages |
| --- | --- |
| **No External Fragmentation** – Blocks can be anywhere | **Table Overhead** – FAT must be stored in memory |
| **Better than Linked List** – FAT allows faster random access | |

### Size of the File Allocation Table (FAT)

**FAT Table Size**: The FAT table **grows** as disk size increases. Each entry corresponds to a block. Size depends on **number of blocks** and **entry size** (FAT12, FAT16, FAT32).

Example Calculation — a 1TB disk with **4KB block size**:

- Total blocks $= \frac{1\text{TB}}{4\text{KB}} = 2^{28}$ (268 million blocks)
- **FAT32 uses 4 bytes per entry**, so FAT size $= 2^{28} \times 4\text{B} \approx 1\text{GB}$

### Exercise: I/O in FAT Allocation

Parameters:

- Block size: 1 KiB (1024 bytes)
- FAT entry size: 4 bytes
- File `data.bin` occupies 10 blocks in FAT allocation
- Each data block stores **1024 bytes** of usable data (no pointer overhead – pointers are in the FAT)
- The directory entry (in RAM) provides the starting block number

Part A – FAT Cached in RAM:

1. How many disk I/O operations to read the byte at **offset 5000**?
2. How many disk I/O operations to read the **last byte** of the file?

Part B – FAT Stored on Disk. Assume each FAT entry lookup requires **1 additional disk I/O**.

3. How many disk I/O operations to read the byte at **offset 5000**?
4. How many disk I/O operations to read the **last byte** of the file?

#### Solution: Part A (FAT in RAM)

**Q1:** Block index $= \lfloor 5000 \div 1024 \rfloor = 4$ (zero-indexed). Follow the FAT chain **in memory** (0 disk I/O) to find the physical block number, then read it:

$$\text{I/O operations} = \mathbf{1}$$

**Q2:** Total data $= 10 \times 1024 = 10{,}240$ bytes. Last byte at offset $10{,}239 \to$ block 9. FAT chain traversal is entirely in RAM:

$$\text{I/O operations} = \mathbf{1}$$

Key Insight: When the FAT is cached in RAM, **any** random access costs just **1 disk I/O** – a huge improvement over linked list allocation.

#### Solution: Part B (FAT on Disk)

**Q3:** Block index $= 4$. Must read FAT entries for blocks $0 \to 1 \to 2 \to 3 \to 4$ from disk (5 FAT reads), then read the data block:

$$\text{I/O operations} = 5 + 1 = \mathbf{6}$$

**Q4:** Block 9. Must read FAT entries for blocks $0 \to 1 \to \cdots \to 9$ from disk (10 FAT reads), then read the data block:

$$\text{I/O operations} = 10 + 1 = \mathbf{11}$$

FAT on Disk $\approx$ Linked List: When the FAT cannot fit in RAM, performance degrades back toward linked list levels.

---

## Defragmentation

### Defragmentation in HDDs

**Fragmentation**: Files stored in **non-contiguous** blocks over time force the read/write head to **seek across disk platters**, slowing down access.

**How Defragmentation Works**: Moves fragmented blocks into **sequential order**, reducing seek time and improving **performance**.

Example: Before and After Defragmentation

*Figure: Two rows of disk blocks. "Before" shows red file blocks scattered in small groups with gaps between them; "After" shows the same blocks packed together in one contiguous run.*

### Why Defragmentation is Not Needed for SSDs

Key Differences:

- **HDDs are mechanical** – Moving parts make fragmented access slower
- **SSDs have no moving parts** – Access time is the same for any block

**Harmful for SSDs**: **Defrag writes data unnecessarily**, causing more wear on flash memory cells.

---

<!-- source pages 34-49 -->

## Introduction to NTFS (New Technology File System)

**NTFS** — the **New Technology File System** — the default file system for modern Windows.

- **File metadata stored in MFT (Master File Table)**
- **Journaling** prevents corruption by tracking changes before writing
- **Access Control Lists (ACLs)** manage file permissions

### Master File Table (MFT)

The MFT is a table where each row is a **1 KB record** describing one file or directory on the volume. The first 16 entries are reserved for NTFS **system metadata**.

| MFT # | Name | Purpose |
|---|---|---|
| 0 | `$MFT` | The MFT itself |
| 1 | `$MFTMirr` | Backup of first MFT entries |
| 2 | `$LogFile` | Transaction journal |
| 3 | `$Volume` | Volume metadata |
| 4 | `$AttrDef` | Attribute definitions |
| 5 | `. (root)` | **Root directory** |
| 6–15 | *(reserved)* | Other system metadata |
| 16+ | *User files* | Files and directories |

### Resident vs Non-Resident Files

**Resident data:** if a file's data is **small enough** (typically $< 700$ bytes), NTFS stores it **directly inside the MFT entry** — no external clusters needed. This is called **resident** storage.

**Resident File**

- Data lives **inside** the 1 KB MFT record
- **Single disk read** to get metadata + data
- Ideal for very small files and short directory listings

**Non-Resident File**

- Data is too large for the MFT record
- MFT entry stores **data runs** (start cluster, length) pointing to external clusters
- Requires **additional disk reads** to fetch file contents

### B-Tree Directory Index (`$INDEX_ROOT`)

NTFS directories store their child entries in a **B-tree** structure, enabling $O(\log n)$ lookups. The root of this tree is the `$INDEX_ROOT` attribute inside the directory's MFT entry.

**Small Directory (Resident)**

- All entries fit inside `$INDEX_ROOT`
- B-tree is **entirely resident** in the MFT record
- Single disk read to list the directory

**Large Directory (Non-Resident)**

- `$INDEX_ROOT` holds the B-tree root node
- Additional nodes spill into `$INDEX_ALLOCATION` clusters on disk
- Tree is traversed by following pointers from root to leaf

### MFT Entry Structure — File

*Figure: A 1 KB record labelled "MFT Entry #42" containing Created: 2024-01-15 09:30, File Name: `example.txt`, Size: 8 KB (non-resident), ACL: Read/Write, and a `$DATA` — Data Runs attribute listing `Run 1: start=100, len=3`, `Run 2: start=250, len=2`, `Run 3: start=410, len=1`. Arrows point from the three runs (labelled 3 clusters, 2 clusters, 1 cluster) to Disk Clusters 100–102, 250–251 and 410, which all belong to the NTFS volume — showing how a non-resident file's bytes are scattered across the disk and reassembled through the data runs.*

### MFT Entry Structure — Directory

*Figure: A 1 KB record labelled "MFT Entry #102 (Directory)" containing Created: 2024-03-10 14:20, File Name: `Documents`, ACL: Read/Write/List, a `$INDEX_ROOT` — B-tree root attribute holding `budget.xlsx → MFT #510`, `notes.txt → MFT #511` and child node pointers, and a `$INDEX_ALLOC` — Data Runs attribute with `Run 1: start=300, len=2`. The child node pointers lead to two B-tree child nodes, "A–D entries" and "O–Z entries", stored in Clusters 300–301 — showing a directory index whose root is resident but whose child nodes live on disk.*

#### Directory: Resident and Non-Resident Index

Small directories keep the **entire B-tree inside `$INDEX_ROOT`** (resident). When the directory grows, child nodes spill to disk via `$INDEX_ALLOCATION` data runs — just like non-resident file data.

### NTFS File Access Flow

To read a file (e.g., `C:\Users\Documents\report.docx`):

1. Start at **MFT entry #5** (always the root directory in NTFS)
2. Read the root directory's **B-tree index** → look up `Users` → get its MFT record number
   - If the directory is **small**: B-tree is **resident** in `$INDEX_ROOT` — single MFT read
   - If the directory is **large**: follow `$INDEX_ALLOCATION` data runs to read B-tree child nodes from disk
3. Repeat for each path component (`Users` → `Documents` → `report.docx`)
4. Read the `report.docx` MFT entry:
   - If the file is **resident**: data is stored directly in the MFT record — done
   - If the file is **non-resident**: follow **data runs** to read file contents from disk clusters

*Figure: Flow diagram of the same traversal — MFT #5 `C:\ (root)` (resident index) --look up Users--> MFT #38 `Users` (resident index) --look up Docs--> MFT #102 `Docs` (non-resident index), which branches sideways to B-tree child nodes; then --look up report.docx--> MFT #519 `report.docx`, feeding a decision diamond "Resident?": Yes → "Data inside MFT record"; No → disk clusters 800–803 and 920–921. Legend: MFT Entry, Disk Clusters, Index Nodes, Resident Data.*

---

## Exercise: I/O in NTFS File Access

**Parameters**

- Cluster size: 4 KiB, MFT entry size: 1 KiB
- Path to resolve: `C:\Users\Documents\report.docx`
- **MFT entries are NOT cached** — each MFT entry read = 1 disk I/O
- Root directory (MFT #5): **resident** index
- `Users` directory: **non-resident** index, B-tree depth $= 3$
- `Documents` directory: **non-resident** index, B-tree depth $= 2$
- `report.docx`: **non-resident** file, 3 data runs

**B-tree traversal cost:** the B-tree **root node** is always inside the MFT entry (no extra I/O). Each additional level requires **1 disk I/O** to read the child node. A B-tree of depth $d$ costs $d - 1$ extra I/O operations.

**Questions**

1. How many disk I/O operations to resolve the full path and read the **first byte** of `report.docx`?
2. If `report.docx` were a **resident** file ($< 700$ bytes), how would the answer change?
3. If **all** directories had **resident** indexes, how would Q1 change?

### Solution: NTFS I/O (Q1)

**Q1:** Trace each step of the path resolution:

1. Read MFT #5 (root): **1 I/O** → index is resident, look up `Users` in memory
2. Read `Users` MFT entry: **1 I/O** → B-tree depth $= 3$, root is in entry, 2 child levels on disk: **2 I/O** → find `Documents`
3. Read `Documents` MFT entry: **1 I/O** → B-tree depth $= 2$, root is in entry, 1 child level on disk: **1 I/O** → find `report.docx`
4. Read `report.docx` MFT entry: **1 I/O** → get data runs, calculate target cluster
5. Read data cluster from disk: **1 I/O**

$$\text{Total} = 1 + 1 + 2 + 1 + 1 + 1 + 1 = \mathbf{8}\ \text{I/O}$$

### Solution: NTFS I/O (Q2–Q3)

**Q2 (Resident file):** Same path traversal (7 I/O), but the file data is **inside the MFT record** — no final cluster read:

$$\text{Total} = 8 - 1 = \mathbf{7}\ \text{I/O}$$

**Q3 (All resident indexes):** Remove all B-tree child node reads ($2 + 1 = 3$ I/O saved). Only MFT entry reads + data cluster remain:

$$\text{Total} = 8 - 3 = \mathbf{5}\ \text{I/O}$$

---

## Journaling in NTFS

**Write-ahead logging:** NTFS uses a **write-ahead log** (`$LogFile`) to ensure file system consistency. Every metadata change is recorded in the journal **before** being applied to disk.

**Transaction lifecycle**

1. **Begin transaction** — record the intended operation in `$LogFile`
2. **Write metadata changes** to disk (e.g., update MFT entry, directory index)
3. **Commit transaction** — mark the journal entry as complete

### NTFS Journal Recovery on Restart

*Figure: Flowchart — "System Restart" → "Scan `$LogFile` from last checkpoint" → decision "Transaction committed?"; Yes → "Redo: Replay changes to disk", No → "Undo: Roll back partial changes"; both branches converge on "File system marked clean".*

### Journaling Limitations in NTFS

**Metadata-only journaling:** NTFS journaling supports only **metadata** (MFT entries, directory indexes, bitmaps) — not file data. This balances performance with reliability.

---

<!-- source pages 50-67 -->

## Single Inode System

**Inode**: Each file has an **inode** storing file metadata (size, timestamps, permissions) and **direct pointers** to disk blocks.

- Simple and efficient for **small files**
- Stored on a pre-defined location on disk (can run out of inode space)
- **Limitation**: Random access depends on the number of direct pointers, can make locating a large file very slow

### Single Inode System - An Example

Example: A Single Inode System

*Figure: A single inode drawn as a vertical box with two labelled rows — "Metadata" on top and "Direct Pointers → Data" below it. Red arrows fan out from the direct-pointer row to three adjacent green data blocks (labelled "File Data") sitting inside the inode's lower region, showing that every data block is reached by one direct pointer.*

---

## Multi-Level Inode System

**Solution: Multi-Level Inodes** — Adds **Indirect Pointers** to extend file size limits beyond what direct pointers allow.

- **Direct Pointers** (point directly to data blocks) (48KB max size)
- **Single Indirect Pointer** (points to a block storing more pointers) (4MB max size)
- **Double Indirect Pointer** (points to a block storing indirect blocks) (4GB max size)
- **Triple Indirect Pointer** (points to a block storing double indirect pointers) (4TB max size)
- **Number of Indirections depends on size of file** in advanced implementations to avoid performance hit

### Pros and Cons of Multi-Level Inodes

| Advantages | Disadvantages |
| --- | --- |
| **Scalable Storage** – Can support very large files | **Access Overhead** – Large files may require multiple lookups |
| **Efficient for Both Small and Large Files** – Direct pointers work for small files | **Complexity** – More layers mean more management |
| **No Preallocation Needed** – Grows dynamically | **Random Access Can Be Slow** – Deeply nested pointers require extra indirection |

---

## EXT

**EXT (Extended File System)**: The default Linux file system family, based on the **multi-level inode system**.

- Internal algorithm attempts to ensure files on disk are located near each other to reduce internal fragmentation
- Advanced versions store **journaling information** (EXT3, EXT4) to prevent corruption

### EXT4 File Access Flow

To read a file (e.g., `/home/user/report.txt`):

1. Start at **inode #2** (always the root directory in ext-family file systems)
2. Read the root directory's data blocks → look up `home` → get its inode number
3. Read inode for `home` → read its data blocks → look up `user` → get its inode number
4. Read inode for `user` → read its data blocks → look up `report.txt` → get its inode number
5. Read inode for `report.txt` → follow **direct/indirect pointers** to read file data

*Figure: The same traversal drawn as a chain — Inode #2 `/ (root)` --read dir--> dir entry `home → 47` --lookup--> Inode #47 `home` --read dir--> dir entry `user → 183` --lookup--> Inode #183 `user` --read dir--> dir entry `report.txt → 590` --lookup--> Inode #590 `report.txt`, which then follows two red "direct ptr" arrows to data Block 1024 and Block 1025. Legend: purple = Inode, orange = Dir Entry, green = Data Block.*

---

## Exercise: Inode

**UNIX-like File System Parameters**

- Total volume size: 200 GiB
- Block size: 4 KiB
- Inode size: 256 bytes
- 1 inode per 8 data blocks
- Pointer size: 4 bytes
- Inode pointers: 10 direct, 2 single-indirect, 2 double-indirect, 1 triple-indirect

Assume indirect blocks are 4 KiB and store only block addresses.

### Questions to Solve

1. Total blocks in the file system
2. Inode table sizing
   - Number of inodes
   - Blocks for inode table
   - Size of inode table in MiB
   - Percentage of volume
3. Maximum file size
   - Contributions from direct, single-, double-, triple-indirect pointers
   - Express in bytes, MiB, GiB

### Solution: Total Blocks

$$\text{Total blocks} = \frac{200 \times 2^{30}}{2^{12}} = 52{,}428{,}800$$

**Explanation:** We first convert 200 GiB to bytes ($200 \times 2^{30}$ bytes), then divide by the block size 4 KiB ($2^{12}$ bytes) to find the number of blocks.

### Solution: Inode Table Sizing

$$\text{Inodes} = \frac{52{,}428{,}800}{8} = 6{,}553{,}600 \quad \text{(1 inode per 8 blocks)}$$

$$\text{Inode table size (bytes)} = 6{,}553{,}600 \times 256 = 1{,}677{,}721{,}600$$

(inode count $\times$ 256 bytes)

$$\text{Blocks for inode table} = \frac{1{,}677{,}721{,}600}{4096} = 409{,}600 \quad \text{(bytes} \div \text{block size)}$$

$$\text{Size (MiB)} = \frac{1{,}677{,}721{,}600}{2^{20}} = 1{,}600 \quad \text{(bytes} \div 2^{20})$$

$$\text{Percentage} = \frac{1{,}600}{200 \times 1024} \times 100 \approx 0.78\% \quad \text{(MiB} \div \text{total MiB)}$$

### Solution: Maximum File Size

**Pointer contributions:**

- Direct (10): $10 \times 4\text{KiB} = 40 \text{ KiB} = 40{,}960$ B
  - 10 pointers $\times$ block size
- Single-indirect (2): $2 \times (1024 \times 4\text{KiB}) = 8{,}388{,}608$ B
  - 2 blocks of 1024 pointers each $\times$ block size
- Double-indirect (2): $2 \times (1024^2 \times 4\text{KiB}) = 8{,}589{,}934{,}592$ B
  - 2 sets of $1024^2$ pointers $\times$ block size
- Triple-indirect (1): $1 \times (1024^3 \times 4\text{KiB}) = 4{,}398{,}046{,}511{,}104$ B
  - 1 set of $1024^3$ pointers $\times$ block size

$$\text{Total max file size} = 4{,}406{,}644{,}875{,}264 \text{ B}$$

---

## Partitions

**Partition**: A **logically divided section** of a storage device. Each partition acts as an **independent volume**. File systems are applied to partitions, not the entire disk.

**Why Partition a Disk?**

- Allows multiple **file systems** on a single disk
- Can **separate system files, user data, and backups**
- Helps with **dual-booting multiple operating systems**

*Figure: A single disk drawn as one bar split into three coloured segments side by side — "Partition 1 (FAT)", "Partition 2 (FAT)", "Partition 3 (NTFS)" — illustrating that different file systems can coexist on one physical device.*

---

## Motherboard, ROM and Firmware

**Motherboard**: The **main circuit board** of a computer. It physically connects and allows communication between the CPU, RAM, storage devices, and all other peripherals.

*Figure: A box labelled "Motherboard" containing five connected components — CPU, RAM, Storage, ROM Chip and Expansion Slots — with lines linking CPU–RAM, CPU–Storage, CPU–ROM Chip, Storage–Expansion Slots and ROM Chip–Expansion Slots.*

**ROM — Read-Only Memory**: A **non-volatile memory chip** on the motherboard that retains its contents **without power**. It stores the firmware. (Modern systems use flash memory that can be updated, but the term ROM persists.)

**Firmware**: Low-level software stored in ROM (or flash). It is the **first code that runs** when the computer powers on — before any operating system.

**Why Do We Need Firmware?** Without firmware, the CPU has **no instructions at power-on**. Firmware initializes hardware, runs diagnostics, and hands control to the OS bootloader. It **bridges hardware and software**.

---

## BIOS

**BIOS — Basic Input/Output System**: The **legacy firmware standard**, found on older computers. Stored in ROM on the motherboard.

**How BIOS Works**

- Runs **POST** (Power-On Self-Test) to verify hardware is functional
- Searches for a bootable disk in a configured order
- Reads the disk's **first 512 bytes** — the Master Boot Record (MBR)
- Executes the **bootloader code** (446 bytes) found in the MBR

### BIOS - Limitations

- Runs in **16-bit real mode** — can only address **1 MB of memory** and use simple instructions; no multitasking or drivers
- No built-in security verification — any code in the MBR is executed blindly
- Tied to the **MBR** partition scheme (max 2 TB disks, 4 primary partitions)

---

## UEFI - Solving BIOS' Limitations

**UEFI — Unified Extensible Firmware Interface**: The **modern replacement** for BIOS. Found on virtually all computers made after ~2012.

---

<!-- source pages 68-79 -->

## UEFI — Advantages over BIOS

**UEFI (Unified Extensible Firmware Interface)** improves on BIOS in the following ways:

- Runs in **32/64-bit mode** — can address **GBs of memory** and run richer pre-boot code
- Has its own **boot manager** — no 446-byte boot-loader limit, can load different OS's boot-loading code and is not limited to a pre-set MBR.
- Reads **GPT** partition tables — supports large disks and many partitions
- Loads boot-loaders from a **dedicated UEFI boot partition** (explained in the GPT slide)
- Supports **Secure Boot** — only executes boot-loaders with a **trusted cryptographic signature**, preventing malware from replacing the boot-loader

---

## The Boot Process

*Figure: Two parallel flowcharts side by side. **BIOS Boot**: Power On + POST → Find boot disk → Read MBR (first 512 bytes) → Execute bootloader code (446 bytes) → Load OS kernel; annotated below as "16-bit real mode / No verification". **UEFI Boot**: Power On + POST → Read GPT → Find UEFI boot partition → Execute `.efi` bootloader → Load OS kernel; annotated below as "32/64-bit mode / Secure Boot supported". The point: both start at POST and end at the kernel, but UEFI replaces the MBR/446-byte-bootloader steps with GPT + an ESP-resident `.efi` binary, gaining wide addressing and Secure Boot.*

| Step | BIOS Boot | UEFI Boot |
|---|---|---|
| 1 | Power On + POST | Power On + POST |
| 2 | Find boot disk | Read GPT |
| 3 | Read MBR (first 512 bytes) | Find UEFI boot partition |
| 4 | Execute bootloader code (446 bytes) | Execute `.efi` bootloader |
| 5 | Load OS kernel | Load OS kernel |
| Mode | 16-bit real mode | 32/64-bit mode |
| Security | No verification | Secure Boot supported |

---

## MBR (Master Boot Record) Disk Partitions

**MBR** is the **traditional partitioning scheme**, primarily used with **BIOS-based booting**. It stores partitioning and boot information in the **first 512 bytes** of the disk.

### MBR Layout (512 Bytes)

- **Bootloader code** (446 bytes) – first-stage code that locates and loads the OS bootloader
- **Partition table** (64 bytes) – 4 entries $\times$ 16 bytes each, defining up to 4 primary partitions
- **Boot signature** (2 bytes) – `0x55AA` magic number confirming a valid MBR

### MBR Limitations

The bootloader must fit in 446 bytes. Only **4 primary partitions** are supported (or 3 primary $+$ 1 extended with logical partitions inside). Maximum addressable disk size: **2 TB**.

---

## GPT (GUID Partition Table)

**GPT** is a **modern partitioning scheme** that replaces MBR. Used in **UEFI-based boot mode**, with a backup header at the end of the disk.

### Advantages of GPT

- **Supports up to 128 partitions** on Windows (Linux may support more)
- **Supports disks larger than 2TB** (max 9.4 ZB)
- **More reliable**: Stores multiple copies of the partition table with CRC32 checksums
- **Protective MBR**: First sector contains a fake MBR so legacy tools don't treat the disk as unformatted

### GPT and UEFI — EFI System Partition (ESP)

A small **FAT32 partition** (typically 100–512 MB) that stores `.efi` bootloader files. UEFI firmware reads this partition directly to find and execute the OS bootloader. Every UEFI system has one.

---

## Linking

### What is Linking?

A **link** is a reference to a file or directory. Types include **Hard Links**, **Symbolic Links**, and **Junctions** (Windows).

- **Hard Links** – Multiple filenames pointing to the same inode, FAT entry or MFT record
- **Symbolic Links (Soft Links)** – A separate file pointing to another file's path

**Why Use Links?** Avoid duplication of large files, maintain references even if files move, and support shared libraries and system shortcuts.

### Hard Links

A **hard link** is a **direct reference** to an existing file system entry. All hard links share the **same inode** and **file data**.

- Deleting one hard link does **not** remove the file until all links are deleted
- Can only be done on files on same partition
- Only works on files, not directories to avoid loops (except for '.' and '..')

*Figure: Example — Hard Link Behavior with Inode. Two boxes, `file1.txt` (left) and `file2.txt` (right), each with a red arrow pointing down to a single shared box `Inode#1001` in the middle. The point: both names are equal, direct references to the same inode.*

Creating a Hard Link:

```bash
$ ln file1.txt file2.txt
```

### Symbolic (Soft) Links

A **symbolic link** is a **shortcut** that points to another file's **path**. Unlike hard links, it does **not share the same inode**.

- If the original file is deleted, the symbolic link **breaks** (becomes invalid)
- Works with directories (Linux)

*Figure: Example — Symbolic Link Behavior. `file2.txt(symlink)` (drawn as a dashed yellow box on the right) has a dashed blue arrow labelled "symlink" pointing left to `file1.txt`; `file1.txt` in turn has a solid red arrow pointing down to `Inode#1001`. The point: the symlink resolves to the target's path/name, not directly to its inode.*

*Note: a further line ("Creating a Symbolic Link") is clipped off the bottom edge of the original slide and its command is not visible.*

### Real-World Use Cases of Linking

**Package Managers** — Package managers like **APT, YUM, Homebrew** use **symbolic links** to ensure applications use the latest installed version.

```
/usr/local/bin/python ->
/usr/local/Cellar/python3/3.9.1/bin/python3
```

**Shared Libraries** — Systems use **symbolic links** to manage shared libraries, helping different applications use **the same version**.

```
/lib/libssl.so -> /lib/libssl.so.1.1
```

---

<!-- source pages 80-97 -->

## File Descriptors

### What is a File Descriptor?

**File Descriptor (FD)**: a **non-negative integer** representing an open file. Every process in Unix manages file descriptors through the kernel. Used for **files, pipes, sockets, and devices**.

Standard File Descriptors:

- $0 \rightarrow$ **Standard Input (stdin)**
- $1 \rightarrow$ **Standard Output (stdout)**
- $2 \rightarrow$ **Standard Error (stderr)**

### Common File Operations in Unix

```c
// 1. Open a file (returns file descriptor)
int fd = open("file.txt", O_RDONLY);

// 2. Read from a file
char buffer[100];
read(fd, buffer, 100);

// 3. Write to a file
write(fd, "Hello", 5);

// 4. Close a file
close(fd);
```

### File Descriptor Table and Process Interaction

*Figure: A vertical chain — `Process` points down to its `File Descriptor Table`, which fans out to four entries `0: stdin`, `1: stdout`, `2: stderr`, `3: file.txt`; all four entries point into a single shared `Kernel File Table`, which in turn points down to `Disk Blocks`.*

### How a Process Holds Open Files

**Process File Management**: Each process maintains a **list of all open files** as part of the **process control block (PCB)**.

- Each entry is a **file descriptor** that maps to:
  - A **File Descriptor Table (per process)**
  - The **Kernel File Table (system-wide)**

Example: A Process with Open Files

```bash
$ cat file.txt > output.log
```

- The process opens **file.txt** for reading and **output.log** for writing.
- These files are tracked in the **process file descriptor table**.

### What Happens If Files Are Not Closed Correctly?

Consequences of Not Closing Files:

- **Resource Leaks:** Open files consume system resources (file table slots)
- **File Locking Issues:** Some files remain locked and cannot be modified by other processes
- **Data Loss and Corruption:** If a file is **not flushed**, data might be lost
- **Exceeding File Descriptor Limits:** Every process (and the system itself) has a **limit on open files**

### Sockets and Other Resources as File Descriptors

**Everything is a File**: In Unix, **everything is a file** – including sockets, pipes, terminals, and devices. All are accessed via **file descriptors**.

- **Sockets** (network communication)
- **Pipes** (inter-process communication)
- **Terminals and Devices**

**Why Use File Descriptors for Sockets?** Allows **uniform handling of files and network resources**. Can use `read()`, `write()`, `close()` on sockets, just like files.

---

## Exercise: File Descriptors After `fork()`

Given Code:

```c
int main() {
    int fd = open("log.txt",
                  O_WRONLY | O_CREAT | O_TRUNC, 0644);
    // fd = 3 (after stdin=0, stdout=1, stderr=2)

    pid_t pid = fork();

    if (pid == 0) {          // Child
        write(fd, "CHILD\n", 6);
        close(fd);
        exit(0);
    } else {                 // Parent
        wait(NULL);
        write(fd, "PARENT\n", 7);
        close(fd);
    }
    return 0;
}
```

### Questions

1. After `fork()`, how many file descriptors point to `log.txt`? Explain why.
2. Do the parent and child share the same **file offset**, or does each have an independent offset?
3. What are the **possible contents** of `log.txt` after the program finishes?
4. If the child calls `close(fd)` **before** writing, can the parent still write to the file? Why?

### Solutions

**Q1: Two FDs** (one in each process). `fork()` duplicates the process's file descriptor table. Both parent and child have `fd = 3`, and both entries point to the **same entry in the kernel's open file table**.

**Q2: Shared offset**. Since both FDs point to the same kernel file table entry, they share the file offset. When the child writes 6 bytes, the offset advances to 6 for **both** processes.

**Q3:** The parent calls `wait(NULL)`, so the order is deterministic – child writes first, then parent. The contents will be:

```
CHILD
PARENT
```

Without `wait()`, both orderings would be possible.

**Q4: Yes.** `close(fd)` only closes the child's file descriptor. The kernel file table entry remains open because the parent still references it. The entry is only freed when **all** FDs pointing to it are closed (reference counted).

---

## Pipes

### What Are Pipes and Their Role in IPC?

**Pipe**: a mechanism for **inter-process communication (IPC)**. Pipes allow **data to flow between processes** via **unidirectional communication**.

Example: Unix Pipe

```bash
$ ls | grep "file"
```

Key Features:

- **Efficient:** Direct memory buffer for process communication
- **Synchronization:** Writer blocks if the pipe is full, reader blocks if empty
- **Two Types:** **Anonymous pipes** and **named pipes (FIFOs)**

### Anonymous (Unnamed) Pipes

**Anonymous Pipe**: a temporary communication channel between **parent and child processes**. Created using **`pipe()`** and **automatically destroyed** when processes exit.

Creating a Pipe in C (Part 1):

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int fd[2];  // File descriptors for the pipe
    char buffer[6]; // Extra byte for null
        terminator

    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
```

Creating a Pipe in C (Part 2):

```c
    if (fork() == 0) {  // Child process
       close(fd[0]);  // Close unused read end
       write(fd[1], "Hello", 5);
       close(fd[1]);  // Close write end after
           writing
       exit(EXIT_SUCCESS);
    } else {  // Parent process
       close(fd[1]);  // Close unused write end
       read(fd[0], buffer, 5);
       buffer[5] = '\0';  // Null-terminate for
           print
       printf("Parent received: %s\n", buffer);
       close(fd[0]);  // Close read end after
           reading
    }
```

*Note: the comment wrapping in both code listings (`terminator`, `writing`, `print`, `reading` on continuation lines) is how the slides render the source.*

Limitations:

- **Works only between related processes** (parent-child)
- **Cannot communicate between unrelated processes**

### Named Pipes (FIFOs)

**Named Pipe (FIFO)**: a **persistent pipe** that exists in the file system. Allows **unrelated processes** to communicate. Created using `mkfifo`.

Example: Creating and Using a FIFO

```bash
$ mkfifo mypipe      # Create named pipe
$ echo "Hello" > mypipe  # Write to pipe
$ cat < mypipe       # Read from pipe
```

Key Features:

- **Exists as a file** – persists after process termination
- **Can be used by any process** – not limited to parent-child
- **Behaves like a regular file** but reads/writes are blocking

---

<!-- source pages 98-113 -->

## RAID

### What is RAID?

**RAID** — **Redundant Array of Independent Disks** — a method to combine multiple disks for performance, fault tolerance, or both.

- Performance improvement (striping)
- Fault tolerance (redundancy)
- Both (hybrid methods)

**Key insight:** different **RAID levels** offer trade-offs in speed, redundancy, and efficiency.

---

### RAID 0 — Striping for Performance

- Data is split into **blocks** and spread across multiple disks
- **No redundancy**, but offers **high performance**

**Warning:** if one disk fails, **all data is lost**.

### RAID 1 — Mirroring for Redundancy

- Data is duplicated on **two disks**
- **High redundancy** but **half storage efficiency**
- Can survive **one disk failure** without data loss

### RAID 1+0 (RAID 10) — Mirroring + Striping

- **Mirroring first**, then **striping** for performance
- Can survive **multiple disk failures**, provided that **no mirrored pair is lost**
- Good for **high-performance, high-redundancy setups**

### RAID 4 — Dedicated Parity Disk

- **Data is striped** across disks (like RAID 0), with a **dedicated parity disk**
- **Can recover from one disk failure** using the parity disk

**Warning:** **parity disk can become a bottleneck** for writes.

### RAID 5 — Distributed Parity for Fault Tolerance

- Data is striped across **multiple disks**
- **Parity is distributed** across disks to prevent bottlenecks
- Can survive **one disk failure** without losing data

### RAID 6 — Double Parity for Extra Redundancy

- Like RAID 5 but with **two parity blocks** per stripe
- Can survive **two simultaneous disk failures**
- **Increased reliability** but **higher write overhead**

---

### XOR-Based Data Recovery in RAID 4/5/6

RAID 4, 5, and 6 use **XOR operations** to calculate parity. If a disk fails, missing data is **reconstructed using XOR**.

**XOR operations:**

- Calculating Parity: $A1 \oplus A2 \oplus A3 = P$
- Recovering a Lost Disk: $A1 \oplus A2 \oplus P = A3$

**RAID 6 enhancements:**

- Uses **two levels of parity** for dual-disk failure recovery
- Implements **Weighted XOR** to improve reconstruction efficiency

---

### Exercise: RAID Capacity Calculation

**Parameters:** 6 identical disks, each 2 TiB.

Fill in the table:

|  | RAID 0 | RAID 1 | RAID 5 | RAID 6 | RAID 10 |
|---|---|---|---|---|---|
| Usable capacity | ? | ? | ? | ? | ? |
| Max disk failures | ? | ? | ? | ? | ? |

#### Solution: RAID Usable Capacity

|  | RAID 0 | RAID 1 | RAID 5 | RAID 6 | RAID 10 |
|---|---|---|---|---|---|
| Usable capacity | 12 TiB | 6 TiB | 10 TiB | 8 TiB | 6 TiB |
| Max disk failures | 0 | 5 | 1 | 2 | 1–3 |

**Capacity formulas:**

- **RAID 0:** $N \times S = 6 \times 2 = 12$ TiB   (no redundancy)
- **RAID 1:** $\frac{N}{2} \times S = 3 \times 2 = 6$ TiB   (full mirror)
- **RAID 5:** $(N - 1) \times S = 5 \times 2 = 10$ TiB   (1 disk of parity)
- **RAID 6:** $(N - 2) \times S = 4 \times 2 = 8$ TiB   (2 disks of parity)
- **RAID 10:** $\frac{N}{2} \times S = 3 \times 2 = 6$ TiB   (mirror + stripe)

#### Solution: RAID Fault Tolerance

- **RAID 0: 0 failures** — no redundancy; any single disk failure loses all data
- **RAID 1: 5 failures** — 3 mirror pairs; can lose all but one disk in each pair. In the best case, 5 of 6 disks can fail (one survivor per pair is enough)
- **RAID 5: 1 failure** — one disk's worth of distributed parity allows reconstructing any single failed disk via XOR
- **RAID 6: 2 failures** — two independent parity blocks per stripe allow recovering from any two simultaneous disk failures
- **RAID 10: 1–3 failures** — guaranteed to survive 1 failure; can survive up to 3 if no mirror pair loses both disks

**Warning — RAID 10 Depends on *Which* Disks Fail:** With 3 mirror pairs (AB, CD, EF): losing A and C is fine (1 survivor per pair), but losing both A and B destroys that pair's data — even though only 2 disks failed.

---

### Exercise: XOR Parity & Recovery

**RAID 5 stripe data.** A RAID 5 array with 4 disks. One stripe contains the following 8-bit data blocks:

| Disk 1 | Disk 2 | Disk 3 | Disk 4 (Parity) |
|---|---|---|---|
| 10110011 | 01101010 | 11010001 | ? |

**Questions:**

1. **Calculate** the parity block stored on Disk 4.
2. Disk 2 **fails**. Reconstruct its data using the remaining disks.

#### Solution — Q1: Calculate Parity

XOR all data disks together:

```text
    10110011   (Disk 1)
⊕   01101010   (Disk 2)
=   11011001
⊕   11010001   (Disk 3)
=   00001000   (Parity → Disk 4)
```

#### Solution — Q2: Reconstruct Disk 2

XOR all surviving disks (including parity):

```text
    10110011   (Disk 1)
⊕   11010001   (Disk 3)
=   01100010
⊕   00001000   (Disk 4 --- parity)
=   01101010   (Recovered Disk 2)
```

**Why this works:** XOR has two key properties: $A \oplus A = 0$ and $A \oplus 0 = A$. When we XOR all surviving disks including parity, the failed disk's data is the only value that "remains" — everything else cancels out.

---

## See You Next Week!

Questions?
