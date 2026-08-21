# Recitation 7 — Virtualization

> Study notes converted from the recitation slide deck `Rec7_OS.pdf` (60 pages) — Operating Systems, Reichman University, Spring Semester 2026.
> Lecturer: Tsvi Cherny-Shahar · TAs: Daniel Karalnik, Liam Tal.
> Faithful conversion: every substantive item on the source pages appears here, and nothing has been added from outside them. Figures are rendered as italic *Figure:* descriptions.
> `<!-- source pages a-b -->` comments mark the original page ranges.

---

<!-- source pages 1-18 -->

## Recitation 7 — Virtual Machines & Virtualization

## Table of Contents

1. A Brief History of Virtualization
2. What is Virtualization?
3. Hypervisor Types
4. CPU Virtualization
5. Memory Virtualization
6. Containers vs Virtual Machines
7. Virtualization & Security
8. The Cloud Abstraction Ladder

---

## A Brief History of Virtualization

### The Origins (1960s–1990s)

**It Started with IBM**

IBM CP-40 (1966) and CP/CMS (1967) were the first systems to fully virtualize a mainframe — letting multiple users each run their own isolated "virtual machine."

- **1966 — IBM CP-40**: First true virtual machine implementation
- **1974 — Popek & Goldberg** publish formal virtualization requirements
- **1998 — VMware founded**: Brings virtualization to commodity x86 hardware
  - x86 was considered "non-virtualizable" at the time because some sensitive instructions didn't trap

### The Modern Era (2000s–Today)

- **2003 — Xen** hypervisor released (paravirtualization approach)
- **2005–2006 — Intel VT-x / AMD-V**: Hardware-assisted virtualization finally makes x86 efficiently virtualizable
- **2006 — AWS EC2 launches**: Virtualization becomes the foundation of cloud computing
- **2013 — Docker**: Containers bring OS-level virtualization to the mainstream
- **2014 — Kubernetes**: Container orchestration at scale
- **Today**: Billions of VMs and containers running simultaneously worldwide

**Key Insight.** Every time you use a cloud service, stream a video, or open a web app — virtualization is involved somewhere in the stack.

---

## What is Virtualization?

### Why Virtualization?

**From University to Industry**

You already understand processes as isolated execution environments — virtualization takes this idea further by isolating **entire operating systems**.

- **Server consolidation** — run multiple workloads on one physical machine
- **Isolation & security** — separate **tenants** (independent users/organizations) on shared hardware
- **Development & testing** — reproduce environments reliably
- **Cloud computing** — the foundation of AWS, Azure, GCP

### The Idea Behind a Virtual Machine

**A Computer Inside a Computer**

Imagine you could take a **physical computer** — with its own CPU, memory, disk, and network — and recreate it entirely in **software**. A program running inside this fake computer wouldn't know the difference. That's a virtual machine.

- You already know this idea at a smaller scale:
  - A **process** thinks it has its own CPU (scheduling) and its own memory (virtual address space)
  - A **virtual machine** takes this further — an entire OS thinks it has its own hardware
- The key trick: a software layer **intercepts** hardware access and **fakes** the responses, so the OS running inside never realizes it's not on real hardware

### What is a Virtual Machine? — Formal Definition

**Virtual Machine**

A **software emulation** of a complete computer system that runs its own operating system (the **guest**) on top of a physical machine (the **host**), managed by a **Virtual Machine Monitor (VMM)** or **hypervisor**.

- **Host OS / Host machine** — the physical hardware + its OS
- **Guest OS** — the OS running inside the VM
- **Hypervisor / VMM** — the software layer that creates and manages VMs

### Virtualization Abstraction Layers

*Figure: A four-box vertical stack with downward arrows, top to bottom — Applications ("Runs inside Guest OS"), Guest OS ("Thinks it controls real hardware"), Hypervisor / VMM ("Intercepts and manages"), Physical Hardware (CPU, Memory, Disk, Network) ("Actual physical resources"). It shows the hypervisor inserted as an extra abstraction layer between the guest OS and the real machine.*

---

## Hypervisor Types

### What is a Hypervisor?

**The OS of Operating Systems**

You know that the **OS manages processes** — it decides which process runs, how much memory it gets, and keeps processes from interfering with each other. A **hypervisor** does the same thing, but for **entire operating systems**.

**Hypervisor / VMM**

A software (or firmware) layer that **creates, runs, and manages virtual machines**. It sits between the VMs and the physical hardware, controlling access to CPU, memory, disk, and network.

### Hypervisor Roles

- A Hypervisor, also called a **Virtual Machine Monitor (VMM)**
- Responsible for:
  - **Multiplexing** hardware across multiple guest OSes
  - **Isolating** guests from each other and from the host
  - **Emulating** or mediating hardware access for each guest

### Type 1 Hypervisor (Bare-Metal)

**Type 1 — Bare-Metal**

Runs **directly on hardware** — no host OS. The hypervisor **is** the OS.

- Examples: **VMware ESXi**, **Microsoft Hyper-V**, **Xen**, **KVM** (Linux kernel module)
- Used in **data centers and cloud providers** (For example, AWS's EC2 runs on a type 1 hypervisor)
- Lower overhead, better performance

### KVM — The Linux Approach

**KVM (Kernel-based Virtual Machine)**

A **Linux kernel module** that turns the Linux kernel itself into a **Type 1 hypervisor** (although lines become a bit blurred). Instead of replacing the OS, KVM **extends** it — Linux keeps running normally, but gains the ability to host VMs.

- KVM leverages **hardware-assisted virtualization** (VT-x / AMD-V) — without it, KVM cannot run
- KVM handles **CPU and memory virtualization**; QEMU for everything else (disk, network, display).

### Type 2 Hypervisor (Hosted)

**Type 2 — Hosted**

Runs **on top of a host operating system** as a regular application.

- Examples: **VirtualBox**, **VMware Workstation**, **Parallels**
- Common for **development, testing, and education**
- Higher overhead — must go through the host OS

### Type 1 vs Type 2 — Comparison

*Figure: Two side-by-side layer stacks. Type 1 (Bare-Metal), bottom to top: Hardware → Hypervisor → two "Guest OS + Apps" boxes. Type 2 (Hosted), bottom to top: Hardware → Host OS → Hypervisor (app) → two "Guest OS + Apps" boxes.*

| Property | Type 1 | Type 2 |
| --- | --- | --- |
| Performance | Near-native | Higher overhead |
| Use-case | Data centers, cloud | Dev, testing, education |
| Overhead | Low | Moderate |
| Examples | ESXi, KVM, Xen | VirtualBox, Parallels |

---

<!-- source pages 19-33 -->

## CPU Virtualization

### The Core Challenge

**The Problem**

A guest OS expects to execute **privileged instructions** (e.g., modifying page tables, handling interrupts). But only the **real OS / hypervisor** should be allowed to do this.

**Key Insight**

This is the same protection problem you already know from **user mode vs kernel mode** — but now applied one level up.

### Sensitive vs Privileged Instructions

Two categories of instructions:

- **Privileged instruction** — an instruction that **traps** (causes a fault) when executed outside kernel mode. You already know these from user/kernel mode.
- **Sensitive instruction** — any instruction whose behavior **depends on the privilege level** or that could **reveal/affect the true state of the machine**. This is a broader category.

**Key Insight**

All privileged instructions are sensitive, but not all sensitive instructions are privileged. This gap is the core problem for virtualization.

---

## Approaches to CPU Virtualization

### Trap-and-Emulate

**Trap-and-Emulate** — The guest OS runs in **user mode**. When it attempts a **privileged instruction**, the CPU **traps** to the hypervisor, which **emulates** the effect on behalf of the guest.

- Works naturally when sensitive instructions are also privileged — the CPU traps, and the hypervisor handles them
- Problem: x86 (before VT-x) has instructions that are **sensitive but not privileged** — they don't trap, they silently behave differently

**The x86 Problem**

Around 17 instructions in the original x86 ISA were sensitive but not privileged. Trap-and-emulate alone **cannot work** on x86 — the hypervisor never gets control when these instructions execute.

### Full Emulation (Different CPU Architectures)

**Full Emulation** — The hypervisor **interprets every guest instruction in software**, translating it to the host's native instruction set. Used when guest and host have **different CPU architectures**.

- **QEMU** — can emulate ARM on x86, MIPS on ARM, etc. Every instruction is decoded and re-executed.

**Performance Cost**

Full emulation is **orders of magnitude slower** than native execution. Acceptable for development/compatibility, not for production workloads.

### Binary Translation (Same CPU, Software Workaround)

**Binary Translation** — The hypervisor **scans guest code at runtime** and dynamically rewrites problematic instructions into safe equivalents. The host CPU executes the rewritten code natively.

- **VMware's breakthrough (1999)**: Made x86 virtualization practical before hardware support existed
- Only **sensitive non-privileged instructions** need translation — safe instructions run at native speed
- Translated blocks are **cached** so repeated code runs fast

**Key Insight**

This is how VMware solved the "x86 is not virtualizable" problem — by rewriting the few problematic instructions on-the-fly.

### Paravirtualization

**Paravirtualization** — Instead of tricking the guest OS into thinking it runs on real hardware, the guest is **modified to cooperate** with the hypervisor. The guest **knows** it's virtualized and uses special **hypercalls** to communicate directly with the hypervisor.

**Hypercalls**

A **hypercall** is to the hypervisor what a **system call** is to the OS. Instead of executing a privileged instruction and waiting for a trap, the guest explicitly calls the hypervisor to perform the operation — faster and with no ambiguity.

#### Pros and Cons

- Pioneered by **Xen** (2003) — required modifying the guest Linux kernel
- **Pros**: Better performance than binary translation, no hardware support needed
- **Cons**: Cannot run **unmodified** guest OSes (e.g., Windows couldn't be paravirtualized without Microsoft's cooperation)

**Key Insight**

Today, paravirtualization lives on in **hybrid form** — modern VMs use hardware-assisted CPU virtualization but paravirtualized I/O drivers (**virtio**) for better disk and network performance.

### System Call Translation (WSL1)

**System Call Translation** — Instead of virtualizing hardware, the host OS **translates guest system calls** into its own native calls. No separate kernel runs.

**WSL1 (2016)**

Windows translated **Linux system calls** into equivalent Windows NT kernel calls. No Linux kernel running at all — just a translation layer.

#### Pros and Cons

- Fast for simple workloads, but **incomplete**
- Not all Linux syscalls had NT equivalents (e.g., `fork()` semantics differ fundamentally)
- Some Linux software simply couldn't run

**Fundamental Limitation**

Perfect translation requires the host kernel to support every semantic of the guest kernel's API — and Linux and Windows kernels were designed with very different assumptions.

### From Translation to VM (WSL2)

**WSL2 (2019)**

Microsoft gave up on translation and switched to running a **real Linux kernel** inside a **lightweight** VM. Full Linux compatibility, slightly more overhead.

- The VM is managed by **Hyper-V** — Windows' built-in **Type 1 hypervisor**
- **Lightweight VM**: a stripped-down VM with a **minimal kernel**, **no full OS boot**, and **shared resources** with the host (e.g., file system, networking) — starts in $\sim 1$ second instead of minutes
- Unlike a traditional VM, it doesn't carry a full guest OS with its own desktop, services, and drivers — just the kernel and what's needed to run Linux processes

### Hardware-Assisted Virtualization

**Hardware-Assisted Virtualization** — Modern CPUs (Intel **VT-x**, AMD **AMD-V**) add a new privilege level — sometimes called **Ring -1** or **VMX root mode** — where the hypervisor runs, while the guest OS runs in its normal Ring 0.

**Key Insight**

With hardware support, the guest OS can execute privileged instructions natively — the CPU itself knows about virtualization and traps to the hypervisor only when necessary. No binary translation needed.

#### Why do we even need this at the Hardware Level?

**VM Exit**

A **VM exit** occurs when the CPU transfers control from the guest back to the hypervisor — for example, when the guest executes a sensitive instruction or accesses a device. The hypervisor handles the event, then resumes the guest (**VM entry**). VM exits are expensive (thousands of cycles), so minimizing them is a key performance goal.

---

## The Virtualization Spectrum

*Figure: A left-to-right arrow ordering the four techniques from most to least overhead — Full Emulation → Binary Translation → Paravirtualization → Hardware-Assisted; the bar is colored red at the left ("Most overhead") fading to green at the right ("Least overhead").*

| Technique | Characteristics |
|---|---|
| Full Emulation | Different CPU arch; Slowest — **Most overhead** |
| Binary Translation | Same arch; Rewrite at runtime |
| Paravirtualization | Modified guest OS; Hypercalls |
| Hardware-Assisted | CPU support; Near-native speed — **Least overhead** |

**Key Insight**

In practice, modern hypervisors combine techniques — e.g., KVM uses hardware-assisted CPU virtualization, paravirtualized I/O (virtio), and emulation for legacy devices.

---

## Memory Virtualization

---

<!-- source pages 34-51 -->

## Memory Virtualization

### The Double Translation Problem

**Two Levels of Translation.** In a VM, each memory access requires **two translations**:

- **Guest Virtual Address (GVA)** $\rightarrow$ **Guest Physical Address (GPA)** — using the guest's page table
- **Guest Physical Address (GPA)** $\rightarrow$ **Host Physical Address (HPA)** — using the hypervisor's mapping

*Figure: A three-box chain GVA → GPA → HPA. The first arrow is labelled "Guest Page Table" and annotated "Managed by Guest OS"; the second arrow is labelled "Hypervisor Mapping" and annotated "Managed by Hypervisor".*

#### Double Trouble

The guest OS thinks it controls physical memory at address `0x1000` — but that's really a *guest physical* address. The hypervisor maps it to some completely different host physical address.

### Shadow Page Tables

**Shadow Page Tables.** The hypervisor maintains a **hidden, combined page table** that maps directly from **GVA** $\rightarrow$ **HPA**, bypassing the two-step translation. The CPU's actual page table register (CR3) points to the **shadow**, not the guest's table.

*Figure: "Guest PT (GVA → GPA)", maintained by the guest, is combined ("compose", dashed arrow) with the "Hypervisor Map (GPA → HPA)", maintained by the hypervisor, which "produces" the "Shadow PT (GVA → HPA)". CR3 points up at the Shadow PT, labelled "Used by CPU".*

#### Shadow Page Table Overhead

**Expensive to Maintain.** Every time the guest **modifies its page table** (new process, page fault, `mmap`), the hypervisor must **intercept** the change (VM exit / trap) and **update the shadow** to match. Every guest CR3 switch (context switch) may require **switching or rebuilding** the shadow table.

- **Frequent VM exits** — each trapped page table write is expensive (thousands of cycles)
- **Memory overhead** — hypervisor stores a shadow PT for every guest process
- **Complexity** — hypervisor must track all guest PT modifications in real-time

#### The Shadow Problem

Shadow page tables essentially require the hypervisor to be a "mirror" of the guest's memory management — any desync causes crashes or security holes.

### Extended Page Tables (EPT / NPT)

**Extended Page Tables.** A **hardware feature** (Intel EPT, AMD NPT) that lets the CPU perform **both translation levels in hardware**. The guest page table maps GVA $\rightarrow$ GPA; a second hardware-managed table maps GPA $\rightarrow$ HPA. The CPU walks both automatically.

- **No shadow tables needed** — the guest manages its own page table freely, no traps required
- **No VM exits** on guest page table modifications — huge performance win
- Trade-off: a **TLB miss** now requires a **2D page walk** (guest levels $\times$ host levels), which can be slow. Mitigated by **large pages** and **EPT caching**.

### Shadow PT vs EPT — Comparison

| Property | Shadow PT | EPT / NPT |
| --- | --- | --- |
| Maintained by | Hypervisor (software) | CPU (hardware) |
| VM exits on PT changes | Yes (frequent) | No |
| Memory overhead | Shadow PT per process | Single EPT per VM |
| TLB miss cost | Standard page walk | Nested 2D walk |
| Hardware required | None | Intel EPT / AMD NPT |
| Used in practice | Legacy hypervisors | All modern hypervisors |

**TLB and Virtual Machines.** When switching between VMs, the TLB must distinguish entries from different guests. **Tagged TLBs** (Intel **VPID**, AMD **ASID**) assign an ID to each VM — no full flush needed on VM switch. Same idea as PCID for process context switches — but now at the VM level.

---

## Containers vs Virtual Machines

### OS-Level Virtualization

**Container.** A **lightweight, isolated execution environment** that shares the **host kernel** instead of running its own OS. Provides process-level isolation using OS features.

- No separate guest OS — containers are just **isolated processes**
- Much faster startup, lower memory overhead
- Industry standard: **Docker**, orchestrated by **Kubernetes (K8s)**

### Linux Building Blocks: Namespaces

**Namespaces.** The Linux kernel feature that provides **isolation** — each container gets its own view of system resources.

| Namespace | What it isolates |
| --- | --- |
| `PID` | Process IDs (container sees its own PID 1) |
| `NET` | Network interfaces, IP addresses, ports |
| `MNT` | Mount points / file system view |
| `UTS` | Hostname |
| `IPC` | Inter-process communication |
| `USER` | UID/GID mapping |

### Linux Building Blocks: cgroups

**Control Groups (cgroups).** The Linux kernel feature that provides **resource limiting** — controls how much CPU, memory, disk I/O, etc. a container can consume.

- **CPU limits** — max percentage or specific cores
- **Memory limits** — hard cap, OOM killer integration
- **I/O bandwidth** — throttle disk and network

**Warning.** Without cgroups, a single runaway container could starve the entire host — similar to how without scheduling, a single process could monopolize the CPU.

### Container Runtimes

**Container Runtime.** The software that actually **creates and runs containers** — it talks to the kernel to set up namespaces, cgroups, and the root filesystem for each container. Think of it as the "hypervisor equivalent" for containers.

**Our Context**

- In this recitation we will focus on docker containers ("containerd").

### Docker in Practice: Images and Containers

**Docker Image.** A **read-only template** containing the application code, runtime, libraries, and dependencies. An image is a **blueprint**; a container is a **running instance** of that image.

- **Image** = packaged file system snapshot (like a class in OOP)
- **Container** = running process created from an image (like an instance)
- Images are stored in **registries** (Docker Hub, AWS ECR, GitHub Container Registry)

### Docker in Practice: Layers

**Image Layers.** Docker images are built from **stacked read-only layers**. Each instruction in a Dockerfile creates a new layer. Containers add a thin **writable layer** on top.

*Figure: A stack of layers, bottom to top — "Base OS — `ubuntu:22.04`", "Install packages — `apt-get install python3`", "Copy application code — `COPY app/ /app`", "Configuration — `CMD ["python3", "app.py"]`" (all bracketed as Read-only), topped by a dashed "Writable container layer" marked Read-write.*

Layers are **shared** between images. If 10 containers use `ubuntu:22.04` as a base, that layer is stored only **once** on disk.

### Docker in Practice: Dockerfile Example

```dockerfile
FROM python:3.11-slim
WORKDIR /app
COPY requirements.txt .
RUN pip install -r requirements.txt
COPY . .
EXPOSE 8080
CMD ["python", "server.py"]
```

- Each line creates a **layer** — ordering matters for **cache efficiency**
- `COPY requirements.txt` before `COPY .` ensures dependency layer is cached unless deps change

### VMs vs Containers — Architecture

*Figure: Two side-by-side stacks. Virtual Machines: Hardware → Hypervisor → two VMs, each a stack of Guest OS / Bins/Libs / App. Containers: Hardware → Host OS → Container Runtime (Docker) → two containers, each just Bins/Libs / App.*

VMs each carry a full **Guest OS** — containers **share the host kernel**.

### VMs vs Containers — Comparison

| Property | Virtual Machines | Containers |
| --- | --- | --- |
| Isolation | Full hardware | Process-level (shared kernel) |
| Startup time | Seconds to minutes | Milliseconds to seconds |
| Overhead | High (full OS per VM) | Low (shared kernel) |
| Image size | GBs (includes full OS) | MBs (app + deps only) |
| OS support | Any OS on any host | Same kernel as host |
| Security | Hypervisor (strong) | Namespaces (weaker) |
| Use-case | Multi-tenant, diff. OSes | Microservices, CI/CD |

### When to Use What?

**Industry Rule of Thumb**

- **VMs** when you need **strong isolation**, different OS types, or **multi-tenant security** (cloud providers, banks)
- **Containers** when you need **fast deployment, scaling, and microservices** (web apps, CI/CD pipelines)
- **Both together** — the most common production pattern: containers running inside VMs for defense-in-depth (e.g., AWS ECS on EC2, GKE nodes are VMs running containers)

---

<!-- source pages 52-60 -->

## Virtualization & Security

### The Security Promise

**Isolation as Security** — VMs provide a **security boundary**: a compromised guest should not be able to affect the host or other guests.

- Hypervisor is a **small, auditable attack surface** (especially Type 1)
- Each VM has its own **address space, kernel, devices** — full separation
- This is what enables **multi-tenant cloud** — your VM runs on the same physical hardware as a stranger's VM

### VM Escape Attacks

**VM Escape** — An attack where malicious code running **inside a guest VM** exploits a vulnerability in the **hypervisor** to break out and access the host or other VMs.

Key insight: VM escape is considered one of the most severe vulnerabilities in cloud computing — it breaks the fundamental trust model.

### Container Security Considerations

**Weaker Isolation** — Containers share the host kernel, so a **kernel vulnerability** affects all containers on the host.

- VMs: attacker must escape the hypervisor (small attack surface)
- Containers: attacker must escape namespace/cgroup boundary (larger kernel attack surface)

**Defense in Depth:**

- Containers inside VMs (isolation + efficiency)
- **seccomp** — filter which system calls a container can make
- **AppArmor / SELinux** — Security Extensions
- Read-only file systems, minimal base images
- **Rootless containers** — processes run as unprivileged users on the host

### MicroVMs — Best of Both Worlds

**MicroVM** — A **minimal, purpose-built virtual machine** designed for running a **single container or function**. Strips away everything a traditional VM doesn't need — no BIOS, no USB, no display — keeping only a tiny device model for disk and network.

**AWS Firecracker** — Built by AWS on top of **KVM**. Each Lambda function or Fargate container runs inside its own Firecracker microVM.

- Boots a Linux kernel in $\sim$**125 ms** (vs. seconds for a traditional VM)
- Memory overhead of $\sim$**5 MB** per microVM
- Runs **thousands of microVMs** on a single host
- Each microVM gets **its own kernel** — a kernel exploit in one container cannot reach another

### Security Comparison Summary

| Aspect | Virtual Machines | Containers |
| --- | --- | --- |
| Isolation | Hypervisor + kernel | Namespaces + cgroups |
| Attack surface | Small (hypervisor) | Large (host kernel) |
| Escape severity | Critical (rare) | High (more common) |
| Mitigations | HW support, minimal VMM | seccomp, AppArmor, rootless |

---

## The Cloud Abstraction Ladder

### From Hardware to Serverless

*Figure: A four-rung ladder drawn bottom-to-top — Physical Servers, then IaaS — VMs (EC2, Azure VMs, GCP Compute), then CaaS — Containers (ECS, GKE, ACI), then FaaS — Serverless (Lambda, Cloud Functions). A left-hand axis runs "More control" (bottom) to "More abstraction" (top); a right-hand axis runs "More ops work" (top) to "Less ops work" (bottom).*

- Physical Servers
- **IaaS** — VMs (EC2, Azure VMs, GCP Compute)
- **CaaS** — Containers (ECS, GKE, ACI)
- **FaaS** — Serverless (Lambda, Cloud Functions)

Moving up the ladder: more control $\longleftrightarrow$ more abstraction, and less ops work $\longleftrightarrow$ more ops work.

---

## Good Luck on Your Exams!

It's been a great semester.

Keep exploring — the OS is everywhere.
