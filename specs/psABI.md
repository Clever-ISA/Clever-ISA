# Recommended psABI for Clever

## Required Extensions

A CPU running code generated under this ABI shall support the floating-point extensions, unless the types, `_Float16`, `float`, `double`, `long double` are not used.


## Type Layouts

### Byte Length and Character Types

A byte (the fundamental unit, and size of `char`, `unsigned char`, and `signed char`) shall be 8 bits.

The type `char` shall be unsigned. 

### Primitive Type Sizes

The sizes of types `short`, `int`, `long`, and `long long` shall be 2, 4, 8, and 8 respectively. The sizes of types `unsigned short`, `unsigned int`, `unsigned long`, and `unsigned long long` shall be the same as the signed counterpart thereof.

The sizes of types `float` and `double` shall be 4 and 8. `long double` shall be the same as `double` for the purposes of this ABI.

The size of any pointer type shall be 8. 

### Primitive Type Alignment

The alignment of each primitive type (`char`, `signed char`, `short`, `int`, `long`, `long long`, `float`, `double`, `long double`, unsigned counterparts thereof, and pointer types) shall all be equal to the size of the type


### Enumeration Base Type

The Base/Underlying type for enums declared in C, and unscoped enums declared in C++ without a fixed underlying type, shall be `int`. 

### Type Aliases

- `size_t` shall be an alias of type `unsigned long`. `ptrdiff_t` shall be an alias of type `long`.
- `int8_t`, `int16_t`, `int32_t`, and `int64_t` shall be aliases of types `signed char`, `short`, `int`, and `long long` respectively. `uintN_t` shall be an alias of the unsigned version of the type aliased by `intN_t`
- `int_leastN_t` and `int_fastN_t` shall both be alaises of the type aliased by `intN_t`. `uint_leastN_t` and `uint_fastN_t` shall both be aliases of the type aliased by `uintN_t`.
- `uintptr_t` shall be an alias of type `unsigned long`, `intptr_t` shall be an alias of type `long`.


### Floating-Point Types

`float` shall be represented according to the IEEE 754 standard, as a single-precision binary floating-point value.

`double` and `long double` shall be represented according to the IEEE 754 standard, as a double-precision binary floating-point value.

If the implementation of this ABI defines a type `_Float16` or `__float16`, then it shall be represented according to the IEEE 754 standard, as a half-precision binary floating-point value. 

## Calling Convention

### Registers

The following registers shall be callee saved: `r6`, `r7`, `r12`, `r13`, `r14`, `r15`, `f6`, `f7`, `v32` through `v63h`. 

All other registers are caller saved.

A callee saved register must be stored by any function that makes use of it and restored before returning to the caller. No such requirement is imposed upon caller saved registers. 

### Calling Convention

#### Type Classification

For the purposes of determining how to pass a type to a function, or return it from a function, it is necessary to classify the type. Classification occurs as defined below.

##### Scalar Types

The classification of scalar types is as follows:
* All integer types have class INTEGER.
* All pointer types have class INTEGER.
* All floating-point types have class FLOAT, except that an extended 128-bit binary floating-point type or 8-bit binary floating-point type shall have class INTEGER. 
* Enumeration types have class INTEGER.

##### Struct/Union Types

The classification of a structure type is as follows:
* If the type, defined in C++, is non-trivial for the purposes of calls according to the relevant C++ ABI, the structure type has class MEMORY.
* If the struct has an extended-alignment requirement, then the structure type has class MEMORY.
* Otherwise, If the struct has exactly one member with class FLOAT, and no other members except for zero-width bitfields, and empty base classes that are not overaligned types, the type has class FLOAT.
* Otherwise, If the struct is empty, the type has class INTEGER.
* Otherwise, if the type has at least one member with class INTEGER, and no members with class MEMORY, then the type has class INTEGER.
* Otherwise, the type has class MEMORY.

The classification of a union type is as follows:
* If the type, defined in C++, is non-trivial for the purposes of calls according to the relevant C++ ABI, the union type has class MEMORY.
* If the union has an extended-alignment requirement or any field is an overaligned type, then the union type has class MEMORY.
* Otherwise, if the union has at least one member with class FLOAT, and no members of class INTEGER or class MEMORY, then the type has class FLOAT.
* Otherwise, if the union is empty, the type has class INTEGER.
* Otherwise, if the type has at least one member with class INTEGER, and no members with class MEMORY, then the type has class INTEGER.
* Otherwise, the type has class MEMORY.

#### Parameters

The first 4 parameters with class FLOAT are passed in registers, `f0`, `f1`, `f2`, and `f3`. If the type is 2, 4, or 8 bytes in size, then it shall be accessed with a size control corresponding to that size and the upper bits of the register are undefined. Remaining parameters with class FLOAT are treated as parameters with class INTEGER. 

Paramaters with class INTEGER or remaining parameters with class FLOAT are modified as follows to be passed in.
- If the type is less than 16 bytes in size, it shall be zero-extended to the next power of two size if it is not already a power of two.
- If the modified type is 16 bytes in size, it shall be split into a parameter pair of class INTEGER with size 8.
- Otherwise, the parameter is treated as though it has class MEMORY.

The first 8 modified parameters shall be passed in the registers `r2`, `r1`, `r3`, `r4`, `r5`, `r6`, `r7`, and `r9`. If the 8th parameter is the first paramater in a split parameter pair, it shall instead be as the last value on the stack. Remaining parameters are pushed to the stack from right to left. The any bits in the parameter that exceed the size of the type shall be zero.

Parameters with class MEMORY, or parameters with class INTEGER or class FLOAT exceeding 16 bytes in size are replaced by a pointer to the value in some well-aligned memory. This memory shall not be read by the caller after the function returns (but may be written to and reused). The pointer is then substituted for the parameter and passed as normal.

#### Return Values

Return values with class FLOAT with size 2, 4, or 8 are returned in `f0`. The value of unused bits is not defined

Return values with class INTEGER with size up to 8 are returned in `r0`. Unused bits shall be zero.

Return values with class MEMORY, or class INTEGER with size greater than 8 are returned as follows:
- The caller shall place a pointer to well-algined memory to construct the return value into in `r0`.
- The callee shall construct the return value into the pointer in `r0`, and shall return this pointer in `r0`.