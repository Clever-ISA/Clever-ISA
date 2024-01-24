# Clever ISA

## Notation

This document uses encoding notation denoted by square brackets, with a sequence of letters, zeroes and ones. Each unit denotes a single bit, and may be grouped together with the same letter to form larger values. A zero indicates a bit that is required to be `0`, and a one indicates a bit that is required to be `1`. 

For Example, the encoding `[11 00 aa bb]` denotes an 8-bit value which starts with 2 `1` bits, followed by 2 `0` bits, then 2 bits called `a`, and 2 bits called `b`. The named groups of bits will have meaning assigned in the following paragraph. 

Bits denoted by encoding notation occur in descending order of significance.

Encodings that denote values longer than 8 bits are in Big-Endian (Most significant byte a lowest order address). 

## License

Copyright (c)  2021  Connor Horman.
Permission is granted to copy, distribute and/or modify this document
under the terms of the GNU Free Documentation License, Version 1.3
or any later version published by the Free Software Foundation;
with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
A copy of the license is included in the repository, under the file entitled LICENSE. Otherwise, you may find a copy at <https://www.gnu.org/licenses/fdl-1.3.en.html>.


## Registers

| r   | Register Name | Availability   |                                  Notes                                  |
| --- | ------------- | -------------- |:-----------------------------------------------------------------------:|
| 0   | r0/racc       | General        |                                                                         |
| 1   | r1/rsrc       | General        | Source Pointer Register for block instructions.                         |
| 2   | r2/rdst       | General        | Destination Pointer Register for block instructions                     |
| 3   | r3/rcnt       | General        | Counter for block instructions                                          |
| 4   | r4            | General        |                                                                         |
| 5   | r5            | General        |                                                                         |
| 6   | r6/fbase      | General        |                        Stack Frame Base Address                         |
| 7   | r7/sptr       | General        |                    Stack Pointer/Frame Head Address                     |
| 8   | r8            | General        |                                                                         |
| 9   | r9            | General        |                                                                         |
| 10  | r10           | General        |                                                                         |
| 11  | r11           | General        |                                                                         |
| 12  | r12           | General        |                                                                         |
| 13  | r13           | General        |                                                                         |
| 14  | r14           | General        |                       Fast Indirect Call Address                        |
| 15  | r15/link      | General        |                        Fast Call Return Address                         |
| 16  | ip            | Readonly[^1]   |                           Instruction Pointer                           |
| 17  | flags         | Flags          |                            Processor Status                             |
| 18  | mode          | Readonly        |                         Processor Execution Mode            |
| 63  | reserved      | Reserved       | Undefined register. Will not be given meaning in a future version |
| 128 | cr0           | Supervisor     |                            Processor Control                            |
| 129 | cr1/page      | Supervisor     |            Physical Address of the Virtual Memory Page Table            |
| 130 | cr2/flprotect | Supervisor     |                  Supervisor Write protected flag bits.                  |
| 131 | cr3/scdp      | Supervisor     |                   Supervisor Call Destination Address                   |
| 132 | cr4/scsp      | Supervisor     |                      Supervisor Call Stack Pointer                      |
| 133 | cr5/sccr      | Supervisor     |                      Supervisor Call State Control                      |
| 134 | cr6/itabp     | Supervisor     |                    Interrupt/Exception Table Pointer                    |
| 135 | cr7/ciread    | Supervisor     | Bitflag of cpuinfo registers that can be read by program execution mode |
| 136 | cpuidlo       | CPUInfo        |        Contains the low order 64 bits of the Processor ID String        |
| 137 | cpuidhi       | CPUInfo        |        Contains the hi order 64-bits of the Processor ID String         |
| 138 | cpuex2        | CPUInfo        |           Processor Extension Availability Field 2. See below           |
| 139 | cpuex3        | CPUInfo        |     Processor Extension Availability Field 3. Contains the value 0      |
| 140 | cpuex4        | CPUInfo        |     Processor Extension Availability Field 4. Contains the value 0      |
| 141 | cpuex5        | CPUInfo        |     Processor Extension Availability Field 5. Contains the value 0      |
| 142 | cpuex6        | CPUInfo        |     Processor Extension Availability Field 6. Contains the value 0      |
| 143 | mscpuex       | CPUInfo        |        Machine Specific Processor Extension Availability Field.         |
| 144 | fcode           | Supervisor   | Set when certain exceptions are triggers |
| 145 | pfcharp | Supervisor       | Set to the virtual address to store information about the page fault to |
| 146 | cr10          | Reserved       |                                                                         |
| 147 | cr11          | Reserved       |                                                                         |
| 148 | msr0          | Supervisor[^3] |                  Machine Specific Control Register 0.                   |
| 149 | msr1          | Supervisor[^3] |                  Machine Specific Control Register 1.                   |
| 150 | msr2          | Supervisor[^3] |                  Machine Specific Control Register 2.                   |
| 151 | msr3          | Supervisor[^3] |                  Machine Specific Control Register 3.                   |
| 152 | msr4          | Supervisor[^3] |                  Machine Specific Control Register 4.                   |
| 153 | msr5          | Supervisor[^3] |                  Machine Specific Control Register 5.                   |
| 154 | msr6          | Supervisor[^3] |                  Machine Specific Control Register 6.                   |
| 255 | undefined     | Reserved[^4]   |  Always reserved, and will not be given meaning in a future revision.   |

All unmentioned register numbers are reserved and have no name

Read Only Registers cannot be written to, except by certain instructions. Any instruction that violates this constraint triggers UND.

The Flags register may be written to, but writes only affect certain bits, and may differ between execution modes.

Reserved Registers cannot be accessed (undefined). Any instruction that accesses a reserved register triggers UND. 

Supervisor registers cannot be accessed in Program Execution Mode. Any instruction that accesses a Supervisor register in Program Execution Mode triggers PROT.

CPUInfo registers cannot be written to; violating this constraint triggers UND. CPUInfo registers cannot be read in Program Execution Mode unless the corresponding bit in ciread is set; violating this constraint triggers PROT. 


flags Register bitfield:

|bit | Name (id)  | Notes
|----|------------|---------------------
|0   | Carry (C\)  | Set by arithmetic operations that cause unsigned overflow
|1   | Zero  (Z)  | Set by operations that have a zero result
|2   | Overflow (V)| Set by arithmetic operations that cause signed overflow
|3   | Negative (N)| Set by operations that have a negative result
|4   | Parity (P\) | Set by operations that have a result with Odd Parity.


Any attempt to write to an unmentioned bit shall be ignored. 
Many instructions will set some of or all of bits 0-4 in flags, based on a result. 
The instructions will mention which flags are affected in a *Flags* section of the instruction definition.
The flags are set as follows:
* flags.C is set if an arithmetic operation overflows the unsigned operand for the destination operand size, and cleared otherwise. 
* flags.Z is set if an operation loads or stores zero, or an arithmetic or logic operand's result is zero, and cleared otherwise.
* flags.V is set if the arithmetic operation overflows the signed operand (that is, changes sign without a Carry) for the destination operand size, and cleared otherwise. 
* flags.N is set if the operation loads or stores a negative number (most significant bit set), or an arithmetic or logic operand's result is a negative signed number, and cleared otherwise.
* flags.P is set if the operation loads a value with a odd parity (odd number of 1 bits set in the value), or an arithetmic or logic operand's result is such a value, and cleared otherwise.

Processor Mode (mode) bitfield:

| bit | Name (id)                |                              Notes                               |
| --- | ------------------------ |:----------------------------------------------------------------:|
| 32  | Execution Mode (XM)      | Whether the program is in supervisor (0) or program (1) mode |
| 33  | Execution Mode Mirror (XMM) | Set to a copy of XM                                          |

`mode` can only be written by the following instructions:
* `jsm` (opcode 0x7ca and 0x7da)
* `callsm` (opcode 0x7cb and 0x7db)
* `retrsm` (opcode 0x7cc)
* `syscall` (opcode 0x7c4)
* `int`     (opcode 0x7c5)
- `scret`   (opcode 0xfc6)
- `iret`    (opcode 0xfc7)

Other writes trigger UND (except for `rstar`, `popar`, and `rstregf`). With the exception of `syscall` and `int`, the upper 32-bits of `mode` are reserved for the supervisor, any attempt to write to them with `mode.XM=1` is ignored.


Processor Control (cr0) bitfield

| bit | Name (id)                |                              Notes                               |
| --- | ------------------------ |:----------------------------------------------------------------:|
| 0   | Paging (PG)              | Memory Accesses are performed in virtual memory described by cr1 |
| 1   | Interrupt Enable (IE)    |             Enable External and software interrupts              |
| 3-5 | Page Table Nesting (PTL) |        The size of virtual memory accesses when cr0.PG=1.        |
 

All unmentioned flags are reserved. Attempting to set any such flag causes an exception


CPU Extension Availability flags (cpuex2)

| bit |          Name (id)          |                 Notes                  |
| --- |:---------------------------:|:--------------------------------------:|
| 0   |         Is NAME (V)         |            Always set to 1.            |
| 2-4 | Physical Address Size (PAS) | The size of system physical addresses. |
| 5-7 | Virtual Address Size (VAS)  |     The size of system addresses.      |

Virtual Address Size, as determined by cpuex2.VAS and cr0.PTL is assigned as follows: 0=32-bit, 1=40-bit, 2=48-bit, 3=56-bit, 4=64-bit.

Physical Address Size, as determined by cpuex2.PAS is assigned as follows:  0=32-bit, 1=36-bit, 2=40-bit, 3=44-bit, 4=48-bit, 5=52-bit, 6=56-bit, 7=60-bit.



## Operands

Operands are encoded in clever as 2 bytes, known as the operand control structure, in big endian (Most-significant Byte at the lowest address), using the 2 most significant bits as a control indicator. The definitions of each encoded form are defined below. 
Not all bits of encoded operands are used. Any unused/reserved bits must be set to 0 or an undefined instruction exception is raised. These bits may be given meaning in future versions or extensions.

Instructions that have multiple operands can have operand size differ between two operands. Each source operand is zero extended to the largest operand size, and the result is truncated to the destination operand size, if any, before being stored. 

### Register Operand

`[00 yyyy ss rrrrrrrr]`

- ss is the size control value (log2(size) bytes).
- yyyy is reserved and must be zero
- rrrrrr is register value 0<=r<64

When reading from a register operand, only the low order bytes indicated by `ss` are read. When writing to a register operand, the value is written only to the low order bytes indicated by `ss`, and all other bytes are zeroed. 

### Indirect 

`[01 oooo lll k ss rrrr]`

ss, and rrrr have the same meanings as in a Register value. Only General registers may be accessed by indirect addressing modes.

If `k` is set, then `oooo` is the offset to add to the value in `rrrr`, after scaling by the scaling factor in `lll`=log2(scale). 
If `k` is clear, then `oooo` is the general purpose register that contains the of the offset, which is added to the value in `rrrr` after scaling by the scaling factor in `lll`=log2(scale). 


### Short Imm

`[10 y r iiiiiiiiiiii]`

An up to 12-bit immediate value can encoded directly into the operand. If `r` is set, then the value of `ip` is added to the signed immediate value.

The size of a Short immediate operand is 12-bit, not 16-bit. When using the `r` bit, or the movsx instruction, the 12-bit is treated as the sign bit for extension to the destination size.

### Long Imm 

`[11 m yy r ss yy zz yyyy]`

An Immediate value, with a size given by ss (`log2(size)-1`).  If `r` is set, then the value of `ip` is added to the signed immediate value.

`yy` and `yyyyyyyy` are all reserved and must be zero. The immediate value follows the operand control structure.

If `m` is set, the the immediate value is a memory reference, and the operand accesses the memory address given by the immediate value. `zz` is the size control (`log2(size)`) for the referent. If `m` is not set, `zz` is reserved and shall be zero.

Immediate values of size 16 (ss=3) are reserved, and attempts to use them in an operand will unconditionally produce `UND`.

Long Immediate Values and values in memory are encoded in the Little-Endian byte Order (The least significant byte occurs at the lowest address). 


### Hetrogenous Operand Sizes and destination operands

Certain instructions will store a result in an operand. These instructions refer to this operand as the "destination". In the case of multi-operand instructions, if any, the destination operand is the first operand. A destination operand must be "Writable" (either a register, an indirect register operand, or a memory reference). Some instructions may use the value of a destination operand.


When using multi-operand instruction, the operands may have different sizes. 
Each operand used as a source is loaded according to it's size. 
Then each operand is zero extended to the largest operand size, if it is not already that size; Immediates (other than memory references) with the `r` bit set will instead be sign-extended to this size, then the value of `ip` is truncated and added to the value instead of being zero extended. 
The operation described by the instruction is then performed on each operand, and the result is truncated or zero-extended to the destination operand size.

Certain instructions, such as movsx, have special behaviour with heterogenous operand sizes, and this behaviour overrides the behaviour described here.


Address computations always occur in 64-bits, regardless of the size of the incoming values (a 16-bit immediate address is still used as a 64-bit value). This is significant for the lea instruction, where the source is the address of an indirect register or memory reference operand, rather than the memory value accessible from that address.

This also applies to immediates using ip-relative encoding: the size of the operand is 64-bit always, and the value is computed in 64-bit.

## Instruction Encoding

Instruction opcodes are encoded as 2 bytes, in big endian byte order. The most significant 12 bits of the instruction determine the opcode, and the least significant 4 bits encode instruction specific information, referred to in this document as `h`. Not all instructions make use of the `h` bits. 
If an instruction does not use a particular bit in the `h` field, it must be set to zero or an undefined instruction exception occurs. 
Such bits may be used in future revisions of this document, or extensions. 

Opcode:
`[oooooooooooo hhhh]`: `o` is the 12-bit opcode, and `hhhh` is used for instruction specific information.

## Memory Model

### Definitions

Memory Operations: An operation (instruction or machine action) that accesses (reads or writes) one or more bytes in memory, or a locked read-modify-write instruction performed on one or more bytes in memory.

Byte: A discrete, 8-bit, region in Physical Memory at a specific address.

Memory Location: A region of bytes accessed by a Memory Operation, with some size

Execution Order on a CPU: The order in which each instruction is dispatched in the CPU, which is linear from the instruction stream.

Store List: A total order List of all store operations that modify a Byte in memory.

Global Synchronization Order: A total order List of all global synchronization operations executed accross all CPUs.

### Stores and Store List on memory locations

Each byte of memory has a total order list, called the Store List, of all stores to that memory location. Each store that modifies a particular memory region is placed in this order. Stores that occur on a single CPU are placed in this list in execution order on that CPU. Stores that occur across multiple CPUs are placed on this list in a nondeterministic order, subject to the rules in this specification.

Each complete memory operation that stores to a memory location is indivisible. If two memory operations, a and b, both store to multiple of the same bytes in memory, then `a` and `b` will be placed in the Store List of all of those bytes in the same order relative to each other. This is true even if `a` and `b` do not access all of the same bytes. 

A memory operation is visible to each other store operation that is executed after it on the same CPU.


### Loads and Store-Load/Load-Load Coherence

Each memory operation that loads from a byte takes the value from some store to that byte in the Store List of that byte. If a memory operation loads from multiple bytes, the value loaded from each byte is taken from the Store List of each of those bytes as follows:
1. If any byte value is taken from a particular store, then each other byte that the load access which that store is also part of the Store List for will take the value from either that store, or a store later in the store list of that byte. 
- _Note: This rule is sufficient to prevent tearing when mixed size or partially overlapping accesses to the same memory location are not used, and to limit tearing to precisely the boundaries of a partially-overlapping memory access when they are._
3. If a store is visible to a load which accesses any byte which the store is part of the Store List for, then the value for that byte is taken from that store, or any store later in the store list for that byte.
4. If there are two loads, `a` and `b`, such that `a` is visible to `b`, and `a` and `b` load from any of the same bytes, then for each of those bytes, if `c` was the store which `a` took the value of that byte from, then `b` takes the value of that byte from `a` or a store later in the store list for that byte.

Each store that is in the Store List of a particular byte is visible to each load executed after it on that CPU, which accesses that byte of memory.

Each load that access from a particular byte is visible to each other load executed after it on that CPU that access that byte.

### Store-Store and Load-Store Coherence

If there are two stores, `a` and `b`, such that `a` is visible to `b`, then for each byte that both `a` and `b` accesses, `b` is placed in the Store List of that byte after `a`.

If there is a load `a` and a store `b`, such that `a` is visible to `b`, then for each byte accessed by `a`, the value taken for that byte is one stored by a store which preceeds `b` in the Store List of that byte.

The order which memory operations becomes visible to all other memory operations shall be acyclical. That is, given three memory operations, `a`, `b`, and `c`, such that `a` is visible to `b`, and `b` is visible to `c`, `c` shall not be visible to `a`. 

### Locked read-modify-writes

Certain instructions that both read and write memory can be "locked" (or are by default). These operations perform a locked read-modify-write operation on each byte.

The store from a locked read-modify-write to each byte accessed appears immediately after the store which the load from that operation takes the value of that byte from in the Store List of that byte.

A locked read-modify-write constitutes a single memory operation, rather than a distinct memory operation for the store and the load.

The following instructions perform a locked-read-modify-write:
* The `add`, `sub`, `and`, `or`, and `xor` ALU instructions, except for the GPR specializations of those instructions, with the `l` bit set in the `h` field of the instruction. 
* The `xchg` instruction.
* The `cmpxchg` and `wcmpxchg` instructions (including if the instruction failed).


### Store-Load Synchronization

When a load operation `a`, accesses a byte, and takes the value for that byte from a store, `b`, then each memory operation visible to `b` becomes visible to `a` and all memory operations executed after `a` on the same CPU.

If a load operation `a` is part of a locked read-modify-write, then any memory operation visible to `a` is visible to the store part of the same read-modify-write.

### Synchronization Operations and Global Synchronization Order

A memory operation `A` is *synchronization ordered before* another memory operation `B` if:
1. `A` is visible to `B`,
2. `A` and `B` are both stores that store to any of the same bytes, and `A` preceeds `B` in the Store List of all of those bytes,
3. `B` is a load and `A` is a store that accesses any of the same bytes, and `B` take the value of any of those bytes from `A`,
4. There exists a memory operation `C`, such that `A` is *synchronization ordered before* `C`, and `C` is *synchronization ordered before* `B`.

Certain operations, known as synchronization operations, participate in a global total order, known as the Global Synchronization Order. This order is constrained as follows, given two synchronization operations `Sa` and `Sb`:
1. If `Sb` is executed after `Sa` on the same thread of execution, then `Sa` preceeds `Sb` in the global synchronization order
2. If a memory operation `Ma` is executed after `Sa` on the same CPU, and there exists a memory operation `Mb`, such that `Mb` is an eligible memory operation with respect to `Sb`, and `Ma` is *synchronization ordered before* `Mb`, then `Sa` preceeds `Sb` in the global synchronization order


Certain operations that participate the Global Synchronization Order may make certain memory operations visible accross CPUs. For a given synchronization operation `S`, a memory operation `A` is considered an "eligible memory operation" with respect to `S` if:
1. `S` is executed after `A` on the same CPU, or
2. There exists a memory operation `B`, such that `B` is an eligible memory operation with respect to `S` and `A` is visible to `B`.

If a memory operation is an eligible memory operation with respect to some synchronization operation, it may be a "Visibility Candidate" for that synchronization operation. Subsequent synchronization operations in the Global Synchronization Order may make Visibility Candidates of preceeding ones visible to memory operations that execute after it on the same CPU.


### Execution and Instruction Visibility

Each Instruction fetch loads two bytes, aligned to two bytes, from memory on a given CPU, as follows in this section. _Note: The fetching of an entire instruction may require more than one instruction fetch, each act as though they were independent fetches for the purpose of the memory model_


 Let `B0` designate the byte fetched from the low order (even numbered) address, and `B1` designate the byte fetched from the high order (odd numbered) address. Let `S0` be the latest store in the Store List of `B0` which is Execution Visible to the current instruction fetch, and let `S1` be the latest store in the Store List of `B1` which is Execution Visible to the current instruction fetch. The value taken for the instruction fetch is taken from stores `K0` to `B0` and `K1` to `B1`, where `K0` and `K1` are determined as follows:
1. `K0` is either `S0` or a store which follows `S0` in the Store List of `B0`. `K1` is either `S1` or a store which follows `S1` in the Store List of `B0`.
- _Note: This rule ensures that each forcing control transfer synchronizes the instruction stream with each visible store to the instruction stream_
2. If `K0` occurred on the same CPU as the instruction fetch, then `K0` shall have preceeded the instruction which is being fetched currently on that CPU, and if `K0` is in the Store List of `B1`, then `K1` is either `K0` or a store which follows `K0` in the Store List of `B1`. 
3. If `K1` occurred on the same CPU as the instruction fetch, then `K1` shall have preceeded the instruction which is being fetched currently on that CPU, and if `K1` is in the Store List of `B0`, then `K0` is either `K1` or a store which follows `K1` in the Store List of `B1`.
- _Note: This rule ensures that instruction fetches can tear stores on the same CPU at most every two bytes, and that instruction fetches are not circular to stores that are executed as a result of the fetched instructions_


A Forcing Control Transfer Operation has the same visibility effects as a store to some unspecified memory location which is not accessed by any other memory operation, except for a Forcing Control Transfer Operation executed on the same CPU (all such Forcing Control Transfer Operations access this memory location). The visibility of memory operations to a Forcing Control Transfer Operation, and the visibility of the Forcing Control Transfer Operation, is independent of the visibility of any memory operation performed at the same time. Any memory operation which is visible to the Forcing Control Transfer Operation is made Execution Visible to all subsequent instruction fetches performed on the same CPU in execution order.

_Note: For example, though the `retrsm` instruction both loads from memory (popping the return address and return mode value from the stack) and performs a Forcing Control Transfer, memory operations visible to the Forcing Control Transfer are not necessarily made visible to the load, nor are memory operations visible to the load necessarily made visible to the Forcing Control Transfer_

The privileged `iflush` and `flall` instructions additionally make memory accesses Execution Visible to subsequent instruction fetches.

## Instruction Descriptions

Instructions and Instruction Groups are described by specifying the following information, with the given prefixes:
- Opcode (or Opcodes): The Opcode or Opcodes in the Group
- Operands: The number of Operands of each Instruction in the Group
- Operand Constraints: Semantic constraints placed on each operand for each Instruction in the group
- h field: The H Field of each instruction in the group
- Exceptions: The Exceptions, if any, which may be generated by the Instructions in the Group
- Flags: Modifications to the `flags` register, if any
- Memory Effects: The Effects, if any, the instruction has on the Memory Model
- Instructions: The list of each instruction in the group (with a suggested mnemonic for assemblers), and the behaviour of each of those instructions.

Followed by any notes, common behaviour, or special handling indicators that don't belong in the previous section.

## Generated Exceptions

Each Instruction executed may, instead of performing any behaviour specified, generate an [Exception](#exceptions).

Each Instruction, in its description, lists all of the Exceptions that can be raised as part of executing the Instruction. 
Additionally, certain other Exceptions occur based on the decoding of the instruction. This includes Decoding Exceptions (UND or PROT), and Page Faults generated by instruction fetches.

If an exception occurs, no register or memory is modified by the instruction execution, no branch occurs as a result of the instruction execution. Additionally, unless the exception is denoted by a `*` following its name, no memory effects are performed by the instruction (Such exceptions are used when the value of the operand must be determined to raise the exception). Such denoted exceptions are referred to as "After-loads Exceptions" or "Late Execution Exceptions".

When an instruction generates an exception is not considered stable (Except for `UND` issued by opcodes 0x000 and 0xFFF), and an exetension may change what exceptions are generated by instructions, or cause an instruction that would previously generate an exception to have some specified behaviour. In the case of instructions accessible to user programs (Opcodes 0x000 through 0x7FF and 0xFFF), the supervisor will be required to opt-in to the change by performing some action (typically setting a unique bit in `cr0`) that would have generated an exception (or had undefined behaviour) without that extension. 

### Fetch Paging Faults

When any part of the instruction is fetched from memory, the address of the fetch is validated as a memory operand for read access (as though by [memory operand validation](#memory-operand-validation-execution)) and paging faults are thrown, as though the access has size 1 and the least significant byte of the fetch is referenced by the memory operand, with the following exceptions:
* If the lpe permission field is `READ` or `WRITE`, `PF` is raised
* If the lpe entry or any npe entry has `SXP=1`, and `mode.XM=0`, `PF` is raised

### Illegal/Undefined Instructions (Decoding)

In certain cases, an instruction fetched from the stream is considered illegal in decoding. This is the case if the instruction is not defined, or if any reserved bit in the h field is set to `1`. 
These checks are performed before any operands are fetched (thus, `PF` will not occur if the operands reside on a different page). 
Instructions that cause a UND exception (and no other exceptions) based on h bits *may* also have this check performed during decoding, prior to any operands being fetched. 
Additionally, privileged instructions that cause a `PROT` exception when executed with `mode.XM=1`, *may* have this check performed during instruction decoding, prior to any operands being fetched. In this case, if any instruction decoding check performed would raise `UND`, and `PROT` would also be generated, then the `UND` exception is raised rather than `PROT`.


### Operand Validation (Decoding)

Each operand is validated as it is fetched. 
An invalid operand control structure generates a `UND` exception after being read. No further operands are fetched, and page faults for these operands are not generated. This includes validating the immediate size specifier of immediate operands (but not the memory size specifier of memory operands)

After all operands are fetched, the following validation steps are performed:
* UND is raised if any register operand refers to a reserved or undefined register,
* PROT is raised if any register operand refers to a supervisor register and `mode.XM=1`. 
* UND is raised if any operand has an invalid size (including memory size for immediate operands)

### Memory Operand Validation (Execution)

Any Memory Operand that is accessed is validated according to paging rules. Any Memory Operand that is to be written to is validated for read/write access, and any memory operand that is to be read from is validated for read access. The validation occurs for the size of the memory operand. If an implicit access size is specified by the instruction, then that size is used instead, and the size of the memory operand is ignored. PFs are generated for validation failures. Some instructions that have memory operands do not perform any access to the memory. These memory operands are not validated.

When Paging is disabled (`cr0.PG=0`), when validating memory accesses to nested or lower page tables, or when validating the resolved physical address of a virtual address:
- Each memory operand is validated to the range specified by the value of `cpuex2.PAS`. Any memory address accessed is out of range, PF is generated.
- If any memory operand refers to non-allocatable physical address, PF is generated

When Paging is enabled (`cr0.PG=1`):
- The ptbl register (`cr1`) is [validated](#page-table-format)
- Each memory operand is validated as a canonical memory address, according to the value of `cr1.PTL`. The number of in-use virtual address bits is specified by `PTL`, and all more significant bits must be a copy of the most significant in-use virtual address bit. If the any address of the memory operand is not a canonical memory address, or any access to a memory address with a different most significant in-use virtual address bit then the address of the memory operand, `PF` is generated
- For each byte accessed by the memory operand, Each nested level of the page table is indirected using the offset from the appropriate in-use bits of the virtual address, and the relevant entry is [validated](#page-table-format). If the level is not present, or has the `XM` bit clear while (`mode.XM=1`), `PF` is generated. The memory access to each nested page table is validated 
- For each byte accessed by the memory operand, The lowest level page table is indirected using the last 9 bits of the address before the page offset (least significant 12 bits of the address). If the lpe permission field is ABSENT, then `PF` is generated. If the access to that memory operand is a write access, and the lpe permission field is `READ` or `EXEC`, then `PF` is generated.
- The resolved physical address (the page physical address for the byte, offset by the page offset in the address) is then validated


All Memory Operand Validation occurs before any Memory Operand is accessed - Memory Operand Validation is not performed as Late 

### Operand Writeback Validation (Late Execution)

In addition to the requirements of the instruction, prior to any register operand being modified, writeback validation is performed for all register operands that will be modified. Writeback validation only generates a `UND` exception.

A `UND` exception occurs in the following cases:
* A read-only register (`ip`, `mode`, CPUInfo registers, or implementation-defined machine-specific control registers) are refered to by a register operand that is to be modified
* A control register (`cr0`, `cr2`, `cr5`, `cr7`, or implementation-defined machine-specific control registers) are referred to by a register operand that is to be modified, and a reserved or undefined bit would be set to 1 by the writeback.
* Implementation-defined machine-specific control registers would be modified to have a state deemed invalid by an implementation-defined validation check.


### Exception Ordering

Exceptions are ordered in the following steps. If two exceptions are generated in the same steps, the order is unspecified. This order may be refined by future extensions:
1. Instruction Fetch (Opcode bytes only) PF 
2. Instruction Decoding Exceptions (UND only)
3. Instruction Decoding Exceptions (PROT only), if any
4. For each operand
    1. Operand fetch PFs
    2. Operand Control Structure validation Exceptions
5. Operand Validation Exceptions (after fetch)
6. Execution (Specified) Exceptions (Except Late Execution Exceptions), Paging Faults
7. Late Execution Exceptions and Operand Writeback Validation Exceptions

If no memory effects (other than stores) would occur for a particular exception, then Late Execution Exceptions (Step 7 Exceptions) are not treated specially and occur in step 6.


## Program Instructions

The following instructions are available in both Supervisor Mode and Program Execution Mode

### Undefined Instructions

Opcodes: 0x000 and 0xfff

Operands: None

h: Unused

Exceptions:
- UND, unconditionally

Opcodes 0x000 and 0xfff are reserved and always generate an undefined instruction exception. 
Tools generating code for this architecture can rely on these opcodes not being defined.

(Note: This means that instruction streams consisting of all 0 bytes or all 0xff bytes will cause an exception)

### Arithmetic/Logic Instructions 

Opcodes: 0x001-0x005.
Operands: 2

h: `[l0 0f]`. If `f` is set, then `flags` is not modified. If `l` is set, the memory operation is totally atomic.


Operand Constraints: At least one operand shall be an immediate value other than a memory reference, or shall be a gpr. The first operand shall be a register, indirect register, or memory reference. If l is set in `h`, then the first operand shall be an indirect register or a memory reference.

Exceptions: 
- UND, if any operand constraint is violated. 
- UND, if a reserved register is accessed by the instruction
- PROT, if a Supervisor register is accessed, and the program is not in Program Execution Mode
- UND, if the destination operand is the flags or ip register.
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if the first operand is a memory operand, and page protections are violated by the access
- PF, if paging is disable, and a memory operand accesses an out of range physical address
- PROT, if a memory operand is out of range for the PTL mode.

Memory Effects:
- Load from operand 2.
- If `l` is not set in `h`, a load and a store to operand 1.
- If `l` is set in `h`, a locked read-modify-write to operand 1.

Flags:  Sets C, M, V, and Z according to the result of the operation, unless the `f` bit is set in `h`

Instructions:
- 0x001 (add): Adds the value of the two operands
- 0x002 (sub): Subtracts the value of the second operand from the first
- 0x003 (and): Performs the bitwise and of the two operands
- 0x004 (or):  Performs the bitwise or of the two operands
- 0x005 (xor): Performs the bitwise exclusive or of the two operands

### Multiplication and Division Instructions

Opcodes: 0x006-0x007, 0x040, 0x048
Operands: None

h: `[ss wf]`, where `ss` is log2(size) of the operation, as though for a register operand size control. `w` shall be set only for opcode 0x007 and 0x048. If `f` is set, then `flags` is not modified. 

Flags: Sets C, M, V, and Z according to the result of the operation if the `f` bit is not set in `h`.

Exceptions:
- UND: If w is set in h for opcodes 0x006 or 0x040
- DIV: For opcodes 0x007 and 0x048, if r3 is 0

Instructions:
- 0x006 (mul): Multiplies r0 and r3, storing the low order bits in r0, and the high order bits in r3.
- 0x007 (div): Divides r0 by r3, storing the quotient in r0 and the remainder in r3. If w is set in h, then the dividend has double the size specified by ss, and the high order bits are present in r1.
- 0x040 (imul): Performs signed multiplication of r0 and r3, storing the low order bits in r0 and the high order bits in r3
- 0x048 (idiv): Performs signed division between r0 and r3, storing the quotient in r0 and the remainder in r3. If w is set in h, then the divend has double the size specified by ss, and  the high order bits are present in r1.

### Register Manipulation Instructions

Opcodes 0x008-0x00b
Operands: 2 for opcodes 0x008 and 0x009, 1 for opcodes 0x00a and 0x00b.

h: Opcodes 0x00a and 0x00b only, the general purpose register for the destination (0x00a) or source (0x00b) operand. Otherwise, reserved and shall be zero

Operand Constraints: For opcodes 0x008 and 0x009, the first operand shall be a memory reference or register, and at least one operand shall be a direct register. For Opcode 0x009, the second operand shall be either an indirect register, or a memory reference. 

Exceptions: 
- UND, if any operand constraint is violated. 
- UND, if a reserved register is accessed by the instruction
- PROT, if a Supervisor register is accessed, and the program is not in Program Execution Mode
- UND, if the destination operand is the flags or ip register.
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if the first operand is a memory operand, and page protections are violated by the access
- PF, if paging is disable, and a memory operand accesses an out of range physical address
- PROT, if a memory operand is out of range for the PTL mode.

Flags: Except opcode 0x009, Sets M and Z according to the result of the operation.

Memory Effects:
- Opcodes 0x008. 0x009, and 0x00B, store to operand 1
- Opcodes 0x008, load from operand 2
- Opcode 0x00A, load from operand 1

Instructions:
- 0x008 (mov): Moves the value of the second operand into the first
- 0x009 (lea): Moves the computed address of the second operand into the first, without setting flags
- 0x00a (mov*r*): Short form if the destination operand is a gpr (0<=r<16). Moves the operand into *r*, according to the size of the operand
- 0x00b (mov*r*): Short form if the source operand is a gpr (0<=r<16). Moves *r* into the operand, according to the size of the operand.
- 0x00c (lea*r*): Short form of lea if the destination operand is a gpr.

If the destination operand of `mov` or `lea` is smaller than the result, the value stored is truncated. If the destination operand is larger than the result, the value stored is zero-extended to the destination size.
All memory accesses are performed atomically, wrt. other memory accesses, and operations performed under a memory lock. 

For `lea`, the size of the source operand is treated as 64-bit always - the address of the operand is calculated in 64-bits before being truncated to the destination operand size (if necessary). For memory references and indirect registers, the size of the reference is ignored.


### No Operation

Opcodes: 0x010-0x013

Operands: Opcode 0x010, 0. Opcode 0x011, 1. Opcode 0x012, 2. Opcode 0x013, 3.

h: Unused

Exceptions: None.

Operations:
- 0x010 (nop): Performs no operation.
- 0x011-0x013 (nop): Performs no operation further than decoding and validating each operand.

Machines should ensure consistent timing for each no-op instruction, but this timing is not specified. 
Normal exceptions for instruction and opcode decoding occurs, but no checks are performed on operands. In particular, a nop instruction cannot contain a malformed operand, but could, for example, reference a supervisor register in program execution mode, reference an undefined or reserved register, or refer to unmapped memory via a memory reference or indirect register operand. No memory access is performed by the instruction, and no registers are modified by the instruction.

### Stack Manipulation

Opcodes: 0x014-0x017
Operands: Opcodes 0x014 and 0x015, 1. Opcodes 0x016 and 0x017, 0.

h: For opcodes 0x016 and 0x017, `[rrrr]` where r is the gpr (0<=r<16) to operate on. 

Operand Constraints: For opcode 0x015, operand shall be a register, indirect register, or memory reference with size 8.

Exceptions:
- UND, if a operand constraint is violated
- UND, if the operand of opcode 0x015 is ip
- PROT, if a supervisor register is an operand, and `flags.XM=1`.
- UND, if a reserved register is an operand.
- PF, if the target address is an unavailable virtual memory address
- PF, if page protections are violated by the access
- PF, if paging is disable, and the target address is an out of range physical address
- PROT, if the target address is out of range for the PTL mode.
- PF, if a memory operand accesses an unavailable virtual memory address

Memory Effects:
- Opcode 0x014, load from operand 1
- Opcodes 0x014 and 0x016, store to `double [r7-8]`
- Opcode 0x015, store to operand 1
- Opcode 0x015 and 0x017, load from `double [r7]` 

Instructions:
- 0x014 (push): Decrements sp by 8, then stores the (potentially zero extended) 8-byte value in the operand to the address indicated by sp
- 0x015 (pop): Loads the 8-byte value from the address indicated by sp, and stores it in the operand, then increments sp by 8
- 0x016 (push*r*): Same as 0x014, but operates on the register indicated by `h`
- 0x017 (pop*r*): Same as 0x015, but operates on the register indicated by `h`

If the operand (opcodes 0x014 and 0x015) or the implicit register (opcodes 0x016 and 0x017) mention `sp`, the value of the register before the operation is used for the operand. All memory operations are atomic.


### Bulk Register Storage

Opcodes: 0x018-0x01f
Operands: For opcodes 0x018-0x01b, 1. For opcodes 0x01c-0x01f, None.

h: Shall be 0.

Operand Constraints: For 0x018-0x01b, the operand shall be an indirect register or a memory reference.

Exceptions:
- UND, if a operand constraint is violated
- UND, if the operand of opcode 0x015 is ip
- PROT, if a supervisor register is an operand, and `mode.XM=1`.
- UND, if a reserved register is an operand.
- PF, if the target address is an unavailable virtual memory address
- PF, if page protections are violated by the access
- PF, if paging is disable, and the target address is an out of range physical address
- PROT, if the target address is out of range for the PTL mode.
- PF, if a memory operand accesses an unavailable virtual memory address
- PROT*, If opcodes 0x01b or 0x01f are used to load a value other than 0 into a reserved or unavailable register.

Memory Effects:
- Opcodes 0x018 and 0x01C, store to `128 [#1]` or `128 [r7-128]`
- Opcodes 0x019 and 0x01D, 8 stores to `128 [#1+128*N]` or `128 [r7-1024+128*N]` where `N∈[0,8)`. The order of the stores relative to each other is unspecified.
- Opcodes 0x01A and 0x01E, load from `128 [#1]` or `128 [r7]`
- Opcodes 0x01B and 0x01F, 8 loads from `128 [#1+128*N]` or `128 [r7+128*N]` where `N∈[0,8)`. The order of the loads relative to each other is unspecified.

Instructions:
- 0x018 (stogpr): Stores the value of each general purpose register (0<=r<16) to the memory operand.
- 0x019 (stoar): Stores the value of each program register (0<=r<128) to the memory operand. Any reserved or unavailable floating-point register stores 0 instead.
- 0x01a (rstogpr): Loads the value of each general purpose register (0<=r<16) from the memory operand. 
- 0x01b (rstoar): Loads the value of each program register (0<=r<128), other than `ip`, from the memory operand. The value of any reserved or protected bit in `flags` is ignored in the memory region.
- 0x01c (pushgpr): Pushes the value of each general purpose register (0<=r<16) to the stack. Lower memory addresses (closer to the stack head) store lower numbered registers.
- 0x01d (pushar): Pushes the value of each program register (0<=r128) to the memory operand. Any reserved or unavailable register stores 0 instead. Lower memory addresses (closer to the stack head) store lower numbered registers.
- 0x01e (popgpr): Pops the value of each general purpose register from the stack.
- 0x01f (popar): Pops the value of each program register (0<=r<128), other than `ip`, from the stack. The value of any reserved or protected bit in `flags` is ignored in the memory region.

If the operand to `ldgpr` or `ldar` is an indirect register, the address to load each register from is determined prior to performing the operation. `pushgpr` and `pushar` update the value of the stack pointer after it is stored, and `popgpr` and `popar` do not update the value of the stack pointer after restoring it. 


### Converting Moves

Opcodes: 0x020-0x021
Operands: 2

h: `[00 0f]`where if `f` is set `flags` is not modified.

Operand Constraints: For opcode 0x020 and 0x021, at least one operand shall be a register. For operand 0x021, both operands must be the same size. 

Flags: `M` and `Z` are set according to the result, unless `f` is set in `h`.

Exceptions:
- UND, if a operand constraint is violated
- UND, if any destination operand is flags or ip
- PROT, if a supervisor register is an operand, and `mode.XM=1`.
- UND, if a reserved register is an operand.
- UND, if a floating-point operand has size 1
- PF, if the target address is an unavailable virtual memory address
- PF, if page protections are violated by the access
- PF, if paging is disable, and the target address is an out of range physical address
- PROT, if the target address is out of range for the PTL mode.
- PF, if a memory operand accesses an unavailable virtual memory address

Memory Effects:
- Load from operand 2
- Store to operand 1

Instructions:
- 0x020 (movsx): Moves a signed integer operand from the second operand, to the first. If the second operand is smaller than the first, the highest bit is copied to each higher bit in the first operand.
- 0x021 (bswap): Moves the second operand into the first, swapping the order of the bytes stored. 

`movsx` modifies the behaviour of heterogenous operand sizes as follows:
- The value of the source operand is loaded
- If it is at least the size of the destination operand, it is truncated to the destination size as normal, 
- Otherwise, if is less than the size of the destination operand, the value is sign extended to the size of the destination operand. 


`bswap`, unlike other instructions, forbids heterogenous operand sizes, and both operands must be the same size.


### Integer Shifts

Opcodes: 0x030-0x036, 0x038-0x03f

Operands: For opcodes 0x030-0x036, 2. For opcodes 0x038-0x03e, 1.

h: For opcodes 0x030-0x037, `[l0 0f]` where if `f` is set, `flags` is not modified, and if `l` is set, the memory operation is performed under a memory lock and is atomic wrt. other memory accesses and memory operations performed under a lock. For opcodes 0x038-0x3f, `[rrrr]` where `r` is the number of the gpr (0<=r<16) of the first operand. 


Operand Constraints: At least one operand shall be an immediate value other than a memory reference, or gpr. The first operand shall be a register, indirect register, or memory reference. No operand shall be a floating-point register. If l is set in h, the first operand shall be an indirect register or a memory reference.

Exceptions: 
- UND, if any operand constraint is violated. 
- UND, if a reserved register is accessed by the instruction
- PROT, if a Supervisor register is accessed, and the program is in Program Execution Mode
- UND, if the destination operand is the flags or ip register.
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if the first operand is a memory operand, and page protections are violated by the access
- PF, if paging is disable, and a memory operand accesses an out of range physical address
- PROT, if a memory operand is out of range for the PTL mode.

Flags: Unless `f` is set in `h`, sets M, and Z according to the result of the operation. Sets C if the last bit shifted out was 1. Sets V if the shift quantity exceeds the width of the first operand. 

Memory Effects:
- Load from operand 2.
- Opcodes 0x030-0x37 only, If `l` is not set in `h`, a load and a store to operand 1.
- Opcodes 0x030-0x37 only, If `l` is set in `h`, a locked read-modify-write to operand 1.

Instructions:
- 0x030 (lsh): Left Shifts the first operand by the second, shifting in 0 bits
- 0x031 (rsh): Right Shifts the first operand by the second, shifting in 0 bits
- 0x032 (arsh): Right Shifts the first operand by the second, preserving and copying the most significant bit
- 0x033 (lshc): Left Shifts the first operand by the second, shifting in the value of the carry flag
- 0x034 (rshc): Right Shifts the first operand by the second, shifting in the value of the carry flag.
- 0x035 (lrot): Left Rotates the first operand by the second, shifting the bits that were shifted out into the low order bits. The second operand is taken modulo the width of the destination, in bits.
- 0x036 (rrot): Right Rotates the first operand by the second, shifting the bits that were shifted out into the high order bits. The second operand is taken modulo the width of the destination, in bits.
- 0x038 (lsh*r*): Specialization of lsh that operates on a gpr.
- 0x039 (rsh*r*): Specialization of rsh that operates on a gpr.
- 0x03a (arsh*r*): Specialization of arsh that operates on a gpr
- 0x03b (lshc*r*): Specialization of lshc that operates on a gpr
- 0x03c (rshc*r*): Specialization of rshc that operates on a gpr
- 0x03d (lrot*r*): Specialization of lrot that operates on a gpr. The second operand is taken modulo 64.
- 0x03e (rrot*r*): Specialization of rrot that operates on a gpr. The second operand is taken modulo 64.

Opcodes 0x038-0x03e uses 64-bits for the width of the instruction. 
For determining intermediate calculation width, the width of the second operand is ignored.

For opcodes 0x030-0x034 and 0x0380-0x03C, if the shift width exceeds the width of the first operand, the result is an undefined value, and the C flag is left in an undefined state.

### Unary Operations

Opcodes: 0x046-0x047, 0x4E-0x4F

Operands: For opcodes 0x040 and 0x041, 1. For opcodes 0x048 and 0x049, 0.

Operand Constraints: The operand must be a register or a memory reference. 

h: For opcodes 0x040 and 0x041, `[l0 0f]` where if `f` is set, `flags` is not modified, and if `l` is set, the memory operation is performed under a memory lock. For opcodes 0x042 and 0x043 `[rrrr]` where `r` is the number of the gpr `(0<=r<16)` of the operand.

Flags: Unless `f` is set in `h`, sets `M`, `Z`, and `P` according to the result of the operation.

Exceptions: 
- UND, if any operand constraint is violated. 
- UND, if a reserved register is accessed by the instruction
- PROT, if a Supervisor register is accessed, and the program is in Program Execution Mode
- UND, if the destination operand is the flags or ip register.
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if the first operand is a memory operand, and page protections are violated by the access
- PF, if paging is disable, and a memory operand accesses an out of range physical address
- PROT, if a memory operand is out of range for the PTL mode.

Memory Effects:
- Opcodes 0x046 and 0x047, if `l` is not set in `h`, a load followed by a store to operand 1
- Opcodes 0x046 and 0x047, if `l` is set in `h`, a locked read-modify-write to operand 1

Instructions:
- 0x046 (bnot): Performs a bitwise negation of the operand.
- 0x047 (neg): Negates the signed integer value in the operand.
- 0x04E (bnot*r*): Specialization of bnot that operands on a gpr.
- 0x04F (neg*r*): Specialization of neg that operands on a gpr.

### Arithmetic/Logic Instruction GPR Specializations

Opcodes: 0x041-0x045, 0x49-0x4D

Operands: 1

Operand Constraints: Opcodes 0x04A-0x04A, operand must be a register or a memory reference. 

h: `[rrrr]` where `r` is the number of the gpr `(0<=r<16)` of the destination (Opcodes 0x042-0x046) or source (Opcodes 0x04A-0x04E) operand.

Exceptions: 
- UND, if any operand constraint is violated. 
- UND, if a reserved register is accessed by the instruction
- PROT, if a Supervisor register is accessed, and the program is in Program Execution Mode
- UND, if the destination operand is the flags or ip register.
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if the first operand is a memory operand, and page protections are violated by the access
- PF, if paging is disable, and a memory operand accesses an out of range physical address
- PROT, if a memory operand is out of range for the PTL mode.

Memory Effects:
- A load from operand 1
- Opcodes 0x046-0x04D, a store to operand 1

Instructions: 
- 0x041 (add*r*): Specialization of opcode 0x001 (add) where the destination operand is a gpr
- 0x042 (sub*r*): Specialization of opcode 0x002 (sub) where the destination operand is a gpr
- 0x043 (and*r*): Specialization of opcode 0x003 (and) where the destination operand is a gpr
- 0x044 (or*r*):  Specialization of opcode 0x004 (or) where the destination operand is a gpr
- 0x045 (xor*r*): Specialization of opcode 0x005 (xor) where the destination operand is a gpr
- 0x049 (add*r*): Specialization of opcode 0x001 (add) where the second operand is a gpr
- 0x04A (sub*r*): Specialization of opcode 0x002 (sub) where the second operand is a gpr
- 0x04B (and*r*): Specialization of opcode 0x003 (and) where the second operand is a gpr
- 0x04C (or*r*):  Specialization of opcode 0x004 (or) where the second operand is a gpr
- 0x04D (xor*r*): Specialization of opcode 0x005 (xor) where the second operand is a gpr

### Conditional Moves

Opcodes: 0x068-0x069

Operands: For opcode 0x068, 3. For Opcode 0x069, 2.

h: `[cccc]` where c is a condition code

Operand Contraints: The first operand shall be a register or a memory reference and at least one of the first two operands must be a register or immediate value. The third operand must be a register.

Exceptions:
- UND, if any operand constraint is violated.
- UND, if any reserved register is operand.
- UND, if the destination operand is the mode or ip register.
- PROT, if any Supervisor register is an operand and mode.XM=0
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if a memory operand violates page protections
- PF, if paging is disabled, and a memory operand accesses an out of range physical address

Memory Effects:
- A load from operand 2
- Opcode 0x069, a load from operand 3
- If the condition is satisfied, a store to operand 1

Flags: none

Instructions:
- 0x068 (cmovt): Based on `flags` set in the third operand, stores the second operand in the first if the condition in `h` is satified.
- 0x069 (cmov): Specialization of `cmovt` where the third operand is implicitly the `flags` register.

For `cmovt` the expected layout of the third operand is the same as the `flags` register. Any bits other than the least significant 5 bits are ignored and should be set to zero

For the purposes of validating memory operands, both `cmov` and `cmovt` are treated as performing a write to the first operand. 

### Comparison Operations

Opcode: 0x06c-0x06f

Operands: Opcode 0x06c-0x06d, 2. Opcode 0x06e-0x06f, 1.

h: For Opcodes 0x06c-0x06d, shall be zero. For opcodes 0x06e-0x06f, `[rrrr]` where r is the gpr to use as the first operand (0<=r<16).

Operand Constraints: At least one operand shall be an immediate value other than a memory reference, or a register. The first operand shall be a register, indirect register, or memory reference. 

Exceptions: 
- UND, if any operand constraint is violated. 
- UND, if a reserved register is accessed by the instruction
- PROT, if a Supervisor register is accessed, and the program is in Program Execution Mode
- UND, if the destination operand is the flags or ip register.
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if the first operand is a memory operand, and page protections are violated by the access
- PF, if paging is disable, and a memory operand accesses an out of range physical address
- PROT, if a memory operand is out of range for the PTL mode.

Flags: Opcode 0x06c and 0x06e, sets Z, N, M, and V according to the result. Opcode 0x06d and 0x06f, sets Z and M according to the result.

Memory Effects:
- A load from operand 1
- Opcodes 0x06c and 0x06d, a load from operand 2

Instructions:
- 0x06c (cmp): Subtracts the second operand from the first, and sets flags based on the result. 
- 0x06d (test): Computes the bitwise and of the first and second operands and sets flags based on the result.
- 0x06e (cmp*r*): Same as cmp, but the first operand is the encoded general purpose register
- 0x06f (test*r*): Same as test, but the first operand is the encoded general purpose register.

### Atomic Operations

Opcodes: 0x200-0x203
Operands: Opcode 0x200: 2, Opcode 0x201-0x202: 3
h: Reserved and shall be zero

Operand Constraints: For Opcodes 0x200, at least one operand shall be a register and no operand may be an immediate value. For opcode 0x201 and 0x202, the neither the first nor second operand may be an immediate value, and at least one of the first and second shall be a register. 

Flags: For Opcode 0x201 and 0x202, `Z` is set to the result of the comparison. 

Memory Effects:
- A locked Read-modify write to operand 1
- A load from operand 2
- Opcode 0x200, a store to operand 2
- Opcodes 0x201 and 0x202, a load from operand 3
- Opcodes 0x201 and 0x202, a store to operand 2 if the comparison fails

Exceptions: 
- UND, if any operand constraint is violated. 
- UND, if a reserved register is accessed by the instruction
- PROT, if a Supervisor register is accessed, and the program is in Program Execution Mode
- UND, if the destination operand is the flags or ip register.
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if the first operand is a memory operand, and page protections are violated by the access
- PF, if paging is disable, and a memory operand accesses an out of range physical address
- PROT, if a memory operand is out of range for the PTL mode.

Instructions:
- 0x200 (xchg): Exchanges the value im the first operand with the second
- 0x201 (cmpxchg): Compares the value in the first operand with the value in the second and stores the third in the first if the comparison sets the Z flag, otherwise the value in the first operand is stored in the second
- 0x202 (wcmpxchg): Same as 0x201, but the comparison is permitted to spuriously fail

wcmpxchg exists to permit efficient implementations which can be made use of when reliable success is not necessary. 
Both cmpxchg and wcmpxchg act as though they always write to the destination.

For the purposes of validating memory operands, `cmpxchg` and `wcmpxchg` are always treated as performing a write access to the first and second operands.

### Memory Synchronization Fence

Opcode: 0x203
Operands: 0.

h: Reserved and shall be zero

Memory Effects:
- Inserts a `Fence` element to the Global Synchronization Order. 

Instructions:
- 0x203 (fence): Makes memory accesses visible accross CPUs according to the behaviour of the `Fence` synchronization operation. 

All eligible memory operations are Visibility Candidates for a given Fence operation. A Fence operation makes all Visibility Candidates from each preceeding synchronization operation in the Global Synchronization Order visible to all memory operations that follow it in the global synchronization order.

The `fence` instruction is analogous to the C++11 function `std::atomic_thread_fence`, called with `std::memory_order_seq_cst`.

### Branches

Branch Instruction Encoding:  
* `[0111 ss0r cccc wwww]`: Conditional Branches
* `[0111 110r oooo hhhh]`: Unconditional branches
* `[1111 110r oooo hhhh]`: Supervisor Branches

Each branch instruction can cause the following exceptions

Exceptions:
- XA*: If the branch is taken, and the destination address is not well-aligned
- PF*: If paging is enabled, and the destination address is on a non-executable page
- PF*: If paging is enabled, the destination address is on a page with supervisor execution protection, and `flags.XM=0`
- PF*: If paging is enabled, and the destination address is an out-of-range virtual address
- PF*: If paging is disabled, and the destination address is an out-of-range physical address

(Note: All branch exceptions are Late Execution Exceptions to support returns and indirect branches)

Taking a conditional branch transfers control to destination if the condition is satisfied, and otherwise execution continues from the following instruction. 
Immediately after the control transfer, `ip` will be equal to the desintation of the branch.
When a branch is taken, modifications to memory are guaranteed to be reflected in the code that control is transfered to (including modifications to the page table, or modification of `cr0.PG`). 
The destination of a branch must be aligned to a multiple of two bytes. 


#### Conditional branches

Encoding: `[0111 ss0r cccc wwww]`

`ss` is the offset size control value (`log2(size)-1`) of the immediate value following the instruction, that indicates the destination of the branch.
`r` is set if the destination is computed by adding `ip` to the signed immediate value following the instruction.
`cccc` is the condition code, which determines when the branch is taken. See [Condition Code Encoding](#Condition-Code-Encoding). 
`wwww` is the signed, 4-bit weight of the branch, which may be used by a branch predictor to determine the likelihood of the branch being taken, between -8 (least likely) and 7 (most likely).

##### Condition Code Encoding

cc value | Name       | Condition
---------|------------|---------------
 0       | Parity     | flags.P=1
 1       | Carry      | flags.C=1
 2       | Overflow   | flags.V=1
 3       | Zero/Eq    | flags.Z=1
 4       | Less Than  | flags.N!=flags.V
 5       | Less or Eq | flags.N!=flags.V or flags.Z=1
 6       | Below or Eq| flags.C=1 or flags.Z=1
 7       | Minus      | flags.N=1
 8       | Plus       | flags.N=0
 9       | Above      | flags.C=0 and flags.Z=0
 10      | Greater    | flags.N=flags.V and flags.Z=0
 11      | Greater/Eq | flags.N=flags.V
 12      | Not Zero/Eq| flags.Z=0
 13      | No Overflow| flags.V=0
 14      | No Carry   | flags.C=0
 15      | No Parity  | flags.P=0


#### Unconditional Branches

Encoding: `[0111 110r oooo hhhh]`

`r` is set if the signed destination value is added to `ip`. May not be set for the following branch opcodes: 0x3, 0x4, 0x5.

h: For branch opcodes 0x8 to 0x9, `[rrrr]`. 
For branch opcodes 0x0-0x2 and 0xB, `[00 ss]` where `ss` is the `log2(size)-1` of the immediate value used for the destination. 


For all other branch opcodes, reserved and must be zero

`oooo` is the branch opcode, given below.

Exceptions:
- PF: If any memory operand violates paging permissions
- UND, if the branch destination has size 16.


Instructions (The `X` in `0x7X_` opcodes denotes either `C` or `D`):
- 0x7X0 (jmp): Unconditional normal branch.
- 0x7X1 (call): Direct call, pushing the ip of the next instruction to the stack.
- 0x7X2 (fcall): Direct call, storing the ip of the next instruction in r14. 
- 0x7C3 (ret): pops an 8-byte value from from the stack and branches to that address.
- 0x7X8 (ijmp): Indirect normal branch to the destination.
    - 0x7X8 with h=0xe (fret): Fast Procedure Call Return.
    - 0x8 with h=0xf (ifjmp): Indirect Fast Jump.
- 0x7X9 (icall): Indirect call to the destination, pushing the ip of the next instruction to the stack.
- 0x7XA (ifcall): Indirect fast call to the address in r15, storing the ip of the next instruction in r14. 

#### Forcing Control Transfers

Encoding: `[0111 110r oooo hhhh]`

h: For branch opcode 0x7XC: `[v0 ss]` where `ss` is `log2(size)-1` of the immediate value used for the destination and `v` is whether to push `mode` before `ip`.

For branch opcode 0x7X5. `[iiii]` where `i` is the software interrupt number.


For branch opcodes 0x7XB, `[00 ss]` where `ss` is the `log2(size)-1` of the immediate value used for the destination. 

Exceptions:
- PROT: For branch opcodes 0x7XB-0x7XD, If the `mode.XM=0`, and the value to be stored in `mode` sets `mode.XM` to be different from `mode.XMM`. 
- PROT: For branch opcodes 0x7XB-0x7XD, if `mode.XM=1`, and the value to be stored in `mode` modifies the values of `mode[32:63]`
- PROT: For branch opcode 0x7X5, if `mode.XM=1` and the `PX` flag in the corresponding interrupt table entry is set to `0`
- PROT: For opcode 0x7X4, if `scdp=0`
- PROT: For opcode 0x7X4, if `mode.XM=0`
- UND: For branch opcodes 0x7XB-0x7XD, if any undefined by in `mode` would be set to `1`
- Other Branch Exceptions

Memory Effects:
- Opcode 0x7XC, Store to `double [r7-8]`.
- Opcode 0x7XC, if `v` is set in `h`, store to `double [r7-16]`.
- Opcode 0x7XD, load from `double [r7]`
- Opcode 0x7XD, load from `double [r7+8]`
- Performs a forcing control transfer

Instructions: 
- 0x7C4 (scall): Supervisor Call (See (Supervisor Call)[#Supervisor-Call])
- 0x7C5 (int): Raises the software interrupt `iiii` (See (Software Interrupts)[#Software-Interrupts])
- 0x7XB (jsm): Jump and Set flags. Reads r0 and stores the result in `mode`. 
- 0x7XC (callsm): Call procedure and set mode. If v is set in `h`, then pushes `mode` before `ip`. Then loads `mode` with the value in `r0. 
- 0x7CD (retrsm): Return from procedure and restore `mode`. pops an 8-byte value from the stack and returns to that address. Prior to returning, also pops `mode`. 


##### Supervisor calls

Opcode: 0x7c4

Supervisor calls are the primary way to transfer control from program execution mode to a supervisor. The branch destiniation is given by the scdp supervisor register, and additional information form the scsp and sccr registers.

The sccr register is a bitfield, with the following bits:

| Bit   | Description |
|-------|-------------|
| 1 (fc)| Uses fast supervisor call ABI (note: clobbers r12, r13, and r14)|

All other bits are reserved. Modifying such bits triggers UND

Memory Effects:
- Performs a forcing control transfer on the current CPU
- If `sccr.fc=0`, store to `32 [newsp-32]` (where `newsp` is `r7` if `scsp=0`, and the value of `scsp` otherwise)

Exceptions:
- PROT: If `scdp` is `0`.
- Other Branch Exceptions


If `sccr.fc=1`, then a fast supervisor call is performed as follows:
- `r7` is stored in `r12`, and, if scsp is not the value `0`, then scsp is stored in `r7`,
- `mode` is stored in `r13`, then both `mode.XM` and `mode.XMM` are cleared.
- the return address is stored in `r14`,
- control is transfered to the absolute address in scdp.

Otherwise, a stack supervisor call is performed as follows:
- `r7` is stored in a temporary location designated `tmp`, then, if scsp is not the value `0`, then `scsp` is stored in `r7`,
- The temporary location `tmp` is pushed to the stack,
- `flags` is pushed to the stack, 
- `mode` is pushed to the stack, then both `mode.XM` and `mode.XMM` are cleared
- The return address is pushed to the stack,
- control is transfered to the absolute address in scdp.


 
## Supervisor Instructions

These instructions are available only to the supervisor. If executed when mode.XM=1, then PROT is raised. 

### Supervisor Branches

Opcodes: 0xfc6-0xfc7
Operands: None

h: Shall be 0

Exceptions:
- PROT, if mode.XM=1
- Any branch exception

Memory Effects:
- Forcing Control Transfer
- Opcodes 0xFC6 (if `sccr.fc=0`) and 0xFC7,

Opcode 0xFC6 (scret) returns from a supervisor call. If sccr.fc is set, then the fast return procedure is used, as follows:
- `ip` is loaded from `r14`,
- `mode` is loaded from `r13`
- `r7` is loaded from  `r12`,
- Control is transfered to the address loaded into `ip`


Opcode 0xFC7 (reti) and Opcode 0xFC6 (when sccr.fc is not set) performs the following actions:
- `ip` is popped
- `mode` is popped
- `flags` is popped
- `r7` is popped
- Control resumes at `ip`


### Machine Specific Instructions

Opcodes: 0xfe0-0xffe
Operands: Machine Specific

h: Machine Specific

Exceptions:
- UND, if the host machine does not provide the specified instruction (refer to machine specific documentation)
- PROT, if mode.XM=1
- Any other Exception documented by the machine

Memory Effects:
- As prescribed by the machine

Instructions 0xfe0-0xffe are reserved for machine dependent behavior and will not be assigned further meaning in future ISA versions. Refer to machine specific documentation.

### Halt

Opcode: 0x801

Operands: 0
h: Shall be 0

Exceptions:
- PROT, if mode.XM=1

Memory Effects:
- None

Instruction:
- halts processor execution 

The halt instruction may be interrupted. The saved instruction pointer is the following instruction. Interrupting this instruction resumes processor execution.

### Clear Caches

Opcode: 0x802-0x805

Operands: Opcodes 0x804-0x805, 1. Opcode 0x802-0x803, 0.
h: Shall be 0

Operand Constraint: All operands shall be indirect registers or memory references.

Exceptions:
- PROT, if mode.XM=1
- UND, if any operand constraint is violated
- PF, if any memory operand is out of bounds
- For opcode 0x802: PF*, if any control bits are set in ptbl
- For opcode 0x805, XA, if the operand is not aligned to 2 bytes and has a size greater than 1.

Memory Effects:
- Inserts `PageFlush`, `GlobalFlush`, `DataFlush`, or `ExecFlush` respectively to the Global Synchronization Order

Instructions:
- 0x802 (pcfl): Causes all page caches to be flushed. This also causes ptbl to be checked (even if cr0.PG!=0). Performs the `PageFlush` synchronization operation.
- 0x803 (flall): Flushes data, instruction, and page caches on the current CPU. Performs the `GlobalFlush` synchronization operation
- 0x804 (dflush): Flushes the Data Cache for the given address. Performs the `DataFlush` synchronization operation on all bytes refered to by the operand
- 0x805 (iflush): Flushes the instruction cache for the given address.  Performs the `ExecFlush` synchronization operation on all bytes refered to by the operand. If the memory operand is larger than size 1, it must be aligned to at least 2 bytes.

All eligible memory operations are Visibility Candidates of a `GlobalFlush` operation. All Visibility Candidates of all preceeding synchronization operations in the global synchronization order are made visible to memory operations executed after it on the same CPU, and each of those memory operations and the Visibility Candidates of the `GlobalFlush` operation are additionally made Execution Visible to subsequent instruction fetches on the same CPU. This operation is strictly stronger than the `fence` instruction.

All eligible memory operations that access any byte refered to by the operand are Visibility Candidates of a `DataFlush` instruction. All Visibility Candidates of all preceeding synchronization operations in the global synchronization order that access any of those bytes are made visibile to memory operations after it on the same CPU.

For an `ExecFlush` synchronization operation, all eligible memory operations, and all Visibility Candidates of synchronization operations that preceed it in the global synchronization order that access any of the bytes refered to by the operand are made Execution Visible to any subsequent instruction fetch that reads any of those bytes.

The `PageFlush` synchronization operation does not make any Visibility Candidates visible to any subsequent memory operation, and does not have any Visibility Candidates. It still participates in the global synchronization order.


### I/O Transfers

Opcodes: 0x806-0x807

Operands: None

h: `[00 ss]`, where `ss` is `lg(size)` of the transfer.

Exceptions:
- PROT, if mode.XM=1

Flags: 0x806 Sets Z, M, and P based on the value read

Memory Effects:
- Inserts `IO` to the Global Synchronization Order.

Instructions:
- 0x806 (in): Reads `ss` bytes from the I/O Device at the address given in `r2` into `r0`. Performs the `IO` synchronization operation.
- 0x807 (out): Writes `ss` bytes from `r0` to the I/O Device at the address given in `r2`. Performs the `IO` synchronization operation.

I/O Device Addresses are 64-bit values, which correspond to some identifier assigned to devices. The I/O Device Addresses from 0x00000000-0xffffffff are reserved for use with this specification, future versions, extensions, and technical documents thereof.
All other device addresses have machine specific use.

If a device is not available on the machine, then reading it shall yield `0` and writing to it shall have no effect.

The `IO` synchronization operation does not make any Visibility Candidates visible to any subsequent memory operation, and does not have any Visibility Candidates. It still participates in the global synchronization order.


### Mass Register Storage

Opcodes: 0x808-0x809

Operands: 1

h: Reserved. Shall be 0

Operand Constraints: The operand shall be an indirect register, or a memory reference

Exceptions:
- UND, if any operand constraint is violated
- For opcode 0x809, PROT, if any reserved register is restored with a value other than `0`. 
- PROT, if mode.XM=1

Memory Effects:
- Opcode 0x808, 16 stores to `128 [#1 + 128*N]` where `N∈[0,16)`. The stores occur in an unspecified order.
- Opcode 0x809, 16 loads from `128 [#1 + 128*N]` where `N∈[0,16)`. The loads occur in an unspecified order.

Instructions:
- 0x808 (storegf): Stores the value of each register to the memory address specified by the operand. The value of `ip` stored is the address immediately following this instruction
- 0x809 (rstregf): Restores the value of each non-reserved, non-cpuinfo register from the memory address specified by the operand


## Multiple CPUs

A Clever Processor may contain more than one CPU. At startup, CPU 0 is initialized. 

The number of CPUs present on a machine can be determined by reading 2 bytes (ss=1) from I/O device address 0xfe80. CPUs <128 can be initialized by the following procedure. The procedure for other CPUs is machine specific:
The register load pointer for CPU *n* is present at I/O Device `0xff00+n`, and is an 8 byte physical address pointing to 512 bytes corresponding to the initialization state of the processes registers. Reserved Registers and cpuinfo registers MUST Be `0` or UND will be raised when the CPU is enabled. The CPU-Enable Control Word, described below, for CPU *n* is present at I/O Device `0xff80+n`. The behaviour of writing to either address for a CPU that is active, is undefined, and the behaviour of writing to the Register Load Pointer for an enabled CPU is undefined.

The CPU Enable Control Word has the following structure:

| Bits     | Name         | Description                                           |
|----------|--------------|-------------------------------------------------------|
| 0        | Enable       | Whether or not the CPU is enabled                     |
| 1        | Active       | Whether or not the CPU is actively executing code     |   
| 2-7      | Exception    | The interrupt or execution number to be serviced (0 indicates no exception) |
| 56-63    | Machine Specific Use| Misc values/flags that, when non-zero, has machine-specific meaning| 

All other bits are reserved and should be zero. The behaviour of setting an invalid CPU Enable Control Word is undefined.

Notes: 
- Exception does not report when a CPU is servicing ABRT, nor permits a different CPU from raising it. A value of `0` indicates the CPU is not servicing any interrupt, or that it is currently servicing ABRT. 
- A CPU that is disabled is also inactive, and all other bits (other than machine specific bits) are unused, including reserved bits. Thus, a disabled CPU can have it's CPU Enable Control Word used for Supervisor-specific purposes.
- An inactive, but enabled, CPU can still service interrupts (and will become active when doing so), but will not service exceptions written into the Exception field until reactivated. 
- Wehther or not the status of an active CPU is updated is not specified. 
- Causing any register to be initialized with an invalid value (`ip` not aligned to `0`, invalid `flags`, `cr`, or `mscr` bits set) causes undefined behaviour when the CPU is enabled.
- Storing an out-of-range physical address to the Register Load Pointer for any CPU causes undefined behaviour. 

The extensions (cpuex2-6 and mscpuex) supported by each CPU need not be the same. A processor can, at any time, query an inactive CPU's (ID<128) CPUInfo by writing an 8-byte physical address to the I/O Device Address `0x7f00+n`. The value of each cpuinfo register (registers 40-47) that would be reported by that CPU will be stored consecutively to the indicated address. If any operation would exceeds the physical address space bounds, `PF` is raised. 

## Interrupts

### Interrupt Table

```c=

struct itab_entry{
    int64_t ip;
    int64_t sp;
    struct{
        _Bool PX:1;
        unsigned __reserved0: 31;
        unsigned __reserved;
    } flags;
    uint64_t reserved;
};
struct itab{
    uint64_t extent;
    uint64_t reserved[3];
    struct itab_entry entry[extent/sizeof(struct itab_entry)];
};
```

The value of itabp shall be the physical address of an itab structure. Each index in itab is associated with a particular interrupt or exception number. `itabp` must be aligned to 32 bytes or the CPU resets when an exception occurs.

There may be up to 64 entries in the itab. The first 16 entries are used for processor exceptions. 
The next 16 entries are software interrupts. 
The remaining 32 entries are available for assigning to Interrupt Requests.

If an exception raised exceeds the bounds of itab, ABRT is raised. If ABRT exceeds those bounds, the processor resets.
If a software interrupt or IRQ raised exceeds the bounds of itab, PROT is raised. 

### Exceptions

num | Name (ID) | Description
----|-----------|------------------------------
 0  | Abort (ABRT) | Issued when an exception occurs in servicing any interrupt other than ABRT[^5]. The stored ip, sp, and flags values are undefined.
 1  | Undefined Instruction (UND) | Issued by undefined opcodes, invalid h values, or invalid operands to opcodes
 2  | System Protection (PROT) | Issued when a protection violation occurs, such as an access to a supervisor register or the use of a supervisor instruction from Program Execution Mode.
 3  | Page Fault (PF) | Issued when a memory access or paging violation occurs, or from Physical Memory Mode when an out-of-range physical address is referenced.
 4  | Execution Alignment (XA) | Issued when a system attempts to execute an instruction that isn't aligned to 2-bytes.
 5   | Non Maskable Interrupt (NMI) | Issued when an interrupt is raised by hardware that cannot be masked.
 6   | Division by Zero (DIV) | Issued when dividing by zero
 7-15| reserved  | Exceptions up to 15 may be assigned a future meaning.
 16 | Debug Trap[^6]| The first software interrupt. May be given special meaning in the future relating to trapping breakpoints for debuggers.
 
 
[^5]: If an exception occurs while servicing ABRT, the processor resets.
[^6]: Interrupt 16 is not an Exception. Failing to service Debug Trap does not raise ABRT.

#### Exception Codes

When certain exceptions occur, the `fcode` register is set by the CPU to certain information about the exception. When an exception has not occurred, the value is preserved from the last time an exception occurs. After any exception, either a value has been written to the register as defined in this section, or the register has an undefined state.

| Exception | Value Written |
|-----------|---------------|
| ABRT      | 0             |
| PROT      | The protection error code* |
| PF        | The virtual (paging enabled) or physical (paging disabled) address that was involved in the access |
| XA        | The address that was attempted to be executed |

When a Page Fault Occurs, additional information may be stored to address in the pfchar register (See [Paging Faults](#paging-fault)).


*All existing sources of PROT set the value to `0`, but future extensions may cause PROT exceptions that. Supervisor code may assume that if it enables no extensions for user code and does not use instructions from any extensions, that it will recieve a value of `0` in PROT handlers, but should not assume such about any particular extension it might enable.


## Init Procedure

During initialization, all registers have undefined values, except:
- ip, which shall be 0xff00
- flags, which shall be 0 (C=0, Z=0, V=0, N=0, FPEN=0, FPERR=0, FPQ=0, XM=0)
- cr0, which shall be 0 (PG=0, IE=0)
- flprotect, which shall be 0
- ciread, which shall be 0
- cpuidhi, cpuidlo, cpuex2, cpuex3, cpuex4, cpuex5, cpuex6, and mscpuex, which shall have machine-specific values consistent with the requirements of this document.
- Machine-specific msrs, which shall have machine-specific values or undefined values.

The memory at physical addreses 0x0000 through 0xffff shall be loaded in a machine-specific manner, all other memory addressess shall be 0. 

## Paging

### Address Space Size

All address spaces in the processor have a size, dictated by either cpuex2.PAS (for physical addresses), or cr0.PTL (for virtual addresses). 

The value of cr0.PTL dictates the size of the address space when cr0.PG=1. When paging is enabled, virtual addresses are signed integer values which are extended to 32-bits. In particular, the values of cr0.PTL specify the following ranges for virtual address
* 0: 32-bit virtual addresses, Range: [-0x80000000,0x80000000)
* 1: 36-bit virtual addresses, Range: [-0x800000000,0x800000000)
* 2: 48-bit virtual addresses, [-0x800000000000,0x800000000000)
* 3: 56-bit virtual address [-0x80000000000000,0x80000000000000)
* 4: 64-bit virtual addresses. [-0x8000000000000000,0x8000000000000000)

A virtual memory address is out of range if, when treated as a signed 64-bit integer, the value is outside of the range specified by the current value of cr0.PTL. 
Accessing an out of range virtual address while cr0.PG=1 causes a PF exception.
The maximum value of cr0.PTL is specified in excpu2.VAS. A UND exception occurs if a value outside this range is stored.

Physical address size, including memory operands when cr0.PG=0, the value of itab, or any element of a page table, is dictated by excpu2.PAS, which is the multiple of 4-bits exceeding 32-bit (up to 64-bit). Physical addresses are unsigned integers. A physical address is out-of-range if any bit exceeding `(4*excpu2.PAS)+32` is set. 
Accessing an out of range physical address while cr0.PG=0 causes a PF exception.

There need not be a correspondence between excpu2.PAS and excpu2.VAS. The physical address space may be smaller or larger than the permitted virtual address space. 



### Page Table format

Clever Name ISA uses a recursive page table format, as an array of 64-bit physical address, 4096-byte aligned (with the lower 12 bits used for flags). The first level table contains physical addresses for pages accessed accessed by corresponding virtual address, the second level table contains physical address of each first level table, etc. 
The physical address of the highest level table is present in the `page` register when `cr0.PG=1`. This address is likewise 4096-byte aligned and uses the lower 12 bits of the flags.

The number of levels (and width of the highest level level) is determined by the Page Table Nesting Level, cr0.PTL. Every level other than the first has length 512:
* The first level table can address 2^21, or 2097152 bytes.
* The second level table can address 2^30, or 1073741824 bytes.
* The third level table can address 2^39, or 549755813888 bytes. When cr0.PTL=0, it has size 8, which can address 2^32, or 4294967296 bytes. When cr0.PTL=1, it has size 64, which can address 2^36, or 68719476736 bytes.
* The fourth level table can address 2^48, or 281474976710656 bytes. It is used with cr0.PTL>1, and always has size 512.
* The fifth level table can address 2^57, or 144115188075855872 bytes. It is used with cr0.PTL>2, and has size 256 when cr0.PTL=3, which can address 2^56 or 72057594037927936 bytes.
* The sixth level table can address 2^64, or 18446744073709551616 bytes. It is used when cr0.PTL=4, and has size 128. 

The first level page table is known as the lower page table. Every page table above it is known as a nested page table. Entries in nested page tables are called nested page entries. Entries in the first level page table are lower page entries.

The lower 12 bits of each table entry are used as a bitfield, as follows:



| bit (mask)     | Name (id)                   | Description                                            |
| -------------- | --------------------------- | ------------------------------------------------------ |
| 0-1  (0x003)     | Page Permissions (PERM)           | Set to the permissions of the page  |
| 2  (0x004)     | Execution Mode (XM)         | Set if the page can be accessed from flags.XM=1        |
| 3 (0x008)      | Execution Mode Mirror (XMM) | Set to the save value as bit 3
| 4  (0x010)     | Supervisor No Execute (SXP) | Set if executing the page with mode.XM=0 should trap  |
| 10-12 (0x1C00) | Supervisor Use (OSSU)       | These bits are reserved for a Supervisior specific use |

All other flag bits are reserved and must be zero. Only values 0 or 1 are valid for PERM for nested page tables.
 Attempting to access a page with a reserved bit set triggers a PF. 
If XM or SXP is set on a nested page entry, it applies to all nested and lower page entries reached from that level page table, recursively. 

For Lower Page Entries, the PERM field is given by the following bitfield

| Value | Name  | Description |
|-------|-------|-------------|
| 0     | EMPTY | Indicates the page is empty/unmapped|
| 1     | READ  | Indicates that only read access is granted to the page|
| 2     | WRITE | Indicates that write access is granted to the page|
| 3     | EXEC  | Indicates that execute access is granted to the page|

Note: Read access is granted for all values other than EMPTY.

Accessing an EMPTY page, performing a write to a page that does not have WRITE permission, or executing instructions from a page that does not have EXEC permission triggers PF.


For Nested Page Entries, the PERM field is given by the following bitfield

| Value | Name     | Description |
|-------|----------|-------------|
| 0     | EMPTY    | Indicates that the nested page entry is unmapped|
| 1     | PRESENT  | Indicates that the nested page entry is present/mapped|

Note: Permission computations for pages do not depend on the PERM field of followed Nested Page Entries. Only present/not-present is determined by followed Nested Page Entries.

Other values are reserved. Attempting to access a page that violates this constraint triggers PF.

All flag bits in the `page` register are reserved and must be zero. Attempting to enable paging while any bit is set triggers PF. Note that paging will not be enabled if this requirement is violated (cr0.PG=0 will remain true). 

Note: If the value of `page` is changed, such that any reserved bit is set, the above check is not performed, and PF will be triggered on the first memory access, which is likely the first following instruction fetch. 
As handling PF will cause a page fault, ABRT will be triggered, which likewise cannot be handled, and the processor will reset. 

Changing the `page` register or modifying `cr0.PG` has the same effect as the `dflush` instruction followed by the `ptlbf`. 

### Paging Faults

A paging fault occurs in a number of situtations, when the paging rules of Clever-ISA are not followed by the program code. 

The pfchar register can point to 16 bytes of virtual memory that gets initialized by the processor when a page fault occurs. If the pfchar register is set to zero, this does not occur. The structure is given by the following definition:

```c
struct fault_information{
    uint64_t pref;
    uint8_t plevel;
    uint8_t access_kind;
    uint16_t status;
    uint16_t reserved[2];
};
```

`pref` is set to the value of the page entry that caused the fault, at whichever level. If the page fault is caused by an out-of-range virtual or physical address, the pref is set to `0`.  
`plevel` is set to the level that caused the page fault, where the lowest level is 0, and the levels increase upwards. If the page fault is caused by an out-of-range virtual or physical address, the plevel is set to `0`.  
`access_kind` is set to the value indicating what kind of access caused the fault, where `0` is a read access, `1` is a write access, `2` is an instruction fetch. 
 Additionally the special value `0xff` indicates a non-present memory fault, that occurs from accessing memory unavailable to the system.
`status` is set to the status flags about the access, indicated as follows:
* `0x0001` (XM): Set to the value of the mode.XM flag when the access occured
* `0x0002` (XMM): Set to a copy of the XM flag in status
* `0x0100` (VALIDATION_ERROR): Flag set if the fault occurs as a result of failed validation of a nested or lower page entry.
* `0x0200` (NON_CANONICAL): Flag set if the fault occurs as a result of a non-canonical (out-of-range) virtual address. It is also set for out-of-range physical addresses
* `0x0400` (NON_PAGED): Flag set if the fault occurs while paging is disabled - either an out-of-range physical address or a non-present memory fault
* `0x0800` (PREVENTED): Flag set if the fault occurs as a result of a failed access blocked by the Supervisor Execution Prevention feature or a similar feature
* `0x1000` (NESTED): Flag set if the fault occurs in a nested page entry, rather than a lower page entry. This is equivalent to the check `plevel>0`.

All other flags are reserved for future use and are set to `0`. The supervisor should not inspect these bits and should consider that the may have any value

The 4 bytes in `reserved` are reserved for future use and are set to `0`. The supervisor should not inspect those bytes and should consider that they may have any value

The write that occurs is totally atomic. 
If the pfchar register is not aligned to 16 bytes, or a page fault occurs writing to the pointee of pfchar, ABRT and no memory is modified. If a page fault otherwise occurs when triggering ABRT, no write occurs.

#### Non-Present Memory

Accessing physical memory may exceed limits on storage available to the system. If so, the system reports a page fault with an access_kind of non-present memory. 

There are two cases the under which the system may report non-present memory:
* The access reached a page physical memory that is not attached to any memory available to the system (For example, it exceeds the size of physical memory installed or addressible),
* The access reached a page dedicated for Memory Mapped I/O and the particular access does not reach a device attached to the system. 

The former case may also be used by emulators, to indicate that attempts to allocate memory for use by the emulated system failed. 

The system shall ensure that at the very least the first 64 kb of memory is available, and that the first 

### Paging and Execution

In virtual addressing mode, certain constraints apply to code being executed under most conditions. 
When control is transfered into a page, or execution crosses a page boundery while fetching an opcode, operand structure, or immediate value, page permissions are checked. 
Attempting to execute a not present page, a not executable page, or a page with supervisor execution protection enabled while `flags.XM=0`, causes a PF exception. 
Likewise, exceeding the physical or virtual address limits likewise causes a PF exception. 
These constraints are not guaranteed to be checked while continuous execution occurs within a particular page. Modifying the permissions of the current page being executed, or enabling/disabling paging, does not guarantee that an exception occurs unless one of the following occurs:
* Execution reaches and crosses a page boundary (checks the permissions of that page),
* a branch is taken (checks the permissions of the page the branch destination resides on)

Notwithstanding the above, a PF exception *may* occur when virtual addressing is enabled or when page permissions are changed, such that the current page being executed violates the execution constraints.

When paging is enabled or disabled, or the page table is modified, the behaviour is undefined unless the current page being executed and every following page until a taken branch occurs, is mapped to the same physical page which was previously being executed.


## Additions

The following behaviours are guaranteed to be maintained by future versions, including extensions:
- All instructions defined by this document, and all behaviours of those defined instructions, aside from exceptions
- All defined registers, and their meanings.
- Defined flags of flags, cr0, cpuex2.
- Required values for initialization state.
- The behaviour of accessing registers 63 and 255
- The format of operands, except operands with any reserved bits set
- The meaning of interrupts 16-63. 

The following changes may be made to the ISA, with a corresponding flag in a cpuex register, other than mscpuex:
- The addition of a new instruction, or a new register, except that new instructions may be reserved for machine-specific use without a corresponding flag
- A change in the format of an operand with any reserved bit set
- Additional uses for the h bits of existing instructions that are required to be 0.
- Additional uses for the bits of ciread, which are unused.
- Additional uses for flag bits of page tables and for the `page` register. 
- Uses of the flag bits 1 and 2 for any level page table beyond the first. 

The following changes may be made without a corresponding flag:
- Changes in the exceptions raised by instructions, but no previously defined behaviour may change behaviour without a corresponding control flag that was set.
- Addition of new Exceptions.
- Any Changes to the initialization procedure, including the values of registers at initialization
- Addition of new meaningful bits to any cpu extended feature availability registers.
- Additional uses for flags bits that are unused.
- Additional uses for bits of cr0.
- Use of cr8, and cr9 as Supervisor registers. 

The following changes are reserved to machine vendors:
- Uses of registers 148 through 154.
- Meanings of bits in mscpuex
- Contents of all cpuinfo registers
- Availability and behaviour of instructions 0xfe0-0xfff. 
- The timing, latency, and throughput of any instructions
- The reservation of instructions 0xfe0-0xfff

## Symbol Glossary

The following well known symbols are given the following meanings when they occur within encodings:

| Symbol | Size(s) | Meaning | 
|--------|---------|---------|
| o      | 12 | Opcode  |
| h      | 4  | Instruction-specific Control Bits |
| s      | 2 | size control value, either log2(size) or `log2(size)-1`. Larger sizes may be possible in extensions |
| z      | 2 | Alternative size control bits |
| r      | 4 or 6 | Register Number. Larger sizes may be possible with extensions |
| c      | 4   | Condition Code |
| l      | 1   | Lock Memory Operations |
| f      | 1   | Disable flags modification |

