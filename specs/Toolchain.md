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
If the fp feature is disabled for during code generation, the Proposed psABI technical document does not specify the ABI of parameters and return values of class FLOAT.
