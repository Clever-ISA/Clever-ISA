# Toolchain Recommendations

## Target names

The architecture for the toolchain name should be `clever`, or clever followed by the publication version. 
Toolchains should not generate code for an unpublished version of the clever ISA specification.

The vdefault vendor name for targets should be `unknown`

## Target Features

Each extension should be given a correponding target feature name. The names for the following target names are given as follows, including the version of the clever ISA targetted where it should start being enabled by default:

|  Extension    | Feature | Default Since |
|---------------|---------|---------------|
| Vector        | vec     | Disabled      |
| Floating-Point| fp      | 1.0 or earlier|


Toolchains should provide mechanisms for disabling default-enabled target features and for enabling default-disbaled target features.