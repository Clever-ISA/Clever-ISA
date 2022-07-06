# Clever Vector Extensions

This extension documents the SIMD/Vector Extensions to the Base Clever Processor. These extensions allow for the processing of multiple different values simultaneously with a single instruction.

## License

Copyright (c)  2021  Connor Horman.
Permission is granted to copy, distribute and/or modify this document
under the terms of the GNU Free Documentation License, Version 1.3
or any later version published by the Free Software Foundation;
with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
A copy of the license is included in the repository, under the file entitled LICENSE. Otherwise, you may find a copy at <https://www.gnu.org/licenses/fdl-1.3.en.html>.

## Conformance

A CPU indicates conformance with these extensions by setting bit 11 (VEC) of cpuex2. A CPU that does not support these extensions shall have that bit set to `0`. 

## Supervisor Support


If a CPU indicates support for these extensions by setting cpuex2.VEC, a supervisor may indicate support by setting bit 8 (VEC) of cr0. Setting the bit enables all of the features described herein, otherwise, all of them are unavailable.

## Interaction with Floating-point extensions

This extension interacts heavily with features described in the Floating-Point extension. 
It is assumed within this document, though not mandated, that CPUs that comply with the vector extensions described herein also comply with the floating-point extensions. 
The floating-point instructions, floating-point registers and cr0.FPEN all refer to the same defined within the floating-point extensions.

## Vector Registers

These vector extensions define an additional class of register. Registers 64-95 are called vector registers. Every even numbered vector register is indicated as `v`*`n`*`l`, with successive values of `n` starting from `0`, and every odd numbered vector register is indicated as `v`*`n`*`h`. A vector pair, labeled `v`*`n`*, consists of the 64-bits from `v`*`n`*`l`, followed by the 64-bits from `v`*`n`*`h`.
Vector registers have register class `VectorHalf`, and can be used in any instruction that expects a general purpose registers or as floating-point registers. 
For the purposes of determining operand contraints, a VectorHalf register may be used in place of a floating-point or general purpose register.

## Changes to Operand Structure

### Register Operands

The structure of the direct register operand is changed to the following: `[000 yyy ss rrrrrrrr]`. The meaning of each indicated group of bits is the same as the original structure for the Main specification.

The notable changes are as follows: The third most significant bit (previously) reserved, is changed to be a control bit for register operands. 

### Vector Register Operands

A new class of operand, modifying the Register operand structure is defined for use in vectorized instructions. The structure of vector register operands is the following `[001 y ssss rrrrrrrr]`

Where: `y` is reserved for future use, `ssss` is the extended size control value=`log2(size)`, and `1rrrrr` is the vector register number discarding the least significant bit. 
Vector Register Operands address pairs of vector registers, according to `rrrrrrrr`: For example, rrrrrrrr=0 accesses `v0l` and `v0h` (together called `v0`). Non vector registers may not be address using this operand type, and the least significant bit shall be set to `0`

`ssss` may be at most `4`, though future extensions may permit values greater than 4.

Except where stated, vector register operands may only be used with the `vmov` instruction, and instructions with the `vec` prefix.

### Long Immediate Values

The Long Immediate value structure is changed as follows: The structure shall be `[11 m yy r ss zzzz yyyy]`; `ss` may have the value `3` (corresponding to a size of 16).

An `ss` value of 3 may only be used with the `vmov` instruction, may not be pcrelative, and may not be a memory reference. Memory references for a `vmov` instruction use `zzzz` as the extended size control value. Such operands shall have a size of at most `16` (zzzz=4).



## Instructions

## Vectorize Scalar Instructions

Opcode: 0x400
Operands: One Instruction followed by it's operands
h: `[00 ss]`, where `ss` is the size of each vector element.

Operand Constraints: At least one operand for the instruction following the prefix shall be a vector operand. 

Exceptions:
- UND, if `cr0.VEC`=0.
- UND, if the instruction following the prefix is invalid or undefined
- UND, if any vector operand has a size less than the element size
- UND, if an operand constraint is violated
- Any exception that occurs while evaluating the intermediate instruction.

Instructions:
 - Reads and decodes a full instruction following the prefix, and applies it to each of the vector elements in each specified vector register, and the exact value of each scalar operand.

Flags: 
If the destination is not a vector operand, flags are set according to the instruction and it's result.
If the destiniation is a vector operand, other than v31, then the flags are set in the elements (corresponding to the element size control) of v31, and the overall Z and P flags are stored in `flags`.


 The following instructions may have a `vec` prefix:
 - Opcodes 0x001-0x005
 - Opcodes 0x008-0x00b
 - Opcodes 0x020-0x027
 - Opcodes 0x030-0x036
 - Opcodes 0x038-0x03e
 - Opcodes 0x100-0x123
 - Opcodes 0x125-0x129

Opcodes 0x008-0x00b and 0x020-0x027 operate on the provided scalar value, moving it into each vector element. 

If the destination is a vector register, with a size less than 16, only the elements matched by that size are affected by the operation. If
See `vmov` for moving values between vectors.

If any source operand is not a vector operand, then the value is copied for each element of the vector (given by the vector register size control and the vector element size), then truncated or zero extended to the vector element size. The Resulting values are used by each operation performed under the vec prefix. For memory operands, memory is accessed exactly once (and, in all cases, the access is atomic).
If the destination operand (if any) is not a vector operand, then the operation is performed as a reduction. The operation is performed for each element of the vector, with that operand as the destination, and each vector element as the second operand (if there are two or more source operands, each are separated or copied as defined). The order in which the operations are performed is unspecified and may be non-deterministic. This may have suprising results if the instruction performs an operation that is not reorderable (such as floating-point operations, or `mov`). The intermediate values of the operation are not stored in the destination until each operation is performed fully (In particular, intermediate values are not truncated or zero-extended and do not undergo floating-point size conversion).

[Note:
As a trivial example, it is unspecified whether `r0` contains 1, 2, 3, or 4 after the following code:
```
    vmov v0, 0x00000001000000020000000300000004
    vec.i32 mov r0, v0
```
]

If any operation performed under a vec prefix causes a floating-point exception, all other operations are completed, and the entire value is stored if the exception is masked (otherwise, no register or memory is modified), and all floating-point exceptions from all operations are indicated in `fpcw`, and then either `FP` or `PROT` occurs if any exception is unmasked. All other exception cases are checked *before* performing any operations.



## Vector Move

Opcodes: 0x401
Operands: 2 
h: Unused and shall be 0

Operand Constraints: No operand may be a Direct Register, other than a Vector Register, or an immediate with a size other than 16 that is not a memory reference. At least one operand shall be a vector register or an immediate value, and the first operand may not be an immediate value

Exceptions:
- UND, if `cr0.VEC=0`
- UND, if any operand constraint is violated
- PF, if any memory reference is invalid or is an out-of-range virtual address
- PF, if an access violation occurs
- PF, if any memory reference is an invalid physical address

Instructions:
- 0x401 (vmov): Moves a 16-byte value into the destination vector operand

## Vector Permutations and Bit Selection 

Opcodes: 0x402-0x403
Operands: 2
h: Opcode 0x402, [00 ss], where `ss` is the element size. Opcode 0x403, `[rrrr]`, where `rrrr` is the general purpose register for the destination operand.

Operand Constraints: No operand may be a memory reference. The first operand must be a vector register, and the second operand must not be a vector register or immediate of size 16. For opcode 0x403, the second operand must be an immediate,

Exceptions:
- UND, if `cr0.VEC=0`
- UND, if any operand constraint is violated
- PROT, for opcode 0x403, if the BASE or COUNT exceeds the size of the vector element.
- PROT, for opcode 0x403, if the total number of elements, multiplied by COUNT, is more than 64-bits. 


Instructions:
- 0x402 (vshuffle): Shuffles the elements of the first operand according to the permutation given in the second operand.
- 0x403 (vextract): Extract bits from vector elements into general purpose register according to a selection mask given in the second operand

For `vshuffle`, The permutation is made up of 2-8 4-bit location references. 
The nth 4-bit reference from the Least significant bit refers to the nth element from the least significant element of the destiniation vector. Additional references are ignored. The value in the reference is the element number (from the least significant) in the source vector. 
Each location has high order bits masked off according to the total number of elements in the vector.

For `vextract`, the selection mask is given as follows:

| Bits | Name | Description |
| 0-5  | BASE | The lowest order bit selected from each element by the instruction, from 0-63|
| 7-9 | COUNT | The number of bits contiguously selected from each element, starting from BASE, minus 1|
| 10-11 | ELEMSS | The size control, `log2(size)`, indicating the size of the elements |


