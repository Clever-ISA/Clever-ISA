# Clever ISA

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
| 1   | r1/rsrc       | General        |                     Counter for block instrunctions                     |
| 2   | r2/rdst        | General        |                                                                         |
| 3   | r3/rcnt       | General        |                                                                         |
| 4   | r4            | General        |             Source Pointer Register for block instructions.             |
| 5   | r5            | General        |           Destination Pointer Register for block instructions           |
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
| 17  | flags         | Readonly[^2]   |                            Processor Status                             |                                                                       |
| 32  | cr0           | Supervisor     |                            Processor Control                            |
| 33  | cr1/page      | Supervisor     |            Physical Address of the Virtual Memory Page Table            |
| 34  | cr2/flprotect | Supervisor     |                  Supervisor Write protected flag bits.                  |
| 35  | cr3/scdp      | Supervisor     |                   Supervisor Call Destination Address                   |
| 36  | cr4/scsp      | Supervisor     |                      Supervisor Call Stack Pointer                      |
| 37  | cr5/sccr      | Supervisor     |                      Supervisor Call State Control                      |
| 38  | cr6/itabp     | Supervisor     |                    Interrupt/Exception Table Pointer                    |
| 39  | cr7/ciread    | Supervisor     | Bitflag of cpuinfo registers that can be read by program execution mode |
| 40  | cpuidlo       | CPUInfo        |        Contains the low order 64 bits of the Processor ID String        |
| 41  | cpuidhi       | CPUInfo        |        Contains the hi order 64-bits of the Processor ID String         |
| 42  | cpuex2        | CPUInfo        |           Processor Extension Availability Field 2. See below           |
| 43  | cpuex3        | CPUInfo        |     Processor Extension Availability Field 3. Contains the value 0      |
| 44  | cpuex4        | CPUInfo        |     Processor Extension Availability Field 4. Contains the value 0      |
| 45  | cpuex5        | CPUInfo        |     Processor Extension Availability Field 5. Contains the value 0      |
| 46  | cpuex6        | CPUInfo        |     Processor Extension Availability Field 6. Contains the value 0      |
| 47  | mscpuex       | CPUInfo        |        Machine Specific Processor Extension Availability Field.         |
| 48  | cr8           | Reserved       |                                                                         |
| 49  | cr9           | Reserved       |                                                                         |
| 50  | cr10          | Reserved       |                                                                         |
| 51  | cr11          | Reserved       |                                                                         |
| 56  | msr0          | Supervisor[^3] |                  Machine Specific Control Register 0.                   |
| 57  | msr1          | Supervisor[^3] |                  Machine Specific Control Register 1.                   |
| 58  | msr2          | Supervisor[^3] |                  Machine Specific Control Register 2.                   |
| 59  | msr3          | Supervisor[^3] |                  Machine Specific Control Register 3.                   |
| 60  | msr4          | Supervisor[^3] |                  Machine Specific Control Register 4.                   |
| 61  | msr5          | Supervisor[^3] |                  Machine Specific Control Register 5.                   |
| 62  | msr6          | Supervisor[^3] |                  Machine Specific Control Register 6.                   |
| 63  | undefined     | Reserved[^4]   |  Always reserved, and will not be given meaning in a future revision.   |

All unmentioned register numbers are reserved and have no name

Reserved Registers cannot be accessed (undefined)
FP Registers cannot be accessed if cr0.FPEN=0. 
Supervisor registers cannot be accessed in Program Execution Mode
CPUInfo registers cannot be written to. CPUInfo registers cannot be read in Program Execution Mode unless the corresponding bit in ciread is set.

flags Register bitfield:

|bit | Name (id)  | Notes
|----|------------|---------------------
|0   | Carry (C\)  | Set by arithmetic operations that cause unsigned overflow
|1   | Zero  (Z)  | Set by operations that have a zero result
|2   | Overflow (V)| Set by arithmetic operations that cause signed overflow
|3   | Negative (N)| Set by operations that have a negative result
|4   | Parity (P\) | Set by operations that have a result with Odd Parity.
|19  | Mode (XM)  | If clear, operating in supervisor mode, otherwise program execution mode. Cannot be written to from program execution mode, reguardless of flprotect.

All unmentioned flags are reserved and writes to those flags are masked.

Processor Control (cr0) bitifield

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



[^1]: Written by `call`, `ret`, `reti`, `scall`, `scret`, `int`, and `b`*`cc`*.
[^2]: Except to the pop, reti, and scret instructions. flprotect masks writes from the pop instruction, other than bits 0-3, in Program Execution Mode
[^3]: Machine Specific Control Registers may be unavailable and reserved in Supervisor mode, and their behaviour is not specified. Refer to documentation for the particular Processor ID. The author of this documentation does not endorse the content of such documentation. An implementation shall not make the registers available to program execution mode, unless permitted by the supervisor.
[^4]: Tools that produce machine code for this architecture may rely on register 63 being reserved. 

## Operands
### Register Operand

`[00 yyyy ss xx rrrrrr]`
Where:
- ss is the size control value (log2(size) bytes).
- xx is reserved and must be zero
- yyyy is reserved and must be zero
- rrrrrr is register value 0<=r<64


### Indirect 

`[01 oooo lll k ss rrrr]`

ss, and rrrr have the same meanings as in a Register value.  Only General registers may be accessed by indirect addressing modes

If `k` is set, then `oooo` is the offset to add to the value of rrrr, after scaling by the scaling factor in `lll`=log2(scale)). 
If `k` is clear, then `oooolll` form the immediate value added to the value of `rrrr`.

### Short Imm

`[10 y r iiiiiiiiiiii]`

An up to 12-bit immediate value can encoded directly into the operand. If `r` is set, then the value of `ip` is added to the immediate value.

### Long Imm 

`[11 m yy r ss yyyyyyyy]`

An Immediate value, with a size given by ss (`log2(size)-1`).  If `r` is set, then the value of `ip` is added to the immediate value.

`yy` and `yyyyyyyy` are all reserved and must be zero. The immediate value follows the operand control structure.

If `m` is set, the the immediate value is a memory reference.

Immediate values of size 16 (ss=3) are reserved, and attempts to use them in an operand will unconditionally produce `UND`.

## Encoding

Opcode:
`[oooooooooooo hhhh]`: `o` is the 12-bit opcode, and `hhhh` is used for instruction specific information.


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


### Arithmetic/Logic Instructions 

Opcodes: 0x001-0x005.
Operands: 2

h: `[l0 0f]`. If `f` is set, then `flags` is not modified. If `l` is set, the memory operation is performed under a lock, and is atomic wrt. other memory operations, loads, and stores.


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

Flags:  Sets C, M, V, and Z according to the result of the operation, unless the `f` bit is set in `h`

Instructions:
- 0x001 (add): Adds the value of the two operands
- 0x002 (sub): Subtracts the value of the second operand from the first
- 0x003 (and): Performs the bitwise and of the two operands
- 0x004 (or):  Perfomrs the bitwise or of the two operands
- 0x005 (xor): Performs the bitwise exclusive or of the two operands

### Multiplication and Division Instructions

Opcodes: 0x006-0x007
Operands: None

h: `[ss wf]`, where `ss` is log2(size) of the operation, as though for a register operand size control. `w` shall be set only for opcode 0x007. If `f` is set, then `flags` is not modified. 

Flags: Sets C, M, V, and Z according to the result of the operation if the `f` bit is not set in `h`.


Instructions:
- 0x006 (mul): Multiplies r0 and r3, storing the low order bits in r0, and the high order bits in r3.
- 0x007 (div): Divides r0 by r3, storing the quotient in r0 and the remainder in r3. If w is set in h, then the dividend has double the size specified by ss, and the high order bits are present in r1.

### Register Manipulation Instructions

Opcodes 0x008-0x00b
Operands: 2 for opcodes 0x008 and 0x009, 1 for opcodes 0x00a and 0x00b.

h: Opcodes 0x00a and 0x00b only, the general purpose register for the source (0x00a) or destination (0x00b) operand. Otherwise, reserved and shall be zero

Operand Constraints: For opcodes 0x008 and 0x009, the first operand shall be a memory reference or register, and at least one operand shall be a register. For Opcode 0x009, the second operand shall be either an indirect register, or a memory reference. 

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

Instructions:
- 0x008 (mov): Moves the value of the second operand into the first
- 0x009 (lea): Moves the computed address of the second operand into the first, without setting flags
- 0x00a (mov*r*): Short form if the destination operand is a gpr (0<=r<16). Moves the operand into *r*, according to the size of the operand
- 0x00b (mov*r*): Short form if the source operand is a gpr (0<=r<16). Moves *r* into the operand, according to the size of the operand.

If the destination operand of `mov` or `lea` is smaller than the result, the value stored is truncated. If the destination operand is larger than the result, the value stored is zero-extended to the destination size.
All memory accesses are performed atomically, wrt. other memory accesses, and operations performed under a memory lock. 

### Subroutines/Unconditional Jumps

Opcodes: 0x7C0-0x7C5, 0x7c8-0x7c9
Operand: For opcodes 0x7c0, 0x7c1, and 0x7c8, 1 ss immediate (not in Operand Form). 
h: For opcodes 0x7c0, 0x7c1, and 0x7c8, `[ss i0]`, where ss is `log2(size)-1` in bytes, and if `i` is set, the signed (2s compliment) value is added to `ip` to obtain the actual address. For opcode 0x7c4, and 0x7c9, `rrrr`, where `r` is the gpr (0<=r<16) that contains the branch target address. For all other opcodes, shall be 0.


Exceptions:
- UND, if any operand constraint is violated.
- UND, if a reserved register is accessed by the instruction
- PROT, if a Supervisor register is accessed, and the program is not in Program Execution Mode
- PROT, If `ss` is 3
- PF, if the target address is an unavailable virtual memory address
- PF, if page execution protections are violated by the access
- PF, if paging is disable, and the target address is an out of range physical address
- PROT, if the target address is out of range for the PTL mode.
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if paging is disable, and a memory operand accesses an out of range physical address
- XA, if the jump address is not 2-byte aligned

Instructions:
- 0x7c0 (call): Performs a subroutine call to the immediate operand, pushing the return address to the stack. If `i` is set, the current value of `ip` is added to the signed immediate.
- 0x7c1 (fcall): Performs a "fast" subroutine call to the immediate operand, storing the return address in `r15`. If `i` is set, the current value of `ip` is added to the signed immediate.
- 0x7c2 (ret): Returns from a subroutine by popping the return address from the stack.
- 0x7c3 (fret): Performs a "fast" subroutine return by restoring the return address in `r15`. 
- 0x7c4 (icall): Performs an indirect call to address stored in the given register, pushing the return address to the stack.
- 0x7c5 (ifcall): Performs a "fast" indirect call to the address stored in `r14`, storing the return value in `r15`.
- 0x7c8 (jmp): Performs a jump to the immediate operand. If `i` is set, the current value of `ip` is added to the signed immediate
- 0x7c9 (ijmp): Performs an indirect jmp to the address stored in the given register.

### Supervisor Calls

Opcodes: 0x7c6 (scall), 0x7c7 (int)

Operands: None

h: For opcode 0x7c6, Shall be 0. For opcode 0x7c7, inum-16. 

Exceptions:
- UND, if scdp is 0
- PF, if the target address is an unavailable virtual memory address
- PF, if page execution protections are violated by the access
- PF, if paging is disable, and the target address is an out of range physical address
- PROT, if the target address is out of range for the PTL mode.
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if paging is disable, and a memory operand accesses an out of range physical address
- XA, if the desination address is not 2-byte aligned.

Opcode 0x7c6 performs the following operations, ignoring flags.XM and register protection:
- The value `sp` is copied into a temporary location, then `sp` is loaded from `scsp` if that value is nonzero
- If `scsc.FRET=1`, the value copied from `sp` is stored to `r14`, otherwise it is pushed to the stack.
- If `scsc.FRET=1`, the return address is stored in `r15`, otherwise it is pushed to the stack.
- `flags` is stored in `r13` if scsc.RSTF=1
- `flags.XM` is cleared
- `ip` is loaded from `scdp`
- Control Resumes from the new value of `ip`.

Opcode 0x7c7 performs the following operantions, ignoring flags.XM and register protection:
- If `itab[inum].flags.PX` is clear, and `flags.XM=1`, then PROT is raised
- Otherwise, the value `sp` is copied into a temporary location, then `sp` is loaded from `itab[inum].sp`
- The copied value of `sp` is pushed onto the stack
- `flags` is pushed onto the stack
- `ip` is pushed onto the stack
- `ip` is loaded from `itab[inum].ip`

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

Instructions:
- 0x014 (push): Decrements sp by 8, then stores the (potentially zero extended) 8-byte value in the operand to the address indicated by sp
- 0x015 (pop): Loads the 8-byte value from the address indicated by sp, and stores it in the operand, then increments sp by 8
- 0x016 (push*r*): Same as 0x014, but operates on the register indicated by `h`
- 0x017 (pop*r*): Same as 0x015, but operates on the register indicated by `h`



### Bulk Register Storage

Opcodes: 0x018-0x01f
Operands: For opcodes 0x018-0x01b, 1. For opcodes 0x01c-0x01f, None.

h: Shall be 0.

Operand Constraints: For 0x018-0x01b, the operand shall be an indirect register or a memory reference.

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
- PROT, If opcodes 0x01b or 0x01f are used to load a value other than 0 into a reserved or unavailable register.

Instructions:
- 0x018 (stogpr): Stores the value of each general purpose register (0<=r<16) to the memory operand.
- 0x019 (stoar): Stores the value of each program register (0<=r<32) to the memory operand. Any reserved or unavailable floating-point register stores 0 instead.
- 0x01a (ldgpr): Loads the value of each general purpose register (0<=r<16) from the memory operand. 
- 0x01b (ldar): Loads the value of each program register (0<=r<32) from the memory operand. 
- 0x01c (pushgpr): Pushes the value of each general purpose register (0<=r<16) to the stack. Lower memory addresses (closer to the stack head) store lower numbered registers.
- 0x01d (pushar): Pushes the value of each program register (0<=r<32) to the memory operand. Any reserved or unavailable register stores 0 instead. Lower memory addresses (closer to the stack head) store lower numbered registers.
- 0x01e (popgpr): Pops the value of each general purpose register from the stack.
- 0x01f (popar): Pops the value of each program register (0<=r<32) from the stack. 

### Converting Moves

Opcodes: 0x020-0x027
Operands: 2

h: For opcodes 0x020-0x021 `[00 0f]`where if `f` is set `flags` is not modified.

Operand Constraints: For opcode 0x020 and 0x021, at least one operand shall be a register. 

Flags: `M` and `Z` are set according to the result, unless `f` is set in `h`.

Exceptions:
- UND, if a operand constraint is violated
- UND, if any destination operand is flags or ip
- PROT, if a supervisor register is an operand, and `flags.XM=1`.
- UND, if a reserved register is an operand.
- UND, if a floating-point operand has size 1
- PF, if the target address is an unavailable virtual memory address
- PF, if page protections are violated by the access
- PF, if paging is disable, and the target address is an out of range physical address
- PROT, if the target address is out of range for the PTL mode.
- PF, if a memory operand accesses an unavailable virtual memory address
- PROT, If opcodes 0x01b or 0x01f are used to load a value other than 0 into a reserved or unavailable register.

Instructions:
- 0x020 (movsx): Moves a signed integer operand from the second operand, to the first. If the second operand is smaller than the first, the highest bit is copied to each higher bit in the first opernd.
- 0x021 (bswap): Moves the second operand into the first, swapping the order of the bytes stored. 


All memory accesses are performed atomically. Note that the entire operation is not required to be atomic.

### Block Operations

Opcodes: 0x028-0x02f

Operands: None.

h: For opcode 0x028 and 0x029, `[cccc]` where `cc` is the condition code encoding (see [Condition Code Encoding](#condition-code-encoding)). For opcodes 0x02a-0x02f,  `[00 ss]`, where `ss` indicates the size of each individual operation.

Exceptions:
- PF, if any address is an unavailable virtual memory address
- PF, if page protections are violated by the access
- PF, if paging is disable, and the target address is an out of range physical address
- PROT, if the target address is out of range for the PTL mode.
- PF, if a memory operand accesses an unavailable virtual memory address
- PROT, If opcodes 0x01b or 0x01f are used to load a value other than 0 into a reserved or unavailable register.

Flags: Opcodes 0x02a-0x02c set Z and M according to the value transfered. Opcodes 0x02d-0x02f set according to the comparison performed.

Instructions:
- 0x028 (repbi): Shall be immediately followed by a block operation (opcodes 0x02a-0x02f). Repeats the following operation until the condition is satisfied
- 0x029 (repbc): Shall be immediately followed by a block operation (opcodes 0x02a-0x02f). Repeats the following operation and decrements r1 until the condition is satisified, or r1 is 0.
- 0x02a (bcpy): Loads the value (according to `ss`) at the address in r4, and stores it to the address in r5, then adds `ss` to both r4 and r5.
- 0x02b (bsto): Stores the value (according to `ss`) in r0 into the address in `r5`, then adds `ss` to `r5`.
- 0x02c (bsca): Loads the value (according to `ss`) from the address in `r4` into `r0`.
- 0x02d (bcmp): Compares the value (according to `ss`) at the address in `r4` with the value at the address in `r5` and sets the corresponding flags, then adds `ss` to both `r4` and `r5`.
- 0x02e (btst): Compares the value (according to `ss`) at the address in `r4` with the value in `r0` and sets the corresponding flags, then adds `ss` to `r4`.

Each Load and each Store operation performed by these instructions are atomic wrt. other memory accesses and memory operations performed under a memory lock. If repi or repc are used, note that each operation is individual. 


### Integer Shifts

Opcodes: 0x030-0x036, 0x038-0x03f

Operands: For opcodes 0x030-0x036, 2. For opcodes 0x038-0x03e, 1.

h: For opcodes 0x030-0x037, `[l0 0f]` where if `f` is set, `flags` is not modified, and if `l` is set, the memory operation is performed under a memory lock and is atomic wrt. other memory accesses and memory operations performed under a lock. For opcodes 0x038-0x3f, `[rrrr]` where `r` is the number of the gpr (0<=r<16) of the first operand. 


Operand Constraints: At least one operand shall be an immediate value other than a memory reference, or gpr. The first operand shall be a register, indirect register, or memory reference. No operand shall be a floating-point register. If l is set in h, the first operand shall be an indirect register or a memory reference.

Exceptions: 
- UND, if any operand constraint is violated. 
- UND, if a reserved register is accessed by the instruction
- PROT, if a Supervisor register is accessed, and the program is not in Program Execution Mode
- UND, if the destination operand is the flags or ip register.
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if the first operand is a memory operand, and page protections are violated by the access
- PF, if paging is disable, and a memory operand accesses an out of range physical address
- PROT, if a memory operand is out of range for the PTL mode.

Flags: Unless `f` is set in `h`, sets M, V, and Z according to the result of the operation. Sets C if the last bit shifted out was 1. 

Instructions:
- 0x030 (lsh): Left Shifts the first operand by the second, shifting in 0 bits
- 0x031 (rsh): Right Shifts the first operand by the second, shifting in 0 bits
- 0x032 (arsh): Right Shifts the first operand by the second, preserving and copying the most significant bit
- 0x033 (lshc): Left Shifts the first operand by the second, shifting in the value of the carry flag
- 0x034 (rshc): Right Shifts the first operand by the second, shifting in teh value of the carry flag.
- 0x035 (lrot): Left Rotates the first operand by the second, shifting the bits that were shifted out into the low order bits
- 0x036 (rrot): Right Rotates the first operand by the second, shifting the bits that were shifted out into the high order bits
- 0x038 (lsh*r*): Specialization of lsh that operates on a gpr.
- 0x039 (rsh*r*): Specialization of rsh that operates on a gpr.
- 0x03a (arsh*r*): Specialization of arsh that operates on a gpr
- 0x03b (lshc*r*): Specialization of lshc that operates on a gpr
- 0x03c (rshc*r*): Specialization of rshc that operates on a gpr
- 0x03d (lrot*r*): Specialization of lrot that operates on a gpr
- 0x03e (rrot*r*): Specialization of rrot that operates on a gpr.


### Comparison Operations

Opcode: 0x12c-0x12f

Operands: Opcode 0x12c-0x12d, 2. Opcode 0x12e-0x12f, 1.

h: For Opcodes 0x12c-0x12d, shall be zero. For opcodes 0x12e-0x12f, `[rrrr]` where r is the gpr to use as the first operand (0<=r<16).

Operand Constraints: At least one operand shall be an immediate value other than a memory reference, or a register. The first operand shall be a register, indirect register, or memory reference. 

Exceptions: 
- UND, if any operand constraint is violated. 
- UND, if a reserved register is accessed by the instruction
- UND, if a floating-point register is accessed, and cr0.FPEN=0
- PROT, if a Supervisor register is accessed, and the program is not in Program Execution Mode
- UND, if the destination operand is the flags or ip register.
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if the first operand is a memory operand, and page protections are violated by the access
- PF, if paging is disable, and a memory operand accesses an out of range physical address
- PROT, if a memory operand is out of range for the PTL mode.

Flags: Opcode 0x12c and 0x12e, sets Z, N, M, and V according to the result. Opcode 0x12d and 0x12f, sets Z and M according to the result.

Instructions:
- 0x12c (cmp): Subtracts the second operand from the first, and sets flags based on the result. 
- 0x12d (test): Computes the bitwise and of the first and second operands and sets flags based on the result.
- 0x12e (cmp*r*): Same as cmp, but the first operand is the encoded general purpose register
- 0x12f (test*r*): Same as test, but the first operand is the encoded general purpose register.


All memory accesses are atomic wrt. other memory accesses and memory stores.

### Atomic Operations

Opcodes: 0x200-0x203
Operands: Opcode 0x200: 2, Opcode 0x201-0x202: 3, Opcodes 0x203: 0.
h: Reserved and shall be zero

Operand Constraints: For Opcodes 0x200, at least one operand shall be a register and no operand may be an immediate value. For opcode 0x201 and 0x202, the neither the first nor second operand may be an immediate value, and at least one of the first and second shall be a register. 

Flags: For Opcode 0x201 and 0x202, `Z` is set to the result of the comparison. 

Instructions:
- 0x200 (xchg): Exchanges the value im the first operand with the second
- 0x201 (cmpxchg): Compares the value in the first operand with the value in the second and stores the third in the first if the comparison sets the Z flag, otherwise the value in the first operand is stored in the second
- 0x202 (wcmpxchg): Same as 0x201, but the comparison is permitted to spuriously fail
- 0x203 (fence): Synchronizes memory accesses accross cpus. Immediately before the instruction is executed, all memory operations performed on the current cpu by prior instructions shall be completed, and no new memory accesses performed on the current cpu by subsequent instructions shall begin until the instruction completes. All fence instructions on all cpus shall occur in some total order.

All operations described in this section are totally atomic.
wcmpxchg exists to permit efficient implementations which can be made use of when reliable success is not necessary.

### Branches

Opcodes: 0x700-0x73F and 0x780-0x78F

Branch Instruction Encoding:
`[0111 00ss cccc hhhh]`: `ss` is the log2(size)-1 of the signed branch offset, following the instruction. `hhhh` is a signed weight for the branch, with -8 being the most likely not to be taken, and 7 being the most likely to be taken (0 indicates roughly even probability). `cccc` is the encoding of the condition code for the branch.

`[1111 1000 cccc rrrr]`: Indirect branch to a (general purpose) register `rrrr`. 

Exceptions:
- UND: If size is `16`.
- PF, if the target address is an unavailable virtual memory address
- PF, if page execution protections are violated by the access
- PF, if paging is disable, and the target address is an out of range physical address
- PROT, if the target address is out of range for the PTL mode.
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if paging is disable, and a memory operand accesses an out of range physical address
- XA, if the desination address is not 2-byte aligned

Instructions:
- Opcodes 0x700-0x73F, if the condition indicated by *c* is satisified, branches to `ip+imm`
- Opcodes 0x780-0x78F, if the condition indicated by *c* is satisfied, branches to the address stored in `r`.

#### Condition Code Encoding

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
 9       | Above or Eq| flags.C=0 or flags.Z=1
 10      | Greater/Eq | flags.N=flags.V for flags.Z=1
 11      | Greater    | flags.N=flags.V
 12      | Not Zero/Eq| flags.Z=0
 13      | No Overflow| flags.V=0
 14      | No Carry   | flags.C=0
 15      | No Parity  | flags.P=0

 
## Supervisor Instructions

These instructions are available only to the supervisor. If executed when flags.XM=1, then PROT is raised. 

### Supervisor/Interrupt Returns

Opcodes: 0xFC8-0xFC9
Operands: None

h: Shall be 0

Exceptions:
- PF, if the target address is an unavailable virtual memory address
- PF, if page execution protections are violated by the access
- PF, if paging is disable, and the target address is an out of range physical address
- PROT, if the target address is out of range for the PTL mode.
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if paging is disable, and a memory operand accesses an out of range physical address
- XA, if the destination address is not 2-byte aligned

Opcode 0xFC8 (scret) performs the following actions:
- If `scsc.RSTF=1`, `flags` is loaded from `r13`
- `flags.XM` is set 
- If `scsc.FRET=1`, then `ip` is loaded from `r15`, otherwise `ip` is popped from the stack
- If `scsc.FRET=1`, then `sp` is loaded from `r14`, otherwise `sp` is popped from the stack
- Control resumes at `ip`

Opcode 0xFC9 (reti) performs the following actions:
- `ip` is popped
- `flags` is popped
- `sp` is popped
- Control resumes at `ip`


### Machine Specific Instructions

Opcodes: 0xfe0-0xfff
Operands: Machine Specific

h: Machine Specific

Exceptions:
- UND, if the host machine does not provide the specified instruction (refer to machine specific documentation)
- PROT, if flags.XM=1
- Any other Exception documented by the machine

Instructions 0xfe0-0xfff are reserved for machine dependent behaviour and will not be assigned further meaning in future ISA versions. Refer to machine specific documentation.

### Halt

Opcode: 0x801

Operands: 0
h: Shall be 0

Exceptions:
- PROT, if flags.XM=1

Instruction:
- ceases processor execution until an interrupt occurs or RESET


### Clear Caches

Opcode: 0x802-0x805

Operands: Opcodes 0x804-0x805, 1. Opcode 0x802-0x803, 0.
h: Shall be 0

Operand Constraint: All operands shall be indirect registers or memory references.

Exceptions:
- PROT, if flags.XM=1
- UND, if any operand constraint is violated
- PF, if any memory operand is out of bounds
- For opcode 0x802: PF, if any control bits are set in ptbl

Instructions:
- 0x802 (pcfl): Causes all page caches to be flushed. This also causes ptbl to be checked (even if cr0.PG!=0). 
- 0x803 (flall): Flushes data, instruction, and page caches on the current CPU.
- 0x804 (dflush): Flushes the Data Cache for the given address. 
- 0x805 (iflush): Flushes the instruction cache for the given address.

### I/O Transfers

Opcodes: 0x806-0x807

Operands: None

h: `[00 ss]`, where `ss` is `lg(size)` of the transfer.

Exceptions:
- PROT, if flags.XM=1

Flags: 0x806 Sets Z, M, and P based on the value read

Instructions:
- 0x806 (in): Reads `ss` bytes from the I/O Device at the address given in `r1` into `r0`.
- 0x807 (out): Writes `ss` bytes from `r0` to the I/O Device at the address given in `r0`.

Both opcodes 0x806 and 0x807 may be modified by opcodes 0x028 and 0x029 (repc and repi). If so, the operation is performed until the condition is satisifed.

I/O Device Addresses are 64-bit values, which correspond to some identifier assigned to devices. The I/O Device Addresses from 0x0000-0xffff are reserved for use with this specification, future versions, and extensions thereof.
All other device addresses have machine specific use

### Mass Register Storage

Opcodes: 0x808-0x809

Operands: 1

h: Reserved. Shall be 0

Operand Constraints: The operand shall be an indirect register, or a memory reference

Exceptions:
- UND, if any operand constraint is violated
- For opcode 0x809, PROT, if any reserved register is restored with a value other than `0`. 
- PROT, if flags.XM=1

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
    size_t extent;
    struct itab_entry entry[extent/sizeof(struct itab_entry)];
};
```

The value of itbp shall be the physical address of an itab structure. Each index in itab is associated with a particular interrupt or exception number.

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
 6-15| reserved  | Exceptions up to 15 may be assigned a future meaning.
 16 | Debug Trap[^6]| The first software interrupt. May be given special meaning in the future relating to trapping breakpoints for debuggers.
 
 
[^5]: If an exception occurs while servicing ABRT, the processor resets.
[^6]: Interrupt 16 is not an Exception. Failing to service Debug Trap does not raise ABRT.


## Init Procedure

During initialization, all registers have undefined values, except:
- ip, which shall be 0xff00
- flags, which shall be 0 (C=0, Z=0, V=0, N=0, FPEN=0, FPERR=0, FPQ=0, XM=0)
- cr0, which shall be 0 (FPEN=0, IE=0)
- flprotect, which shall be 0
- ciread, which shall be 0
- cpuidhi, cpuidlo, cpuex2, cpuex3, cpuex4, cpuex5, cpuex6, and mscpuex, which shall have machine-specific values consistent with the requirements of this document.
- Machine-specific msrs, which shall have machine-specific values or undefined values.

The memory at physical addreses 0x0000 through 0xffff shall be loaded in a machine-specific manner, all other memory addressess shall be 0. 

## Address Space Size

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

The lower 12 bits of each table entry are used as a bitfield, as follows:



| bit (mask)     | Name (id)                   | Description                                            |
| -------------- | --------------------------- | ------------------------------------------------------ |
| 0  (0x001)     | Present (PR)                | Set if the page entry is mapped to a physical address  |
| 1  (0x002)     | Writeable (WR)              | Set if the page entry permits writing                  |
| 2  (0x004)     | No Execute (NX)             | Set if the page prohibits instruction fetches          |
| 3  (0x008)     | Execution Mode (XM)         | Set if the page can be accessed from flags.XM=1        |
| 4  (0x010)     | Supervisor No Execute (SXP) | Set if executing the page with flags.XM=0 should trap  |
| 10-12 (0x1C00) | Supervisor Use (OSSU)       | These bits are reserved for a Supervisior specific use |

All other flag bits are reserved and must be zero. Bits 1 and 2 are reserved for all levels other than the first. Attempting to access a page with a reserved bit set triggers a PF. 
If a bit is set for any level above the first, it applies to all pages below that level. 

All flag bits in the `page` register are reserved and must be zero. Attempting to enable paging while any bit is set triggers PF. Note that paging will not be enabled if this requirement is violated (cr0.PG=0 will remain true). 

Note: If the value of `page` is changed, such that any reserved bit is set, the above check is not performed, and PF will be triggered on the first memory access, which is likely the first following instruction fetch. 
As handling PF will cause a page fault, ABRT will be triggered, which likewise cannot be handled, and the processor will reset. 


## Additions

The following behaviours are guaranteed to be maintained by future versions, including extensions:
- All instructions defined by this document, and all behaviours of those defined instructions, aside from exceptions
- The reservation of instructions 0x7e0-0x7ff
- All defined registers, and their meanings.
- Defined flags of flags, cr0, cpuex2.
- Required values for initialization state.
- The behaviour of accessing r63
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
- Uses of registers 56 through 62.
- Meanings of bits in mscpuex
- Contents of all cpuinfo registers
- Availability and behaviour of instructions 0xfe0-0xfff. 
- The timing, latency, and throughput of any instructions

