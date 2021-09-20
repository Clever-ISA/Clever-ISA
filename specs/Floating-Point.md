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
| 8-13  | EXCEPT   | The current floating point exceptions that have occured. |
| 14-19 | EMASK    | The exceptions that are masked                           |


The value of fpcw.RND is given as follows:
| Mode | Description                    |
|------|--------------------------------|
| 0    | Towards ∞ (rounding up)        |
| 1    | Towards -∞ (rounding down)     |
| 2    | Towards 0  (truncation)        |
| 3    | Half Up                        |
| 4    | Half To Even                   |

The bits in fpcw.EXCEPT and fpcw.EMASK are set corresponding to the following numbered floating-point exceptions (IE. if the 0th exception is raised, then bit 8 of fpcw is set, and if the 6th exception is masked, then bit 19 of fpcw is set):
| Exception | Name    | Description                      |
|-----------|---------|----------------------------------|
| 0         | INVALID | Invalid/Undefined Operation      |
| 1         | 



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

Instructions:
- 0x022 (movsif): Moves a signed integer value from the second operand into the first, converting it into a floating-point value.
- 0x023 (movxf): Moves a fixed-point value from the second operand into the first, converting it into a fixed-point value.
- 0x024 (movfsi): Moves a floating point value from the second operand into the first, truncating it into a signed integer value.
- 0x024 (movfx): Moves a floating point value from the second operand into the first, converting it into a fixed-point value
- 0x026 (cvtf): Moves a floating-point value from the second operand into the first, converting it to the size of the destination operand.

floating-point moves use the size of the floating-point operand to determine the format. 
`size`=1 is not valid for any such operand. 
For `movf` and `movfd`, the size of both operands are used, and the value is converted from the source format to the destination format.
For all operations, IEEE754 binary floating-point format is used for the size of the operand.

Fixed point operands (`movxf` and `movfx`) use the higher half of the value for the integer portion, and the lower half for fractional portion. 

All memory accesses are performed atomically. Note that the entire operation is not required to be atomic.

### Floating-point Arithmetic

Opcodes: 0x100-0x129
Operands: Opcodes 0x100-0x11F, 1. Opcodes 0x120-0x127, 2. Opcodes 0x128-0x129, 3.

h: Opcodes 0x11e and 0x124 all bits shall be zero. All other opcodes, `[00 0f]` where if `f` is set, `flags` is not modified.

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
- PROT, if quiet floating-point errors are not enabled, cr0.FPEXCEPT=0, and an exception would otherwise raise FPE.
- FPE, if quiet floating point errors are not enabled, and any operation causes an IEEE 754 error
- FPE, if quiet floating point errors are not enable, and an operand is an sNaN.

Flags: If `f` is not set in `h`, `M` and `Z` are set according to the result. Opcodes 0x11e and 0x124, `C` and `V` are also set according to the result.

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
- 0x11e (fcmpz): Compares the floating point operand with 0, and sets flags according to the comparison. 
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

All floating-point operations described within this section shall be performed to within 0.5 ULPs of the exact value. No other requirements are imposed on instructions in this section.

fcmp (0x124) and fcmpz (0x) sets flags as follows: 
If any operand 
If the first operand is less than the other, sets `V` to `N`, otherwise sets `V` to `~N`.
If the second operand is equal to the other, sets `Z`, otherwise clears `Z`.



## Exceptions

Exception 6 (FPE) shall be raised when a floating-point operation causes a floating-point exception or is performed on a signalling NaN value, while flags.FPQ=0, and cr0.FPEXCEPT=1. The error shall be present in flags.FPEXCPT.

If cr0.FPEXCEPT=1, then PROT shall be raised instead of FPE.
