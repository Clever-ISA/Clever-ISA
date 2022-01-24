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