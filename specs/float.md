# Floating Point Operations

This specification describes required extensions to the Clever ISA to support hardware-level floating-point operations. 



## License

Copyright (c)  2021  Connor Horman.
Permission is granted to copy, distribute and/or modify this document
under the terms of the GNU Free Documentation License, Version 1.3
or any later version published by the Free Software Foundation;
with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
A copy of the license is included in the repository, under the file entitled LICENSE. Otherwise, you may find a copy at <https://www.gnu.org/licenses/fdl-1.3.en.html>.

## General

## CPU Support

A CPU indicates conformance with these extensions by setting bit 9 (FP) of cpuex2. A CPU that does not support these extensions shall have that bit set to `0`. 

## Supervisor Support

If a cpu reports conformance to these extensions by setting cpuex2.FP, then a supervisor may indicate support for these extensions by setting bit 6 (FPEN) of cr0. If a supervisor additionally supports handling Floating-Point Exceptions, may set bit 7 (FPEXCPT), to handle floating-point errors in accordance with [Exceptions](#Exceptions).

## Registers

The following previously reserved registers are defined as part of this extension for the following uses:

| Number   | Register Name | Availability   |
|----------|---------------|----------------|
| 19       | fpcrw         | FPFLAGS        |
| 24       | f0            | FP             |
| 25       | f1            | FP             |
| 26       | f2            | FP             |
| 27       | f3            | FP             |
| 28       | f4            | FP             |
| 29       | f5            | FP             |
| 30       | f6            | FP             |
| 31       | f7            | FP             |

Registers listed with FP availability are floating-point registers. They may be accessed in any mode when cr0.FPEN=1. Register 19 (fpcrw) may be accessed when cr0.FPEN=1. 

Bit 17 (FPEN) of register 17 (flags) shall be set to 1 when cr0.FPEN=1, and 0 otherwise. This bit cannot be modified by software.

If Opcodes 0x001-0x007 have an operand that's a floating-point register, UND is raised.

`mov`, `test`, `cmp`, `cmpxchg`, and `xchg` may all have floating-point register operands. If any of these instructions do, they operate on the value bitwise. In particular, `cmp` may be used to totally order floating-point values.

`fpcrw` shall consist of the following control word for floating-point operations

| Bit   | Name     | Description                                              |
|-------|----------|----------------------------------------------------------|
| 0-2   | RND      | The current rounding mode for floating point operations. |
| 4     | DENORM   | Denormal Values enabled when set, otherwise underflow to +/-0.0|
| 8-15  | EXCEPT   | The current floating point exceptions that are asserted. |
| 16-23 | EMASK    | The exceptions that are masked (will not trigger a hardware exception)|
| 24    | EMASKALL | If set, all exceptions are masked                        |
| 25-26 | XOPSS    | The upper-bound for extended precision                   |

The value of fpcrw.RND is given as follows:
| Mode | Description                    |
|------|--------------------------------|
| 0    | Towards ∞ (rounding up)        |
| 1    | Towards -∞ (rounding down)     |
| 2    | Towards 0  (truncation)        |
| 3    | Half Up                        |
| 4    | Half To Even                   |

The bits in fpcrw.EXCEPT and fpcw.EMASK are set corresponding to the following numbered floating-point exceptions (IE. if the 0th exception is raised, then bit 8 of fpcrw is set, and if the 5th exception is masked, then bit 19 of fpcrw is set):
| Exception | Name      | Description                      |
|-----------|-----------|----------------------------------|
| 0         | INVALID   | Invalid/Undefined Operation      |
| 1         | DIVBYZERO | Division by +/-0.0               |
| 2         | OVERFLOW  | Operation Overflowed to +/-Inf   |
| 3         | UNDERFLOW | Operation Underflowed to +/-0.0  |
| 4         | INEXACT   | Operation produced inexact result|
| 5         | SIGNAL    | An operation was performed on an sNaN |


XOPSS shall be the `ss`-1 value which the maximum upper-bound extended precision for floating-point operations performed in hardware. The value `3` is reserved for future use. Performing a floating-point operation with this value set triggers UND.

The values in floating-point registers are stored bitwise. An instruction that does not store a value in a floating-point register shall not modify the representation of the value. Operations that store bitwise values in floating-point registers shall not modify the representation of the value (for example, using `mov` to store a signalling NaN value in a floating-point register does not quiet or canonicalize the NaN representation). 

### Startup

During startup, the fpcw is set as follows:
* RND is set to 4
* DENORM is set
* All exceptions are not asserted
* All exception bits in EMASK are set
* EMASKALL is set
* XOPSS is 0
* All other bits are zero

All FP registers have undefined values during startup.

## Instructions

### Converting Moves

Opcodes: 0x022-0x026
Operands: 2

h: For opcodes 0x022,0x24, and 0x26 `[00 0f]`where if `f` is set `flags` is not modified.  For opcodes 0x023 and 0x025, `[ss i f]` where `ss` corresponds to the number of fractional (`i` clear) or integer (`i` set) bits in the fixed point format.

Operand Constraints: For opcode 0x022-0x023, the destination register shall be a floating-point register. For opcodes 0x024-0x025, the source register shall be a floating-point register. For opcodes 0x026, at least one operand shall be a floating-point register, and neither operand shall be a direct register other than a floating-point register.

Flags: `M` and `Z` are set according to the result, unless `f` is set in `h`.

Exceptions:
- UND, if a operand constraint is violated
- UND, if cr0.FPEN=0
- UND, if any destination operand is flags or ip
- PROT, if a supervisor register is an operand, and `flags.XM=1`.
- UND, if a reserved register is an operand.
- UND, if a floating-point operand has size 1
- UND, if fpcrw.EOPSS=3
- PF, if the target address is an unavailable virtual memory address
- PF, if page protections are violated by the access
- PF, if paging is disable, and the target address is an out of range physical address
- PROT, if the target address is out of range for the PTL mode.
- PF, if a memory operand accesses an unavailable virtual memory address
- PROT, If opcodes 0x01b or 0x01f are used to load a value other than 0 into a reserved or unavailable register.
- PROT, if any operation results in an unmasked floating point exception and cr0.FPEXCEPT is clear
- FPE, if any operation results in an unmasked floating point exception, and cr0.FPEXCEPT is set

Floating Point Exceptions:
- OVERFLOW: If an operand to opcode 0x022 or 0x023 is an integer or fixed-point value that is larger than the maximum value for the destination format
- INEXACT: If an operand to opcode 0x022 or 0x023 is an integer or fixed-point value that cannot be exactly represented in the destination format
- OVERFLOW: If an operand to opcode 0x024 or 0x025 is too large for the destination size, or is infinite.
- OVERFLOW: If an operand to opcode 0x026 is too large for the destiniation format.
- UNDERFLOW: If an operand to opcode 0x026 is too large for the destination format.
- INEXACT: If an operand to opcode 0x026 cannot be exactly represented in the destination format.
- SIGNAL: If the operand to opcode 0x024 or 0x025 is an sNaN

Instructions:
- 0x022 (movsif): Moves a signed integer value from the second operand into the first, converting it into a floating-point value.
- 0x023 (movxf): Moves a fixed-point value from the second operand into the first, converting it into a fixed-point value.
- 0x024 (movfsi): Moves a floating point value from the second operand into the first, truncating it into a signed integer value.
- 0x025 (movfx): Moves a floating point value from the second operand into the first, converting it into a fixed-point value
- 0x026 (cvtf): Moves a floating-point value from the second operand into the first, converting it to the size of the destination operand.

floating-point moves use the size of the floating-point operand to determine the format. 
`size`=1 is not valid for any such operand. 
For `cvtf` the size of both operands are used, and the value is converted from the source format to the destination format.
For all operations, IEEE754 binary floating-point format is used for the size of the operand.

For fixed point operands (`movxf` and `movfx`), the format is determined by dividing the integer operand width in bits by `exp(3-ss)`. If `i` is set, this becomes the number of leading integer bits, and the remaining portion is the fractional part. Otherwise, this becomes the number of trailing fractional bits, and the remaining portion is the integer part. 

All memory accesses are performed atomically. Note that the entire operation is not required to be atomic.

Overflow caused by `movfsi` and `movfx` yields the maximum (for positive inputs) or minimum (for negative inputs) value of the type. Underflow yields 0. If any operand is a NaN, the result is the minimum value of the type.  
If the input for `movxf`, `movsif`, `movfx`, or `cvtf` cannot be exactly represented, the value is rounded according to `fpcw.RND`, and `INEXACT` is raised.  
If the input for `movfsi` cannot be exactly represented, the value is truncated (rounded towards zero).  INEXACT is not raised when integer truncation occurs.
If the input to `movxf`, `movsif`, or `cvtf` is out of range of the destination format, then OVERFLOW is raised, and the result is +/- Infinity. 
If the input to `movxf` or `cvtf` is too small for the destination format, then UNDERFLOW is raised, and the result is +/-0. 

If the input to `cvtf` is a NaN, then the result is an unspecified NaN of the same type. This occurs even when both operands are of the same format.

### Floating-point Arithmetic

Opcodes: 0x100-0x10f,
Operands: Opcodes 0x100-0x105, 1; Opcodes 0x106-0x10a, 2; Opcode 0x10b, 3.

h: All other opcodes, `[00 0f]` where if `f` is set, `flags` is not modified.

Operand Constraints: The first operand shall be a floating-point register, or a memory reference. 
At least one operand shall be a floating-point register. No operand shall be a direct register, other than a floating-point register.

Exceptions:
- UND, if a operand constraint is violated
- UND, if cr0.FPEN=0.
- If `d` is set in h, UND, if cpuex2.DFP=0.
- UND, if a floating-point operand has size 1
- UND, if fpcrw.EOPSS=3
- PF, if the target address is an unavailable virtual memory address
- PF, if page protections are violated by the access
- PF, if paging is disable, and the target address is an out of range physical address
- PROT, if the target address is out of range for the PTL mode.
- PF, if a memory operand accesses an unavailable virtual memory address
- PROT, if any operation results in an unmasked floating point exception and cr0.FPEXCEPT is clear
- FPE, if any operation results in an unmasked floating point exception, and cr0.FPEXCEPT is set

Floating Point Exceptions:
- INVALID: If an operand to an instruction is outside of it's domain
- OVERFLOW: If an operation produces a value that is too large for the destination format
- UNDERFLOW: If an operation produces a value that is too small for the destination format
- INEXACT: If an operation with a rational result produces a value that cannot be exactly represented in the destiniation format.
- INEXACT: If the operation is performed with greater precision than the destination format, and the result cannot be exactly represented in the destiniation format
- UNDERFLOW: If the result would be a subnormal value and fpcrw.DENORM is clear.


Flags: If `f` is not set in `h`, `N`, `Z`, and `P` are set according to the result. 

Instructions:

- 0x100 (round): Rounds the operand half-up and stores the result in the operand
- 0x101 (ceil): Rounds the operand up and stores the result in the operand
- 0x102 (floor): Rounds the operand down and stores the result in the operand
- 0x103 (fabs): Computes the absolute value of the operand, and stores the result in the operand.
- 0x104 (fneg): Negates the operand, and stores the result in the operand
- 0x105 (finv): Calculates the multiplicative inverse of the operand
- 0x106 (fadd): Adds the second operand to the first.
- 0x107 (fsub): Subtracts the second operand operand from the first.
- 0x108 (fmul): Multiplies the first operand by the second.
- 0x109 (fdiv): Divides the first operand by the second
- 0x10a (frem): Computes the remainder of the first operand, divided by the second, and stores the result in the first.
- 0x10b (fma): Multiplies the first operand by the second, and adds the third as though with infinite intermediate precision, storing the result in the first.

If any operand is a NaN, the result is a qNaN. This does not cause INVALID exceptions to occur.

All floating-point operations described within this section shall be performed to within 0.5 ULPs of the exact value, as determined by the result format.

Handling of the Z flag for floating-point operations is on the logical value of the operation, rather than the bitwise value (like for integer operations). Both + and - 0.0 in results set the Z flag. -0.0 in results also sets the `N` flag.

If either operand is NaN, the "sign" of the result is the same as the sign of the NaN. If both operands are NaN, the sign is negative if either sign is negative. 


In computing the result, the size of the operand shall specify the minimum range and precision. If there is more than one operand, the largest operand specifies the minimum range and precision. If XOPSS is larger than the operand size, than the maximum range and precision for computing the result is given by XOPSS. A processor is not required to compute the result in the range and precision given by XOPSS. 
The result is then converted to the destiniation format and stored in the destination operand. 
If the result is larger than the maximum value of destination format, then OVERFLOW is triggered. If the result is smaller than the minimum value of the destination format, then UNDERFLOW is triggered. If the result cannot be exactly represented in the destination format, then INEXACT is triggered.

If the result is smaller than the minimum normal value of the format, then the result is a denormal value of fpcrw.DENORM is set. Otherwise, UNDERFLOW is triggered and the result is +/-0.0.

If an operation produces a NaN result, it is unspecified what NaN is produced, except that any operation on a qNaN or sNaN shall produce an qNaN. 

The meaning of NaN representations is unspecified, except that the "canonical" values with only the most significant bit in the mantissa set with either sign, shall be quiet.

### Floating-point comparions

Opcodes: 0x118-0x119

Operands: For opcode 0x118, 1. For Opcode 0x119, 2.


Operand Contraints: No operand shall be have size 1. 
 For opcode 0x11e, the operand shall not be a register, other than a floating-point register. For opcode 0x124, at least one operand must be a floating-point register or a long immediate value and no operand shall be a register, other than a floating-point register. 

h: 

Flags: M, Z, C, and P are set according to the result. V is not modified.

Floating-point Exceptions:
- SIGNAL, if any operand is an sNaN

Exceptions:
- UND, if cr0.FPEN=0
- UND, if an operand constraint is violated
- UND, if fpcrw.XOPSS=3
- PF, if the target address is an unavailable virtual memory address
- PF, if page protections are violated by the access
- PF, if paging is disable, and the target address is an out of range physical address
- PROT, if the target address is out of range for the PTL mode.
- PF, if a memory operand accesses an unavailable virtual memory address
- FPE, if cr0.FPEXCEPT=1 and an unmasked floating-point exception occurs
- PROT, if cr0.FPEXCEPT=0 and an unmasked floating-point exception occurs


Instructions:
- 0x118 (fcmpz): Performs partial-order floating-point comparison with the operand and +0.0
- 0x119 (fcmp): Performs partial-order floating-point comparison with both operands

For fcmpz, +0.0 is the first operand of the comparison.

### Trigger Previously Masked Floating-point Exceptions

Opcodes: 0x130-0x131

Operands: None

h: Reserved and shall be zero

Exceptions:
- UND, if cr0.FPEN=0
- UND, if any undefined exception bit is set in fpcrw.EXCEPT
- PROT, if any exception bit set in fpcrw.EXCEPT is not masked, and cr0.FPEXCEPT=0
- FPE, if any exception bit set in fpcrw.EXCEPT is not masked, and cr0.FPEXCEPT=1

Instructions:
- 0x130 (fraiseexcept): Triggers an FPE if any unmasked floating-point exception is asserted
- 0x131 (ftriggerexcept): Triggers and FPE if any unmasked floating-point except is asserted, ignoring the value of fpcrw.EMASKALL in determining masking.



## Floating Point Exceptions

Exception 6 (FPE) shall be raised when a floating-point operation causes an unmasked floating-point exception and cr0.FPEXCEPT=1. 

If cr0.FPEXCEPT=0, then PROT shall be raised instead of FPE.

A floating-point exception is masked if fpcrw.EMASKALL is set or the corresponding bit in fpcrw.EMASK is set. Regardless of whether an exception is masked, the corresponding bit in fpcrw.EXCEPT is set when an instruction causes that exception. No instruction, other than an explicit write to fpcrw, clears any bit in fpcrw.EXCEPT.

## Stability

The listing of floating-point exceptions generated by instructions is considered stable.
