# Hardware Random Bit Source

## License

Copyright (c)  2021  Connor Horman.
Permission is granted to copy, distribute and/or modify this document
under the terms of the GNU Free Documentation License, Version 1.3
or any later version published by the Free Software Foundation;
with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
A copy of the license is included in the repository, under the file entitled LICENSE. Otherwise, you may find a copy at <https://www.gnu.org/licenses/fdl-1.3.en.html>.

## Conformance

A CPU indicates conformance with these extensions by setting bit 8 (RAND) of cpuex2. A CPU that does not support these extensions shall have that bit set to `0`.

Additionally, a CPU that reliably supplies cryptographically strong non-deterministic random bits through the interfaces specified in this extension, should set bit 13 (NDRAND) of cpuex2. A CPU that does not reliably supply non-determinstic random bits shall have that bit set to `0`.

## Supervisor Support

If cpuex2.RAND is set by the CPU, the supervisor may set bit 9 (XMRAND) to enable the unprivileged `rpoll`instruction in program execution mode. 
Additionally, the supervisor may set bit 10 (RPOLLINFO) to allow programs to recieve information about the state of the generator when polling fails

## Registers

The following registers are available in supervisor mode when cpuex2.RAND is set by the CPU (otherwise, accessing the registers triggers UND):
| Register | Number |
|----------|--------|
| randinfo | 156    |

`randinfo` is a read-only register that stores the result of an `rpoll` instruction that failed. It shall not be modified when an `rpoll` instruction succeeds. Writes to the register triggers UND.


## Instructions

### Random Bit Generator Polling

Opcode: 0x230

Operands: 0

h: `[rrrr]` where `r` is a General Purpose Register `0<=r<16`

Exceptions:
- UND if cpuex2.RAND=0
- PROT if mode.XM=1, and cr0.XMRAND=0
- PROT if mode.XM=1, cr0.RPOLLINFO=0, and polling the random bit generator fails

Flags: Sets `P` if polling the random bit generator fails, otherwise clears `P`. Note that `P` is set reguardless of whether an exception occurs

Instructions:
- 0x230 (rpoll): Attempts to poll the hardware random bit generator, storing 64-bits of entropy in the general purpose register specified in `h`. If generation fails, and information reporting is available (See Below), then `r` is loaded with information about the generator, including available enthropy.

The hardware random bit generator (which is referred to as the "random device") may either be a cryptographically strong psuedorandom bit generator (Described by [Requirements on Pseudorandom Devices](#requirements-on-pseudorandom-devices)) or a cryptographically strong nondeterministic random bit generator (Described by [Requirements on Nondeterministic Devices](#requirements-on-nondetermistic-devices)). 
When cpuex2.NDRAND is set by the CPU, the `rpoll` instruction shall be implemented using a nondeterminstic random bit generator.

Information Reporting is available in program execution mode only when the supervisor sets `cr0.RPOLLINFO`. Otherwise, failures will set the register to `0`, and raise `PROT` to be handled by the supervisor (`P` is still modified in this mode). Reguardless of `cr0.RPOLLINFO`, information reporting is always available in supervisor mode. The last result from a poll failure is stored available in the `randinfo` register for the supervisor.


The result stored in the destination register when polling fails is the following bitfield:
| Bit | Name | Description |
|-----|------|-------------|
| 0-16| ENTROPY | Contains the amount of entropy the implementation can prove is available to the generator, multiplied by 2^16|
| 17-18 | FAIL | The cause of the failure (see below)|
| 19 | REPEAT | Whether the program can repeat the request immediately |

All other bits have undefined values (Note: An implementation should not set any undefined bits).

the value of FAIL is set as follows
| Value | Name | Description |
|-------|------|-------------|
| 0     | UNAVAIL | Insufficient Entropy is available |
| 1     | RESET   | Device reset requested |
| 2     | FAULT   | Device faulted |
| 3     | PAUSE   | Device Paused |

FAULT shall be set whenever the device has a fault the implementation is unable to correct. After the random device reports a FAIL code of FAULT, all subsequent operations until a processor RESET will continue to report FAULT. REPEAT is never set for this FAIL code.

RESET and PAUSE are used for correctable error conditions. 
PAUSE shall only be reported when the issue can be corrected by the device automatically (REPEAT should be set for this failure mode if and only if the device determines it can correct itself immediately). 
RESET is reported when supervisor intervention is required to reset the device. The mechanism for correcting a RESET fault is not specified and is machine specific. REPEAT should not be set for this FAIL code.
After the device reports a FAIL Code of RESET, it should continue to do so until the device is reset, except that it may change to FAULT if it determines the error to be unrecoverable after first discovering the error, and may change to PAUSE or normal operation if it becomes able to correct the error.


UNAVAIL indicates that the random device cannot prove sufficient entropy is available to service the request. REPEAT should be set only if the device determines that sufficient entropy can be made available in a reasonable period of time. 

When any particular failure code, other than UNAVAIL, is indicated, is unspecified. 

#### Constraints on Execution

To ensure security of the device and output, the following constraints are placed on machines implementing this extension:
1. By the time any interrupt occurs on a CPU, the CPU shall have determined whether preceeding `rpoll` operation succeeded or fail, and, if the poll succeed, storing the result and reflecting the operation in the random device (e.g. by advancing a pseudorandom device, or depleting the entropy pool of a nondeterministic random device)
2. By the time a control flow transfer occurs, the results of any preceeding successful `rpoll` operation shall be stored and reflected in the source. This shall include the speculative execution of conditional branch instructions (even conditional branches that are not taken or are predicted to not be taken).
3. The results of rpoll instructions executed on the current cpu may be reordered, but may not be interleaved, and no rpoll instruction shall observe the same successful result as another rpoll instruction.
4. Different CPUs should not share a random device. 
5. Regardless of whether different CPUs share a random device, an rpoll instruction on one shall determine, and store (as necessary), its result such that no rpoll instruction executed on a different CPU observes the same result
6. If the result of a successful rpoll instruction is produced, even speculatively, the operation shall be reflected in the random device


#### Requirements on Pseudorandom Devices

A Pseudorandom device shall be cryptographically strong and secure.
An implementation shall guarantee a period of at least 2^128 on average.

Additionally, the implementation shall ensure that an attack to determine the sequence of future outputs from the psuedorandom device given some sequence of past outputs, is at least as computationally intensive as for a 256-bit key for a symmetric cryptographic algorithm.

Initialization of the pseudorandom device is unspecified, but the implementation shall ensure that it is unlikely (with at least 2^-128 probability) for two pseudorandom devices to be initialized in the same state, or for the same psuedorandom device, initialized at different times, to be initialized in the same state.

#### Requirements on Nondeterministic Devices

A nondeterministic random device shall be cryptographically strong and secure.
It shall guarantee that, with 4 (potentially nonconsecutive) invocations of the generator, producing 256 total bits, transformed by a proper cryptographic hash algorithm (Such as SHA2 (256-bit or wider) or SHA3), produces at least 128-bits of full entropy.

If the random device cannot satisfy the guarantees given in this section for any given poll, it shall return a failed result with a code of UNAVAIL until it is able to do so. 
