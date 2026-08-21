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
