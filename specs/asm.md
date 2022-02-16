# Assembly Syntax Recommendations

The following document indicates recommended syntax for assembly

## Operand Syntax

#### Explicit size control

Any operand may be prefixed by an explicit size control keyword "byte" for 1 byte values,
"half" for 2 byte values, "single" or "word" for 4 byte values, "double" for 8 byte values, and "vector" for 16 byte values.
Additionally, for short immediate (12-bit immediate) operand, the `short` or `simm` keyword can be used. 
This only applies to immediate operands that are not memory references.

### Register Operands

Register operands should be encoded as the name of the register (e.g. `r0`)

To use smaller sizes, use register suffixes, `b` for byte, `h` for half word, and `s` for single word (e.g. `r0b`, `r0h`, or `r0s`). Alternatively, use the general size controls `byte`, `half`, `single`, and `double`.
[Note: A word here is 32-bits. The choice was made so that suffixes could be consistent with floating-point registers]

For Vector register operands, place the `b`, or `s` suffix in place of `l` or `h`. Additionally, to use a vector register with a 64-bit width, use the `d` (double) suffix. Note that because `v`*n*`h` refers to the higher half of the vector registers, the `half` size control must be used instead of the `h` suffix to use a vector register (`half v0`). 

### Indirect Register Operands

Indirect Register operands should be encoded as the name of the register, plus the scaled displacement or index register name, placed inside square brackets. 

Explicit size control operations

Examples: `[r0]`, `[r0+r1]`, `[r7+16*r2]`.

### 



