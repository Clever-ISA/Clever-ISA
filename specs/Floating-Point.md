# Floating Point Operations

This specification describes required extensions to the Clever ISA to support hardware-level floating-point operations. 

A CPU indicates conformance with these extensions by setting bit 9 (FP) of cpuex2. A CPU that does not support these extensions shall have that bit set to `0`. 

## License

Copyright (c)  2021  Connor Horman.
Permission is granted to copy, distribute and/or modify this document
under the terms of the GNU Free Documentation License, Version 1.3
or any later version published by the Free Software Foundation;
with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
A copy of the license is included in the repository, under the file entitled LICENSE. Otherwise, you may find a copy at <https://www.gnu.org/licenses/fdl-1.3.en.html>.


## Supervisor Support

If a cpu reports conformance to these extensions by setting cpuex2.FP, then a supervisor may indicate support for these extensions by setting bit 6 (FPEN) of cr0. If a supervisor additionally supports handling Floating-Point Exceptions, may set bit 7 (FPEXCPT), to handle floating-point errors in accordance with [Exceptions](#Exceptions).

## Registers

The following previously reserved registers are defined as part of this extension for the following uses:

| Number   | Register Name | Availability   |
|----------|---------------|----------------|
| 19       | fpcw          | FPFLAGS        |
| 24       | f0            | FP             |
| 25       | f1            | FP             |
| 26       | f2            | FP             |
| 27       | f3            | FP             |
| 28       | f4            | FP             |
| 29       | f5            | FP             |
| 30       | f6            | FP             |
| 31       | f7            | FP             |

Registers listed with FP availability are floating-point registers. They may be accessed in any mode when cr0.FPEN=1. Register 19 (fpcw) may be accessed when cr0.FPEN=1. 

Bit 17 (FPEN) of register 17 (flags) shall be set to 1 when cr0.FPEN=1, and 0 otherwise. This bit cannot be modified by software.

If Opcodes 0x001-0x007 have an operand that's a floating-point register, UND is raised.

`mov`, `test`, `cmp`, `cmpxchg`, and `xchg` may all have floating-point register operands. If any of these instructions do, they operate on the value bitwise. In particular, `cmp` may be used to totally order floating-point values.

`fpcw` shall consist of the following control word for floating-point operations

| Bit   | Name     | Description                                              |
|-------|----------|----------------------------------------------------------|
| 0-2   | RND      | The current rounding mode for floating point operations. |
| 4     | DENORM   | Denormal Values enabled when set, otherwise underflow to +/-0.0|
| 8-13  | EXCEPT   | The current floating point exceptions that are asserted. |
| 14-19 | EMASK    | The exceptions that are masked (will not trigger a hardware exception)|
| 20-21 | EOPSS    | The upper-bound for extended precision                   |

The value of fpcw.RND is given as follows:
| Mode | Description                    |
|------|--------------------------------|
| 0    | Towards ∞ (rounding up)        |
| 1    | Towards -∞ (rounding down)     |
| 2    | Towards 0  (truncation)        |
| 3    | Half Up                        |
| 4    | Half To Even                   |

The bits in fpcw.EXCEPT and fpcw.EMASK are set corresponding to the following numbered floating-point exceptions (IE. if the 0th exception is raised, then bit 8 of fpcw is set, and if the 5th exception is masked, then bit 19 of fpcw is set):
| Exception | Name      | Description                      |
|-----------|-----------|----------------------------------|
| 0         | INVALID   | Invalid/Undefined Operation      |
| 1         | DIVBYZERO | Division by +/-0.0               |
| 2         | OVERFLOW  | Operation Overflowed to +/-Inf   |
| 3         | UNDERFLOW | Operation Underflowed to +/-0.0  |
| 4         | INEXACT   | Operation produced inexact result|
| 5         | SIGNAL    | An operation was performed on an sNaN |


### Startup

During startup, the fpcw is set as follows:
* RND is set to 4
* DENORM is set
* All exceptions are not asserted
* All exceptions are masked
* All other bits are zero

All FP registers have undefined values during startup.

## Instructions

### Converting Moves

Opcodes: 0x022-0x026
Operands: 2

h: For opcodes 0x022-0x026 `[00 0f]`where if `f` is set `flags` is not modified. 

Operand Constraints: For opcode 0x022-0x023, the destination register shall be a floating-point register. For opcodes 0x024-0x025, the source register shall be a floating-point register. For opcodes 0x026, at least one operand shall be a floating-point register, and neither operand shall be a direct register other than a floating-point register.

Flags: `M` and `Z` are set according to the result, unless `f` is set in `h`.

Exceptions:
- UND, if a operand constraint is violated
- UND, if cr0.FPEN=0
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
For `movf` and `movfd`, the size of both operands are used, and the value is converted from the source format to the destination format.
For all operations, IEEE754 binary floating-point format is used for the size of the operand.

Fixed point operands (`movxf` and `movfx`) use the higher half of the value for the integer portion, and the lower half for fractional portion. 

All memory accesses are performed atomically. Note that the entire operation is not required to be atomic.

Overflow caused by `movfsi` and `movfx` yields the maximum (for positive inputs) or minimum (for negative inputs) value of the type. Underflow yields 0. If any operand is a NaN, the result is the minimum value of the type.

### Floating-point Arithmetic

Opcodes: 0x100-0x129
Operands: Opcodes 0x100-0x11F, 1. Opcodes 0x120-0x127, 2. Opcodes 0x128-0x129, 3.

h: Opcodes 0x11e and 0x124 `[00 g0]`, where if `g` is set, comparisons involving NaN are inverted. All other opcodes, `[00 0f]` where if `f` is set, `flags` is not modified.

Operand Constraints: The first operand shall be a floating-point register, or a memory reference. 
At least one operand shall be a floating-point register. No operand shall be a direct register, other than a floating-point register.

Exceptions:
- UND, if a operand constraint is violated
- UND, if cr0.FPEN=0.
- If `d` is set in h, UND, if cpuex2.DFP=0.
- UND, if a floating-point operand has size 1
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
- INEXACT: If the operation is performed with greater precision 

Flags: If `f` is not set in `h`, `N`, `Z`, and `P` are set according to the result. Opcodes 0x11e and 0x124, `V` is also set according to the result.

Instructions:
- 0x100 (exp): Computes exp(x) of the operand, and stores the result in the operand.
- 0x101 (ln): Computes ln(x) (the natural logarithm) of the operand, and stores the result in the operand
- 0x102 (lg): Computes lg(x) (the base 2 logarithm) of the operand, and stores the result in the operand.
- 0x103 (sin): Computes sin(x) of the operand, and stores the result in the operand.
- 0x104 (cos): Computes cos(x) of the operand, and stores the result in the operand
- 0x105 (tan): Computes tan(x) of the operand, and stores the result in the operand.
- 0x106 (asin): Computes asin(x) of the operand, and stores the result in the operand.
- 0x107 (acos): Computes acos(x) of the operand, and stores the result in the operand.
- 0x108 (atan): Computes atan(x) of the operand, and stores the result in the operand.
- 0x109 (sinh): Computes sinh(x) (the hyperbolic sin) of the operand, and stores the result in the operand.
- 0x10a (cosh): Computes cosh(x) (the hyperbolic cos) of the operand, and stores the result in the operand.
- 0x10b (tanh): Computes tanh(x) (the hyperbolic tan) of the operand, and stores the result in the operand.
- 0x10c (asinh): Computes asinh(x) (the inverse hyperbolic sin) of the operand, and stores the result in the operand
- 0x10d (acosh): Computes acosh(x) (the inverse hyperbolic cos) of the operand, and stores the result in the operand.
- 0x10e (atanh): Computes atanh(x) (the inverse hyperbolic tan) of the operand, and stores the result in the operand.
- 0x10f (exp2): Computes 2^x of the operand, and stores the result in the operand.
- 0x110 (lgamma): Computes ln(Gamma(x)) of the operand, and stores the result in the operand.
- 0x111 (tgamma): Computes Gamma(x) of the operand, and stores the result in the operand.
- 0x112 (erf): Computes the erf(x) of the operand, and stores the result in the operand.
- 0x113 (log10): Computes log10(x) (the base 10 logarithm) of the operand, and stores the result in the operand
- 0x114 (lnp1): Computes ln(x+1) of the operand, and stores the result in the operand
- 0x115 (expm1): Computes exp(x-1) of the operand, and stores the result in the operand.
- 0x116 (round): Rounds the operand half-up and stores the result in the operand
- 0x117 (ceil): Rounds the operand up and stores the result in the operand
- 0x118 (floor): Rounds the operand down and stores the result in the operand
- 0x119 (sqrt): Computes sqrt(x) of the operand, and stores the result in the operand.
- 0x11a (cbrt): Computes cbrt(x) of the operand, and stores the result in the operand.
- 0x11b (fabs): Computes the absolute value of the operand, and stores the result in the operand.
- 0x11c (fneg): Negates the operand, and stores the result in the operand
- 0x11d (finv): Calculates the multiplicative inverse of the operand
- 0x11e (fcmpz): Compares the floating point operand with +0.0, and sets flags according to the comparison. 
- 0x11f (erfc): Computes the 1.0-erf(x) of the operand, without loss of precision, and stores the result in the operand.
- 0x120 (fadd): Adds the second operand to the first.
- 0x121 (fsub): Subtracts the second operand operand from the first.
- 0x122 (fmul): Multiplies the first operand by the second.
- 0x123 (fdiv): Divides the first operand by the second
- 0x124 (fcmp): Compares two floating-point operands and sets flags according to the result
- 0x125 (hypot): Computes the hypotenuse of the two operands and stores the result in the first.
- 0x126 (atan2): Computes the arc tangent of the first operand, divided by the second, but use the sign of both operands to determine quadrent.
- 0x127 (frem): Computes the remainder of the first operand, divided by the second, and stores the result in the first.
- 0x128 (fma): Multiplies the first operand by the second, and adds the third as though with infinite intermediate precision, storing the result in the first.
- 0x129 (hypot3): Computes the hypotenuse of the three operands and stores the result in teh first.

If any operand is a NaN, the result is a qNaN. This does not cause INVALID exceptions to occur.

All floating-point operations described within this section shall be performed to within 0.5 ULPs of the exact value. 

Handling of the Z flag for floating-point operations is on the logical value of the operation, rather than the bitwise value (like for integer operations). Both + and - 0.0 in results set the Z flag.

The `g` bit in `h` affects the value of the `V` flag when comparing NaNs. If any operand to `fcmp` or `fcmpz` is NaN, then `V` is set to the value of `N` if `g` is set, and is set to the inverse of `N` if `g` is clear.

If either operand is NaN, the "sign" of the result is the same as the sign of the NaN. If both operands are NaN, the sign is negative if either sign is negative. 

There is no explicit hardware support for the total comparison predicate, the `cmp` instruction may be used for this purpose. 
Likewise, there is no hardware support for an unordered comparison result - a comparison that returns an optional tridirectional result would need to handle NaNs explicitly.


For instructions with more than 1 operand, the size of the largest operand is used to compute the result, and the result is then converted into the destination format. This may cause OVERFLOW, UNDERFLOW, or INEXACT errors.

If an operation produces a NaN result, it is unspecified what NaN is produced, except that any operation on a qNaN or sNaN shall produce an qNaN. 

The meaning of NaN representations is unspecified, except that the "canonical" values with only the most significant bit in the mantissa set with either sign, shall be quiet.

## Exceptions

Exception 6 (FPE) shall be raised when a floating-point operation causes an unmasked floating-point exception, while flags.FPQ=0, and cr0.FPEXCEPT=1. The error shall be present in flags.FPEXCPT.

If cr0.FPEXCEPT=1, then PROT shall be raised instead of FPE.
