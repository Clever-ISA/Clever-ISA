# Transactional Memory

This Specification describes instructions for performing transactional operations on memory, that cannot be observed to have performed any operation, except as a single indivisible operation.

## License

Copyright (c)  2021  Connor Horman.
Permission is granted to copy, distribute and/or modify this document
under the terms of the GNU Free Documentation License, Version 1.3
or any later version published by the Free Software Foundation;
with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
A copy of the license is included in the repository, under the file entitled LICENSE. Otherwise, you may find a copy at <https://www.gnu.org/licenses/fdl-1.3.en.html>.

## General

## CPU Support

A CPU indicates support for this extension by setting bit 15 of `cpuex2` (`TXMEM`). A CPU that does not support this extension shall set that bit to `0`.

## Supervisor Support

When a CPU sets `cpuex2.TXMEM`, the supervisor may indicate supervisor support for the extension by setting bit 12 of `cr0` (`TXMEM`).


## Supervisor Information

The `cpuex3` register contains information that the supervisor may use about the extension. The defined fields are set to the value prescribed herein, and the remainder of the register is reserved and set to `0`. Supervisors should not rely on any reserved portion of a CPU Extended Feature Register being set to `0`.

| Field  | Bits | Description |
|--------|------|-------------|
| TXGRAN | 0-3  | The minimum granularity of address association within a tranaction, as `lg(size)` in bytes |
| TXSTSZ | 4-7  | The size of the state saved by the `txsuspend` instruction, in multiples of 4096 bytes, minus one |
| TXMAXADDRS | 8-15 | The maximum number of addresses which can be associated with a given transaction before the transaction fails | 

## Instructions

### Load/Store in Transaction

Opcodes: 0x380-0x381

Operands: 1

h field: `[rrrr]` where `r` is the general purpose register to use as the destination (0x380), or source (0x381) operand.

Operand Constraints: The operand shall be a memory reference, except for a memory reference of size 16. 


Exceptions:
- UND, if `cpuex2.TXMEM=0`
- UND, if `cr0.TXMEM=0`
- PROT(0), if a transaction is not currently in progress
- PROT(0), if any memory reference is not aligned to at least the size of the operand
- PF, if paging is enabled, and any memory reference violates paging permissions
- PF, if paging is enabled, and any memory reference is a noncanonical virtual address
- PF, if paging is disabled, any any memory reference is an out of rage physical address

Instructions:
- 0x380 (`txload`): Reads from the memory reference into the register specified in `h`, and associates the memory address referred to with the in-progress transaction. 
- 0x381 (`txstore`): Queues a store of the value of the register specified in `h` into the memory reference, and associates the memory address referred to with the in-progress transaction.

For the purposes of issuing Paging Faults, `txstore` acts as though it writes to the address immediately. 

`txload` and `txstore` must be aligned to the size of the access, otherwise a `PROT` exception occurs. 

A `txload` that reads from an address previously written by `txstore` in the same transaction will read the value written, unless a later store to the same address (with the granularity of `cpuex3.TXGRAN`) overwrites it (note that a concurrent store to the associated address that doesn't overlap may cause a `txload` to read a previous value from memory).

A `txstore` is not visible to any other operation, except for a `txload` witrhin the same transaction, until a successful `txcommit` completes the current transaction. If the transaction fails or is aborted, the `txstore` is not made visible to any subsequent operation.


Every byte accessed by `txload` and `txstore` is associated with the transaction, additionally, if the value of `cpuex3.TXGRAN` is greater than `0`, every well aligned region that contains a byte accessed by a `txload` or `txstore`, and is the size indicated by `cpuex3.TXGRAN` is also associated. A smaller region may be associated if the CPU is capable of doing so.

Subsequent accesses to the same association region do not count towards the limit in `cpuex3.TXMAXADDRS`. For this purpose, the CPU shall act as though each region is the maximum possible size. 


## Commit/Abort Transaction

Opcodes 0x382-0x383

Operands: 0

h field: set to `0`

Exceptions:
- UND, if `cpuex2.TXMEM=0`
- UND, if `cr0.TXMEM=0`
- PROT(0), if a transaction is not currently in progress
- PROT(0), if `mode.XM=1` and `txbegin` was issued with `mode.XM=0`.
- PROT(beginaddr), for opcode 0x382, if any memory address was associated with the transaction, and the value of `cr0.PG` changed since `txbegin` was issued.
- PF, if paging is enabled, and any store performed by the transaction refers to page that is no longer mapped or cannot be accessed.

Flags: For 0x382, `Z` is cleared if the transaction succeeded, and set if the transaction failed. For 0x383, `Z` is cleared unconditionally. All other flags are preserved.

Instructions:
- 0x382 (`txcommit`): Attempts to commit the in progress transaction. If it succeeds, sets `flags.Z` and stores `0` to `r0`. Otherwise, clears `flags.Z` and loads the failure code into `r0`.
- 0x383 (`txabort`): Forcibly aborts the in progress transaction. Clears `flags.Z`. `r0` is not modified.

When a transaction is committed, each store queued by `txstore` is written back if no conflicting accesses occur. If any of these stores violates paging requirements, a `PF` is raised, and the transaction is aborted. In this case, no stores occur. If the transaction would fail, but for the store to the invalid page, it is unspecified whether the `txcommit` instruction raises a `PF` exception, or indicates a failure, but a transaction shall not indicate success if any store queued cannot be completed due to a page fault. Upon a success `txcommit`, all stores queued by the transaction become visible on the current CPU, and all stores become visible to each other CPU at the same time. If the transaction fails, no stores become visible to any CPU.

If multiple transactions perform conflicting operations to the same associated addresses, and each would succeed but for the other transactions, exactly one of those transactions succeeds, and each other transaction fails. If some (but not all) of these transactions would otherwise fail, it's unspecified whether any transaction succeeds.

Two memory operations conflict in a transaction if:
* At least one associates a memory address to a transaction,
* There exists some address, such that each operation either accesses that memory address, or associates that address with the transaction,
* At least one is a store, including a transactional store, and
* Neither operation is visible to the other.

Except that a non-transactional load never conflicts with a transactional store.

A transancation shall fail if:
* A memory operation occured that was not part of a transaction (including one issued on the current CPU) that conflicted with a memory operation that was part of the transaction,
* A memory operation occured on the current CPU that loaded from 
* Another transaction that executed concurrently performed a conflicting operation, except that exactly one such transaction does not fail in this way,
* A privileged `in` or `out` instruction was issued since the beginning of the transaction - I/O operations are not retriable, 
* A number of address regions (of size given by `cpuex3.TXGRAN`) are associated with a given transaction, in excess of the limit given by `cpuex3.TXMAXADDRS`.

Transactions may fail supriously (without reason). It is unspecified when this occurs (Note: When a memory cache is made use of to enable the transaction support, this may occur when cache lines involved in a transaction are evicted or otherwise invalidated). In particular, a transaction *may* fail if any of the following occur:
* The privileged `flall` instruction is issued since the beginning of the transaction,
* The privileged `dflush` instruction is issued with an address that is associated with the transaction,
* For a transaction started when paging is enabled, the physical address mapped to address associated with a transaction changed since the beginning of the transaction.

A successful `txcommit` makes all prior memory accesses visible to all loads that load the value written by any `txstore` executed in the transaction.
Further, a successful `txcommit` makes all memory operations that are visible to the stores each `txload` that is a part of the transaction took the value of any bytes from, visible to all subsequent memory operations on the same CPU.

A transaction may be ended without a `txcommit` in any of the following ways (this is called `abort`ing the transaction):
* Executing the `txabort` instruction,
* Executing an instruction (other than the privileged `reti` instruction) that causes a mode switch - in particular the unprivileged `scall`, `int`, `jsm`, `callsm`, and `retrsm` instructions, and the privileged `scret` instruction,
* Executing an instruction that causes an Exception.

When a transaction is aborted, it acts as though a `txcommit` instruction was issued and committing the transaction failed. 

## Begin Transaction

Opcodes: 0x384-0x385

Operands: 0

h field: set to `0`

Exceptions:
- UND, if `cpuex2.TXMEM=0`
- UND, if `cr0.TXMEM=0`

Flags: `Z` is set if no transaction was in progress, and `Z` is cleared otherwise.

Instruction:
- 0x384 (`txbegin`): Begins a transaction on the current CPU and sets `flags.Z`. If there is already a transaction in progress, cleras `flags.Z` and does not modify the in-progress transaction.
- 0x385 (`txtest`): Tests if a transaction is in progress on the current CPU. Sets or clears `flags.Z` according to the behaviour of `txbegin` but a transaction is *not* started.



## Save/Restore Transaction State

Opcodes: 0xA00-0xA01

Operands: 1

Operand Constraints: The operand must be a memory reference. 

h field: set to `0`

Exceptions:
- UND, if `cpuex2.TXMEM=0`
- UND, if `cr0.TXMEM=0`
- UND, if any operand constraint is violated
- PROT(0), if `mode.XM=1`
- For opcode 0xA00, PROT(0), if a transaction is not currently in progress
- For opcode 0xA01, PROT(0), if a transaction is currently in progress
- For opcode 0xA01, PROT(0), if the format of the restored state area is invalid
- PROT(0), if a memory reference is not 4096 byte aligned.
- PF, if paging is enabled, and any memory reference refers to an invalid virtual address
- PF, if paging is enabled, and paging permissions are violated
- PF, if paging is disabled, and any memory reference refers to an out of range physical address

Flags: None

Instructions:
- 0xA00 (`txsuspend`): Suspends the current in progress transaction, and writes the transaction state to a number of bytes starting from the memory reference, equal to the value specified by `cpuex3.TXSTSZ`. 
- 0xA01 (`txresume`): Reloads the transaction state from the memory reference, saved by a previous `txsuspend` instruction, and resumes the indicated transaction.

The format of the transaction state area is unspecified. How and whether validity is checked is unspecified - invalid state areas will raise `PROT(0)`. If validity checking succeeds, the implementation shall ensure that the processor is placed into a consistent state even if the state area is malformed (in particular, it shall place the processor in a state such that a transaction is in progress), but unless the state refers to one that was previously saved, no further guarantees are made (for example, which addresses are associated, stores pending, visibility effects on `txcommit`, and whether or not committing the transaction can possibly succeed). 


Regardless of the format, the transaction state area is at least 4096 bytes, in multiples of 4096 (the total size of the area). The size of the area is at most `1+cpuex33.TXSTSZ` times 4096 bytes, but less may be written or read per invocation. In particular it may be possiible to write to a memory reference that has only 3 consecutive pages mapped when `TXSTSZ` reports that 16384 bytes is required, but this is not guaranteed. The size of any particular transaction state is unspecified. 
The transaction state area is aligned to 4096 bytes (page alignment). This is regardless of the reported size. Violating this constraint yields a `PROT` exception.

### Interrupts, Exceptions, and Transaction State.

With the exception of the `reti` instruction, branch instructions that modify the `mode` register are forbidden in a transaction and cause an implicit transaction abort. Additionally, exceptions caused by instruction execution during a transaction will also abort the transaction
This is done to simplify the required behaviour of supervisors, by not requiring system calls (which are generally not idempotent anyways) or exceptions (which generally are not expected to occur and typically terminate the process they're running in) to save transaction state.

However, hardware-caused interrupts (particularily the periodic interrupt from a hardware timer used for context switches) may occur during any user code, including the middle of a transaction. To avoid overly burdening transactions in user code (which may be long-running, or started at inopportune times before a context switch), hardware interrupts and the `reti` instruction preserve the transaction state. For hardware interrupts that may be entered from user code supervisors must save the transaction state using the `txsuspend` instruction, and restore it with the `txresume` instruction.

To decrease the number of interrupts that require, supervisors may set `cr0.XMINTRDEFER` (bit 3) whenver the CPU indicates support for `cpuex2.TXMEM`. When `XMINTRDEFER` is in use, the 2nd double word of the interrupt table prefix contains a field called `xmintrflags`. Two flags are defined in this version, the remaining bits must be set to zero or an interrupt handling error occurs. Additionally, if `cr0.XMINTRDEFER=0`, both flags defined must be set to `0` or an interrupt handling error occurs.
* Bit 0 (`xmintrflags.PRMODE`): Two modes for XMINTRDEFER are available, selectable by this flag - DEFER Mode (`0`) and PRIORITY Mode (`1`),
* Bit 1 (`xmintrflags.IECLEAR`): Controls whether all interrupts should clear `cr0.IE`  - `0` means priority interrupts only, and `1` means always.

For hardware interrupts, the `PX` flag in the software controls whether interrupts should be deferred in program mode. When the processor would signal a hardware interrupt it checks this flag and the `PRMODE` flag (in addition to normal checks of `cr0.IE`). In DEFER mode, the interrupt is signaled when `PX=0` and is deferred when `PX=1`. In PRIORITY mode, the interrupt is only signaled when `PX=1` and is deferred when `PX=0`.


Additionally, when an Interrupt occurs, `cr0.IE` is cleared according to the `xmintrflags.IECLEAR` flag. When set to `1`, any interrupt (including software interrupts) causes this flag to be cleared. When this is set to ``0`, only priority interrupts (exceptions, and hardware interrupts with `PX=1` when `PRMODE` is set to PRIORITY mode) will clear this flag (Note: Both flags being set to `0` preserves the interrupt behaviour from `X-main`). This continues deferring hardware interrupts until they are enabled by setting the `IE` bit explicitly. The `reti` instruction does not set this flag, so the exit routine must handle this directly.

When an Interrupt is deferred, the interrupt will be raised when the processor is first about to execute any instruction with `mode.XM=0` *and* `cr0.IE`. If the mode switch to `XM=0` is caused by a hardware interrupt, it additionally is deferred exactly one instruction.


As Exceptions may also be indicated out-of-band from user instructions, there is no indication to user code that an exception occurs. If the code is resumed and continues to assume that the transaction was not aborted, it may encounter a PROT exception upon a `txcommit` or `txstore`/`txload` instruction. If the supervisor allows for the resumption of user code after an exception, it should indicate that such has happened, or otherwise emulate the continuation of the transaction (loading placeholder values - or otherwise passing through loads), and indicate a transaction failure upon `txcommit`. How the supervisor chooses to do this is not specified.



