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


