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
