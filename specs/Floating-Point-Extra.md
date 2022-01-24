# Floating Point Extra Operations

This specification describes optional extensions to the Clever ISA to support hardware-level extended floating-point operations, such as trigonometric functions.



## License

Copyright (c)  2021  Connor Horman.
Permission is granted to copy, distribute and/or modify this document
under the terms of the GNU Free Documentation License, Version 1.3
or any later version published by the Free Software Foundation;
with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
A copy of the license is included in the repository, under the file entitled LICENSE. Otherwise, you may find a copy at <https://www.gnu.org/licenses/fdl-1.3.en.html>.

## General

## CPU Support

A CPU indicates conformance with these extensions by setting bit 10 (FP) of cpuex2. A CPU that does not support these extensions shall have that bit set to `0`. 

A CPU that conforms with these extensions shall also conform to the Floating-Point Operations extension (X-float).

## Supervisor Support

Supervisor support for these extensions are indicated the same way as for the Floating-Point Operations extension

## Instructions

### Floating-point Functions

Opcodes: 0x100-0x10f, 0x113-0x115, 0x119-0x11a, 0x125, 0x126, 0x129
Operands: Opcodes 0x100-0x11F, 1. Opcodes 0x120-0x127, 2. Opcodes 0x128-0x129, 3.

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
- 0x113 (log10): Computes log10(x) (the base 10 logarithm) of the operand, and stores the result in the operand
- 0x114 (lnp1): Computes ln(x+1) of the operand, and stores the result in the operand
- 0x115 (expm1): Computes exp(x-1) of the operand, and stores the result in the operand.
- 0x119 (sqrt): Computes sqrt(x) of the operand, and stores the result in the operand.
- 0x11a (cbrt): Computes cbrt(x) of the operand, and stores the result in the operand.
- 0x125 (hypot): Computes the hypotenuse of the two operands and stores the result in the first.
- 0x126 (atan2): Computes the arc tangent of the first operand, divided by the second, but use the sign of both operands to determine quadrent.
- 0x129 (hypot3): Computes the hypotenuse of the three operands and stores the result in the first.