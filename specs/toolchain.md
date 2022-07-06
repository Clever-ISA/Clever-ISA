# Toolchain Recommendations

## Target names

The architecture for the toolchain name should be `clever`, or clever followed by the publication version. 
Toolchains should not generate code for an unpublished version of the clever ISA specification.

The default vendor name for targets should be `unknown`, toolchains may accept target names for the clever processor with any vendor.

## Target Features

Each extension should be given a corresponding target feature name. The names for the following target names are given as follows, including the version of the clever ISA targeted where it should start being enabled by default:

|  Extension    | Feature | Default Since |
|---------------|---------|---------------|
| Vector        | vector  | Disabled      |
| Floating-Point| float   | 1.0 or earlier|
| Floating-Point Extra| float-extra | Disabled |


Toolchains should provide mechanisms for disabling default-enabled target features and for enabling default-disabled target features. 
If the `float` feature is disabled for during code generation, the Proposed psABI technical document does not specify the ABI of parameters and return values of class FLOAT.


## Intrinsics

Toolchains that provide such a mechanism and the relevant types (or equivalent) should make the following intrinsic functions available

### Main Instruction Set

#### Flags Register

`unsigned long long __rdflags()`  

Reads the contents of the `flags` register

`__wrflags8(uint8_t flags)`  
`__wrflags16(uint16_t flags)`  
`__wrflags32(uint32_t flags)`  
`__wrflags64(uint64_t flags)`  

Writes to the lower 8, 16, 32, or 64 bits of the `flags` register. The value is not zero-extended or truncated.

#### CPU Information Access

Requires Supervisor mode or the corresponding bit be set in `ciread`

`void __rdcpuid(unsigned long long out[2])`  

Reads the cpuidlo and cpuidhi register into `out[0]` and `out[1]` respectively

`unsigned long __rdcpuex2()`  
`unsigned long __rdcpuex3()`  
`unsigned long __rdcpuex4()`  
`unsigned long __rdcpuex5()`  
`unsigned long __rdcpuex6()`  
`unsigned long __rdmscpuex()`  

Reads the Appropriate cpu information register

#### Supervisor Instructions

Requires Supervisor Mode

`void __xms_pcfl(void* __physical v)`

Flush the page cache corresponding to the given physical address

`void __xms_flall(void)`  
`void __xms_dflush(void)`  
`void __xms_iflush(void)`  

Flush all (data, instruction, and page) caches, the data cache only, or the instruction cache only

`uint8_t __xms_in8(uint64_t port)`  
`uint16_t __xms_in16(uint64_t port)`  
`uint32_t __xms_in32(uint64_t port)`  
`uint64_t __xms_in64(uint64_t port)`  

Reads 8, 16, 32, or 64 bits from the I/O port with address `port`.

`void __xms_out8(uint64_t port,uint8_t val)`  
`void __xms_out16(uint64_t port,uint16_t val)`  
`void __xms_out32(uint64_t port, uint32_t val)`  
`void __xms_out64(uint64_t port,uint64_t val)`  

Writes 8, 16, 32, or 64 bits to the I/O port with address `port`.

#### Register Storage

`void __store_gpr(unsigned long long out[16])`  
`void __load_gpr(const unsigned long long out[16])`  
`void __store_ar(unsigned long long out[128])`  
`void __load_ar(const unsigned long long out[128])`  
`void __store_regf(unsigned long long out[256]`  

Stores or loads the general purpose registers, user registers, or entire register file to/from the memory designated by `out`. 

`__store_regf` requires supervisor.

### Floating Point Operations

All intrinsics in this section requires `X-float`. `_Float16` includes `__float16` or another equivalent if the toolchain defines that type.

`unsigned long long __fp_rdfpcw()`

Reads the floating-point control word.

`void __fp_wrfpcw(unsigned long long word)`

Writes the floating-point control word

`uint16_t __fp_cvtf16_fx16(_Float16 f)`  
`uint32_t __fp_cvtf16_fx32(_Float16 f)`  
`uint64_t __fp_cvtf16_fx64(_Float16 f)`  

Converts a binary16 floating-point value to 16-bit (8.8), 32-bit (16.16), or 64-bit (32.32) fixed point.

`uint32_t __fp_cvtf32_fx32(float f)`  
`uint64_t __fp_cvtf32_fx64(float f)`

Converts a binary32 floating-point value into a 32-bit (16.16) or 64-bit (32.32) fixed point.

`uint64_t __fp_cvtf64_fx64(double f)`

Converts a binary64 floating-point value into a 64-bit (32.32) fixed-point. 
This function may also be called with a signature that accepts `long double`.

`_Float16 __fp_fma16(_Float16 a, _Float16 b, _Float16 c)`  
`float __fp_fma32(float a, float b, float c)`  
`double __fp_fma64(double a, double b, double c)`  

Computes `a*b+c` with infinite intermediate precision using the `ffma` instruction.

`void __fp_raiseexcept(void)`
`void __fp_triggerexcept(void)`

Raises any pending unmasked floating-point exceptions with `fraiseexcept`/`ftriggerexcept`.

In the case of `__fp_triggerexcept`, the effect of the EMASKALL bit in `fpcw` is ignored.

### Floating-point Extra Functions

All intrinsics in this section requires `X-float-extra`. `_Float16` includes `__float16` or another equivalent if the toolchain defines that type.


`_Float16 __fpx_expf16(_Float16 f)`  
`_Float16 __fpx_lnf16(_Float16 f)`  
`_Float16 __fpx_lgf16(_Float16 f)`  
`_Float16 __fpx_sinf16(_Float16 f)`  
`_Float16 __fpx_cosf16(_Float16 f)`  
`_Float16 __fpx_tanf16(_Float16 f)`  
`_Float16 __fpx_asinf16(_Float16 f)`  
`_Float16 __fpx_acosf16(_Float16 f)`  
`_Float16 __fpx_atanf16(_Float16 f)`  
`_Float16 __fpx_exp2f16(_Float16 f)`  
`_Float16 __fpx_log10f16(_Float16 f)`  
`_Float16 __fpx_lnp1f16(_Float16 f)`  
`_Float16 __fpx_expm1f16(_Float16 f)`  
`_Float16 __fpx_sqrtf16(_Float16 f)`  
`float __fpx_expf32(float f)`  
`float __fpx_lnf32(float f)`  
`float __fpx_lgf32(float f)`  
`float __fpx_sinf32(float f)`  
`float __fpx_cosf32(float f)`  
`float __fpx_tanf32(float f)`  
`float __fpx_asinf32(float f)`  
`float __fpx_acosf32(float f)`  
`float __fpx_atanf32(float f)`  
`float __fpx_exp2f32(float f)`  
`float __fpx_log10f32(float f)`  
`float __fpx_lnp1f32(float f)`  
`float __fpx_expm1f32(float f)`  
`float __fpx_sqrtf32(float f)`  
`double __fpx_expf64(double f)`  
`double __fpx_lnf64(double f)`  
`double __fpx_lgf64(double f)`  
`double __fpx_sinf64(double f)`  
`double __fpx_cosf64(double f)`  
`double __fpx_tanf64(double f)`  
`double __fpx_asinf64(double f)`  
`double __fpx_acosf64(double f)`  
`double __fpx_atanf64(double f)`  
`double __fpx_exp2f64(double f)`  
`double __fpx_log10f64(double f)`  
`double __fpx_lnp1f64(double f)`  
`double __fpx_expm1f64(double f)`  
`double __fpx_sqrtf64(double f)`  

Computes the respective function on `f`, which is either a `_Float16`, `float`, or, `double`.

Signatures with `double` may be called with a signature using `long double` in either or both positions.

Each function in this section corresponds with the appropriate instruction X-float-extra applied to the appropriate size floating-point register.

### Vector Intrinsics

All intrinsics in this section require `X-vector`

The toolchain should define the type `__v128` if it defines any intrinsic in this section.

### Vector Manipulations

`__vec128 __vec_load_int128(__int128 a);`

Loads a 128-bit integer into a vector register. Only defined if the toolchain supports `__int128`

`__vec128 __vec_load_int64(long long lo, long long hi);`

Loads two 64-bit integers into a vector register. `lo` is moved into the low vector half, and `hi` into the high vector half.

`void __vec_load_int64_lo(__vec128* out, long long lo);`  

Loads the 64-bit value in `lo` into the low vector half of `out`. The high vector half of `out` is unmodified.

`void __vec_load_int64_hi(__vec128* out, long long hi);

Loads the 64-bit value in `hi` into the high vector half of `out`. The low vector half of `out` is unmodified.

`__vec128 __vec_load_float128(__float128 a);`

Loads a 128-bit integer into a vector register. Only defined if the toolchain supports `__float128`. 

`__vec128 __vec_load_float64(double lo, double hi);`

Loads two 64-bit integers into a vector register. `lo` is moved into the low vector half, and `hi` into the high vector half.

`void __vec_load_float64_lo(__vec128* out, double lo);`  

Loads the 64-bit value in `lo` into the low vector half of `out`. The high vector half of `out` is unmodified.

`void __vec_load_float64_hi(__vec128* out, double hi);

Loads the 64-bit value in `hi` into the high vector half of `out`. The low vector half of `out` is unmodified.

## Inline ASM

Toolchains that support inline assembly should support Clever-ISA as follows in this section.
The mechanisms under which operands to the expression may be specified are toolchain and language-specific.

### Register Constriants

The following registers should be made available for use with inline assembly for operands

* All general purpose registers other than r6, r7, and r14,
* All floating-point registers,
* All vector half and vector pair registers (named separately).

Where it is not necessary to choose specific registers, the following register groups should be available for constraints:
* general - Each general purpose register
* float - Each floating-point register
* vectorhalf - each vector half register
* vectorhi - each higher-half vector register
* vectorlo - each lower-half vector register
* vector - each vector pair

Types which are accepted by each register group, and registers belonging to each group, should be as follows:
* general - integer, fixed-pointed, and floating-point scalar types, of any standard width up to 64-bit (signed or unsigned)
* float - floating-point scalar types of widths 16, 32, and 64, when implemented as IEEE754 binary floating-point
* vectorhalf, vectorhi, vectorlo - integer, fixed-point, and floating-point scalar and vector types with a total size up to 8 bytes
* vector - integer, fixed-point, and floating-point scalar and vector types with a total size up to 16 bytes

At the option of the toolchain, access to the Z, C, V, M, and P flags may be used to pass boolean types or integer types with a width of 1 bit. If so, the `flag` register group may name these flags.

### Preserved State

Toolchains may assume that the following state is preserved, unless it is indicated in a toolchain-specific manner, that it is not. In the alternative, the toolchain may provide a mechanism to indicate that it is:
* Each flag in the `flags` register,
* The exception flags of the `fpcw` register

Additionally, toolchains may assume that the following is preserved, and is not requiured to provide a mechanism to indicate it is not preserved:
* The mode register
* The frame pointer (r6) and stack pointer (r7)
* The rounding mode and extended operation precision size fields of the `fpcw` register

Toolchains may further assume the following is preserved, but may, at its option, provide a mechanism to indicate that it is clobbered:
* The fast call return address register (r14)

Beyond this, the toolchain may assume that the value of any register that is not mentioned as an output or that is not clobbered in a toolchain-specific manner is preserved by the assembly expression.