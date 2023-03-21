# Atomic RMW Exchange Instructions

This extension defines a function that 

## Conformance

A CPU indicates conformance with these extensions by setting bit 15 (ATOMICXCHG) of cpuex2. A CPU that does not support these extensions shall have that bit set to `0`.


## Supervisor Support

If cpuex2.ATOMICXCHG is set by the CPU, the supervisor may set bit 12 (ATOMICXCHG) to enable the instructions in this extension.

## Instructions

### Atomic XCHG

Opcode: 0x204

h bits: `[0ooo]` where `o` is the opcode applied by the operation

Operands: 2

Operand Constraints: The first operand must be a memory reference, and the second operand must be a register.

Exceptions:
- UND, if cr0.ATOMICXCHG is clear 
- UND, if any operand constraint is violated
- UND, if `o` is `0`, `6`, or `7`
- PF,  if paging is enabled, and paging permisions are violated by the access
- PF, if paging is enabled, and a memory operand has an invalid virtual address
- PF, if paging is disabled, and a memory operand is an out of range physical address

Instruction:
- 0x204 (`axchg`): Atomically performs the read-modify-write with a modify operation given by the opcode in `o`, storing the original value of the first operand in the second operand.

The entire read-modify-write memory operation is totally atomic. 