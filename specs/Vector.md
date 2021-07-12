# Clever Vector Extensions

This extension documents the SIMD/Vector Extensions to the Base Clever Processor. These extensions allow for the processing of multiple different values simultaneously with a single instruction.

A CPU indicates conformance with these extensions by setting bit 11 (VEC) of cpuex2. A CPU that does not support these extensions shall have that bit set to `0`. 

## Supervisor Support

If a CPU indicates support for these extensions by setting cpuex2.VEC, a supervisor may indicate support by setting bit 8 (VEC) of cr0. Setting the bit enables all of the features described herein, otherwise, all of them are unavailable.

## Interaction with Floating-point extensions

This extension interacts heavily with features described in the Floating-Point extension. 
It is assumed, though not mandated, within this document that CPUs that comply with the vector extensions described herein also comply with the floating-point extensions. 
The floating-point instructions, floating-point registers and cr0.FPEN all refer to the same defined within the floating-point extensions.

## Vector Registers

These vector extensions define an additional class of register, and, in doing so, expand the register space from 64 to 128. Registers 64-127 are called vector registers. Every even numbered vector register is indicated as `v`*`n`*, with successive values of `n` starting from `0`, and every odd numbered vector register is indicated as `v`*`n*``h`. Vector regtisters have register class `VectorHalf`, and can be used in any instruction that expects a general purpose registers or as floating-point registers. VectorHalf registers are simple and are accessible only if `cr0.VEC=1`. 

## Changes to Operand Structure

### Register Operands

The structure of the direct register operand is changed to the following: `[000 yyy ss x rrrrrrr]`. The meaning of each indicated group of bits is the same as the original structure for the Main specification.

The notable changes are as follows: The third most significant bit (previously) reserved, is changed to be a control bit for register operands. `r` is expanded to be 7 bits to accomidate accessing vector registers as normal. 

### Vector Register Operands

A new class of operand, modifying the Register operand structure is defined for use in vectorized instructions. The strucutre of vector register operands is the following `[001 y ssss xx 1 rrrrr]`

Where: `y` and `xx` are reserved for future use, `ssss` is the extended size control value=`log2(size)`, and `1rrrrr` is the vector register number discarding the least significant bit. 
Vector Register Operands address pairs of vector registers, according to `rrrrr`: For example, rrrrr=00000 accesses `v0` and `v0h`. Non vector registers may not be address using this operand type, and 

`ssss` may be at most `4`, though future extensions may permit values greater than 4.

Except where stated, vector register operands may only be used with the `vmov` instruction, and instructions with the `vec` prefix.

### Long Immediate Values

The Long Immediate value structure is changed as follows: `ss` may have the value `3` (corresponding to a size of 16). 

An `ss` value of 3 may only be used with the `vmov` instruction, may not be pcrelative, and may not be a memory reference

## Instructions

## Vectorize Scalar Instructions

Opcode: 0x400
Operands: One Instruction followed by it's opernads
h: `[00 ss]`, where `ss` is the size of each value operated on by the instruction.

Operand Constraints: At least one operand for the instruction following the prefix shall be a vector operand. For opcode 0x200, both operands shall be vector registers. For opcodes 0x201-0x202, the first two operands shall be vector registers and the third shall be an immediate value with size equal to the size control of both vector registers (An immediate value with size 16 is permitted)

Exceptions:
- UND, if `cr0.VEC`=0.
- UND, if the instruction following the prefix is invalid
- UND, if an operand constraint is violated
- Any exception that occurs while evaluating the intermediate instruction.

Instructions:
 - Reads and decodes a full instruction following the prefix, and applies it to each of the vector elements in each specified vector register, and the exact value of each scalar operand.

Flags: Any prefixed instructions that modifies flags sets the M, Z, and P flags according to the whole value in the vector result. The C, and V flags are not modified by the vectorized instruction (Note: This is a defect. Later versions will provide a mechanism for determining when individual operations in a vectorized operation)

 The following instructions may have a `vec` prefix:
 - Opcodes 0x001-0x007
 - Opcodes 0x008-0x00b
 - Opcodes 0x020-0x027
 - Opcodes 0x030-0x036
 - Opcodes 0x038-0x03e
 - Opcodes 0x200-0x202
 - Opcodes 0x100-0x123
 - Opcodes 0x125-0x129

Opcodes 0x008-0x00b and 0x020-0x027 operate on the provided scalar value, moving it into each vector element. If the destination is a vector register, with a size less than 16, only the elements matched by that size are affected by the move. 
Opcodes 0x200-0x202 ignore the element sizes in the vec prefix size control. 
See `vmov` for moving values between vectors

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

## Bulk Vector Register Storage

Opcodes: 0x402-0x405
Operands: Opcodes 0x402 and 0x404, 1. Opcodes 0x403 and 0x405, 0.
h: Unused and shall be 0

Operand Constraints: 0x402 and 0x404, the operand shall be an indirect register or a memory reference.

Exceptions:
- UND, if `cr0.VEC=0`
- UND, if any operand constraint is violated
- PF, if any memory reference is invalid or an out-of-range virtual address
- PF, if an access violation occurs
- PF, if a memory reference is an invalid physical address.

Operations:
- 0x402 (stovr): Stores each vector register pair in 512 bytes of consecutive storage starting at the memory reference given by the operand
- 0x403 (pushvr): Pushes each vector register pair in reverse order to the stack.
- 0x404 (resvr): Restores each vector register pair from the 512 bytes of consecutive storage starting at the memory reference given by the operand.
- 0x405 (popvr): Pops each vector register pair from the stack.

Each operation is relatively atomic with other memory operations (Note: in particular, `pushvr` and `popvr` only perform one memory operation)


