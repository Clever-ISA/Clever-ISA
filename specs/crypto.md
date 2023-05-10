# Hash/Pseudorandom Function Hardware Acceleration

This extension defines a number of operations used to accelerate the execution of Cryptographic Hash and Cipher algorithms

## Conformance

A CPU indicates conformance with these extensions by setting bit 16 (CRYTPO) of cpuex2. A CPU that does not support these extensions shall have that bit set to `0`.


## Supervisor Support

If cpuex2.HACCEL is set by the CPU, the supervisor may set bit 14 (CRYPTO) to enable the instructions in this extension.

## Required Additional Extensios

A CPU that supports these extensions must also support the X-vector extension

## Operands

### Changes to Vector Register Operands

An operand that refers to a vector register or a memory reference may have size specifiers 5 and 6, for the instructions in this section, and for opcode 0x401 (`vmov`) defined in the X-vector extension.
Other uses of such operands shall UND.

If a vector register has size specifier 5 (corresponding to 256-bit or size 32) or 6 (corresponding to 512-bit or size 64), then 4 or 8 vector half registers starting from the base register are used. 
The register number of the base register must be a multiple of 4 or 8 respectively, or use of the operand shall UND. 

## Instructions

### SHA2 

### SHA2 Round

Opcode: 0x300

h bits: `[000 s]` where `s` is the size - 0 for 32-bit SHA2 and 1 for 64-bit SHA2.

Operands: 3

Operand constraints: The first operand shall be a vector register of size 32 if `s` is clear, or size 64 if `s` is set. The second operand shall not be vector register or an immediate/memory reference with size greater than 8. The third operand shall be a register or an immediate, except for a vector register, or an immediate of size 16.


Exceptions:
- UND, if `cpuex2.CRYPTO` is 0
- UND, if `cr0.CRYPTO` is 0
- UND, if any operand constraint is violated
- PF, if paging is enabled, and page protections are violated
- PF, if paging is enabled, and a memory reference refers to a non-present page
- PF, if paging is not enabled, and a memory reference refers to an out-of-range, or invalid physical address
- PROT, if `s` is clear, and the second operand has size 64.
- PROT, if the third operand exceeds the maximum round count for the algorithm (64 if `s` is clear, and 80 if `s` is set)

Instructions:
- `sha2round`: Computes the round function for SHA256 (or SHA224) or SHA512 (or SHA384), depending on the value of `s`, using the state stored in the first operand, the current message schedule element in the second, and the round number in the third operand.
