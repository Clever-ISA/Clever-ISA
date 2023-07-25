# Hash/Pseudorandom Function Hardware Acceleration

This extension defines a number of operations used to accelerate the execution of Checksum and Pseudorandom functions.

Most algorithms in this extension are not suitable for use when security is required - the X-crypto extension should be used instead.

## Conformance

A CPU indicates conformance with these extensions by setting bit 14 (HACCEL) of cpuex2. A CPU that does not support these extensions shall have that bit set to `0`.


## Supervisor Support

If cpuex2.HACCEL is set by the CPU, the supervisor may set bit 11 (HACCEL) to enable the instructions in this extension.

## Instructions

### SipHash Round function

Opcodes: 0x238

Operands: 2 

hbits: Set to `0`

Operand Constraints: Both operands must be a vector register with size 16. The two operands may not refer to the same vector register.


Exceptions:
- UND if cpuex2.HACCEL is `0`
- UND if cr0.HACCEL is `0`
- UND if any operand constraint is violated

Instructions:
- 0x238 (`sipround`): Performs the SipRound function from the [SipHash](https://ia.cr/2012/351) algorithm, reading the state `[v0 v1 v2 v3]` from the two 64-bit parts of the first then second operand, then stores the result back in the same operands. 

### Partial Load

Opcodes: 0x070

Operands: 3

hbits: Set to `0`

Operand Constraints: The first operand must be a general purpose register or a vector register. The second operand must be a memory operand. The third operand must be a general purpose register or an immediate value, other than a vector immediate

Exceptions:
- UND if cpuex2.HACCEL is `0`
- UND if cr0.HACCEL is `0`
- UND if any operand constraint is violated
- UND, if any operand is a vector register or a memory reference of size 16, and cr0.VEC is `0`
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if the first operand is a memory operand, and page protections are violated by the access
- PF, if paging is disabled, and a memory operand accesses an out of range physical address
- PROT, if the third operand exceeds the size of the memory operand

Instructions:
- 0x070 ('pload'): Performs a partial memory load from the second operand of a size given by the third operand, and places the result in the general purpose register designated by the first operand. 

The behaviour of the instruction may differ from other instructions when the memory operand is not aligned to the size of the second operand (as specified by its size-specifier). In particular, if a page fault occurs from reading an unaligned memory operand that crosses a page boundary, it is unspecified whether any portion that resides on a readable page is read. In addition, an unaligned partial load is not guaranteed to be atomic, and is permitted to tear on byte boundaries. 


### Multiply-arithmetic

Opcodes: 0x23A-0x23B

Operands: 3

hbits: `[0ooo]` Where `ooo` is the lower 3 bits of the corresponding opcode for the aritmetic operation

Operand Constraints: The first operand must be a register or a memory reference, and at most one operand may be a memory reference. No operand may be a register, other than a vector register or general purpose register. No operand may be a vector pair or have size 16.

Exceptions:
- UND if cpuex2.HACCEL is `0`
- UND if cr0.HACCEL is `0`
- UND if any operand constraint is violated
- UND if `ooo` is 6, or 7.
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if the first operand is a memory operand, and page protections are violated by the access
- PF, if paging is disabled, and a memory operand accesses an out of range physical address


Instructons:
- 0x23A (`fusedmul`): Multiples the first operand by the second, then combines the result with the third operand using the arithmetic or logic operation encoded in `ooo`
- 0x23B (`fusedimul`): Same as 0x23A, except peforms signed multiplication instead of unsigned.

The operations that can be encoded into `fusedmul` are `nop`, `add`, `sub`, `and`, `or`, or `xor`, corresponding to the opcode value of operation.

`o`=0 correponds to `nop` rather than `und`. The result of the computation is `a=a*b`, where `a` is the first operand and `b` is the second. 
This can be used to peform multiplication with explicit operands, rather than `r0` and `r3` implicitly used by the `mul` and `imul` instructions. The third operand is ignored in this case.

`o`=6 and `o`=7 (corresponding to `mul` and `div` respectively) are reserved.

The result size is given by the size of the destination operand, and upper bits are discarded. This instruction cannot be used to perform a widening multiplication.


### CRC

Opcodes: 0x23C-0x23D

Operands: For Opcode 0x23C, 3. For Opcode 0x23D, 2.

hbits: For opcode 0x23C, `[000t]` where `t` is the nth bit of the second operand with width `n`. For opcode 0x23D, set to `[0001]`

Operand Constraints: The first operand must be a register or a memory reference, and at most one operand may be a memory operand. The third operand may not be a memory operand.

Exceptions:
Exceptions:
- UND if cpuex2.HACCEL is `0`
- UND if cr0.HACCEL is `0`
- UND if any operand constraint is violated
- PF, if a memory operand accesses an unavailable virtual memory address
- PF, if the first operand is a memory operand, and page protections are violated by the access
- PF, if paging is disabled, and a memory operand accesses an out of range physical address

Instructions:
- 0x23C (`crcaccum`): Computes the reminder of each byte of the second operand modulo the polynomial given in the third operand, and xors the result with the first operand shifted right by 8, storing the result in the first operand.
- 0x23D (`crc32accum`): Same as 0x23C, but uses the 32-bit polynomial 0x04C11DB7 (with an implicit top bit) instead of one given in a third operand.

The decoding of the polynomial is MSB major order, and the bit following the most significant bit of the operand is set to `t`. If the polynomial operand is a short immediate, a width of 12 bits is used, and the 13th bit is set to `t`. For the purpose of evaluating opcode 0x23D, `t` is treated as though it is set, and the 3rd operand is treated as an immediate of size 4. 
If the second operand is a short immediate with no bits more significant than bit 7 set, the operand is treated as though it has size 1. Otherwise, it is treated as though t has size 2.