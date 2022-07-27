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

### Immediate Operands

Immediate Operands (other than memory references) are indicated by literal integer values. 
Additionally, when the name of a symbol (not defined by the assembler as a literal constant) is used, it may be prefixed by the `value` prefix to be used as an immediate value.

To indicate an `ip` relative value, add the immediate to the register name `ip`. `ip` may preceed or trail the constant value, e.g. `5+ip` or `ip+5`. Negative values may be used: e.g. `-5+ip` or `ip-5`. 


When encoding an immediate operand without an explicit size control keyword, the smallest immediate size that encodes the value should be used. When symbol values are used, 8 byte encodes must be used by the assembler unless it can resolve the reference, but the assembler may emit link relaxations for smaller immediate encodings
Negative values are encoded using 8 bytes for non-`ip` relative immediates, for any instruction other than `movsx` (opcode 0x020).


If an explicit size control is used, then the constant must be encoded using that control. The `byte` control is illegal for immediates. 
The assembler should check that the value is within range for an unsigned (non-relative positive immediate) or signed (relative, negative immediate, or immediate operand of `movsx`) value of that size.



### Memory References

Memory References are encoded by an immediate value enclosed in square brackets, or a symbol name (optionally enclosed in square brackets) without the `value` prefix, optionally added to `ip`. 

Any size prefix other than `short` may be applied before the symbol name (if no square brackets are used) or outside the square brackets to control the operand size. Additionally, a size prefix other than `byte` or `short` may be applied before any value within the square brackets to indicate the memory reference size.

If no size prefix for the operand size is used, the size of the other operand is used if it exists, otherwise the assembler should refuse to assemble the program. If no size prefix for the address is used, the memory reference should be encoded the same as for immediate operands.

### Symbol Relocations

An explicit relocation may be indicated using the `@` suffix, followed by the name of the relocation given by the psABI, without the `R_CLEVER_` prefix. 
If no explicit relocation is used, the relocation for the explicit size should be used, with PCREL relocations used when immediates or memory references are encoded with the `ip` prefix. 

When no explicit size or relocation is used, the assembler may emit linker relaxations for the operand, in addition to emitting relocations.

### Operand Order

Operands should be given in the order they are emitted.

e.g. for `mov cr0, 0x1000`, first the operand for `cr0` should be generated, then the operand for `0x1000`. 

## Instruction Mnemonics

Base instruction mnemonics are the ones given in the instruction overviews.
When more than one instruction is given the same mnemonic, then when encoding such an instruction, the instruction with the lowest numbered opcode eligible for the number of operands given, except that GPR Specializations should be considered before considering non-specialized instructions, and instructions with GPR destinations should be considered before instructions with GPR sources.

The branch mnemonics, and `repcc` instruction should be indicated as with the prefix (`j` for branching, or `rep` for `repcc`), followed by the lowercase one or two character condition name.

Examples:
- `mov cr0, 0x1000` Should be encoded with opcode 0x008
- `mov r0, 0x1000` should be encoded with opcode 0x00A
- `mov cr0, r1` should be encoded with opcode 0x00B
- `mov r0, r1` should be encoded with opcode 0x00A


For single bit h flags, such as `l`, `f`, or `w`, these should be indicated by the presense of the character after a `.` following the mnemonic.

Example: `add.l` sets the `l` bit in `h`, while `add` does not.

For branch weight hints, the keywords `likely` or `l`, and `unlikely` or `u` can be used to indicate a branch weight of `7`, or `-8` respectively. Additionally, an explicit integer between -8 and 7 inclusive may be given. Both suffixes follow a `.`.

Example: `jeq.l` encodes a branch weight of `7`, and `jne.-6` encodes a branch weight of -6

For the vec prefix, the element size value in bits should be used following the `.` suffix
Examples: `vec.8`, `vec.16`, etc.

## Condition Codes

The condition codes for conditional branches, and the repcc instruction are as follows, including Aliases:

| Canonical Condition Name | Canonical Mnemonic | Alternative Mnemonics |
|==========================|====================|=======================|
| Parity | p  | po (Parity Odd) |
| Carry  | c  | b (Below) |
| Overflow|v  | | 
| Zero   | z  | eq (Equal) | 
| Less Than| lt | | 
| Less or Eq | le | |
| Below or Eq | be | |
| Minus | mi | n (Negative), sn (Sign Negative) |
| Plus  | pl | nn (Non-negative), sp (Sign Positive) |
| Above | a  | |
| Greater | gt | |
| Greater or Equal | ge | |
| Not Zero | nz | ne (Not Equal) |
| No Overflow | nv |  |
| No Carry  | nc | ae (Above or Equal) |
| No parity | np | pe (Parity Even) |