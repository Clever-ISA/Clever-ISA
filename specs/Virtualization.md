# Virtualization and Hypervisor Operation

This specification describes an extension to the Clever ISA to support virtualization and native hypervisors.

## License

Copyright (c)  2021  Connor Horman.
Permission is granted to copy, distribute and/or modify this document
under the terms of the GNU Free Documentation License, Version 1.3
or any later version published by the Free Software Foundation;
with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
A copy of the license is included in the repository, under the file entitled LICENSE. Otherwise, you may find a copy at <https://www.gnu.org/licenses/fdl-1.3.en.html>.

## CPU Support

A CPU Indicates conformance to these extensions by setting bit 12 (VIRT) of cpuex2.

## Supervisor Execution Mode 

This document describes supervisors (flags.XM=0) as executing in one of two further modes:
- Unmanaged Supervisor Mode which is the default, and
- Managed Supervisor Mode, which indicates the presence of a hypervisor

Supervisors need not indicate support for this extension, and any previously valid code is valid in both Unmanaged and Managed Mode, except that io addresses defined by the spec may be modified by the hypervisor in Managed Mode.

Additionally, a fourth mode, Hypervisor mode, exists as a superset of Unmanaged Supervisor Mode, when Managed Supervisor Mode is enabled. 

Switching between these modes is described in "Managed Execution Control Word".

As managed mode only affects supervisor instructions and exceptions, there is no distinction made between managed program execution mode and unmanaged program execution mode.

Hypervisor mode is incompatible with program-execution mode. A managed supervisor must first be created (vstart) to transition into program-execution mode, or the program must disable mxcw.ME. While in hypervisor mode, an iret or scret that places the system in Program-Execution Mode (by setting flags.XM to 1) causes an ABORT.

Registers other than r0 and r1 used by the hypervisor are preserved in an unspecified manner that does not modify any memory accessible to the hypervisor when transfering control to a managed supervisor, and are restored when control is transfered to the hypervisor. Control is transfered to a hypervisor from a managed supervisor when:
- The supervisor issues an hcall instruction,
- The supervisor recieves an interrupt that is trapped by the hypervisor or is RESET, or
- When the hypervisor recieves an interrupt. 

When transferring control to the hypervisor from a managed supervisor, the register state of the virtual machine is stored in the register array given by vmcs.vmregs, and are reloaded when transferring control back to the virtual machine. Any register that is not available in the CPU's current mode are *not modified* when loading or storing register values. Additionally, illegal values of control registers loaded in this manner, the `flags` register, or `fpcw` (register 19 when floating-point extensions are available) are reflected unmodified in the supervisor (however, writing such values will continue to have the standard effect). This allows hypervisors to emulate extensions that does not have available hardware support. The presence of extensions are given by bits in the loaded cpu extended feature registers intersect those set by the cpu at startup - an exception occurs in the supervisor if either version of the register indicates the feature is unavailable. 

When the hypervisor recieves an interrupt while within the context of a virtual machine (including an exception raised by the hcall instruction executed from within a hypervisor) the stored ip value is undefined, but the current virtual machine's vm context identifier is available in io port 0x7f800001, and a pointer to the virtual machine's control structure in io port 0x7f800002. 


### Detecting Managed Supervisor Mode

It is possible to detect the prescene of a hypervisor from Managed Supervisors in a few standard ways, as permitted by the hypervisor. io address 0x7f800000 contains the managed execution control word, and the ME bit will be set in a Managed Supervisor (and clear in an Unmanaged Supervisor). 
Additionally, io port 0x7f800001 contains the vm context identifier of the current vm when read in the context of a virtual machine (including from within that virtual machine), and otherwise will contain `0`
Both of these mechanisms may be disabled by the hypervisor that enables IO Trapping by either reading zero or triggerring UND. 

Other, non-standard mechanisms may be by using the cpuidhi and cpuidlo registers with prearranged identifiers for particular hypervisors, a machine specific instruction, or a hypervisor procedure call. 

## Managed Execution Control Word

io address 0x7f800000 refers to a 64-bit register, the Managed Execution Control Word (mxcw).
The layout of mxcw is as follows:
| Bit  | Name   |      Description            |
|------|--------|-----------------------------|
| 0    | ME     | Management Enable           |
| 1-16 | TRX    | Trap Exceptions             |


All other bits are reserved, setting those bits to 1 has undefined behaviour. 
mxcw.TRX is unused with mxcw.ME=0

When mxcw.ME is set, the system is in Managed Mode or Hypervisor mode. Otherwise, it is in Unmanaged Supervisor Mode. 

When a system in Unmanaged Supervisor Mode sets mxcw.ME, the system is placed into Hypervisor mode. When in Hypervisor Mode, setting the bit to `0` places the system back into Unmanaged Supervisor Modes.

mxcw.TRX specifies which exceptions are trapped by the hypervisor before passing through to the supervisor. Bits 1-16 of mxcw correspond to exceptions 0-15. Additionally, Hypervisors always trap RESETs. 

When a trapped exception occurs within a supervisor, the pointer to the handler stored in vmcs.vmtrxtab is used as the vector address, and control is transfered to the hypervisor starting from that address. XA and PF can occur within the hypervisor for the corresponding address conditions. In either case (if a hypervisor exception does or does not occur) the vm context identifier indicates the corresponding exception recieved, and the exception will be handled by the supervisor's interrupt table if the hresume instruction is used. The vm context identifier will be in r0 and a pointer to the vmcs in r1.

Writes to the mxcw from Managed Supervisor Mode has no effect (Note: If trapio is enabled, caution must be taken by the Hypervisor not to propagate the writes to the io address verbaitum as the write will occur from the hypervisor not the Managed Supervisor - if this behaviour is not desired, the hypervisor must manually ignore or otherwise handle such a write).

Disabling Management resets the values of io addresses 0x7f800001 and 0x7f800002, even if management is subsequently reenabeled. Additionally, virtual machines that were created prior to management being disabled are disposed of. Attempts to transfer control into such virtual machines from vm context identifiers using the hret or hresume instructions have undefined behaviour. This resets the number of created virtual machines counting towards the limit.

## Virtual Machine Control Structure

A Virtual Machine Control Structure is a 128-byte structure, given by the following C definition:
```c
struct vmcs{
    uint64_t (* __physical vmregs)[256];
    ptab vmmmap;
    int vmpas: 3;
    int vmvas: 3;
    int vmcsver: 4;
    _Bool trapio: 1;
    int reserved35: 21;
    int32_t reserved56;
    void* __physical (* __physical vmtrxtab)[17];
}
```

vmcs.vmregs points to the physical address of the memory to store the values of the registers of the virtual machine, and to load those values from. vmcs.vmmmap stores the pointer to the virtual machine's address lookup table, with the depth as given by vmcs.vmpas. 

vmcs.vmpas and vmcs.vmvas are the physical and virtual address sizes of the virtual machine, equivalent to cpuex2.VAS and cpuex2.PAS respectively (note: this does not modified the observed value of those fields in the VM). Both address sizes must be at most cpuex2.VAS, and vmcs.vmpas must be at most vmcs.vmvas. The behaviour is undefined if either constraint is violated.  
vmcs.vmcsver is the version of the vmcs structure, which must be 0. 

vmcs.vmtrxtab points to an array of physical addresses to transfer control to when trapping the corresponding exception. Entry 17 is used when trapping RESET.

vmcs.trapio is set to true if the `in` and `out` instructions require hypervisor handling. When these are set, a PROT exception will be generated within the supervisor if the instructions are executed in any mode, rather than just program execution mode. 

reserved35 and reserved56 are reserved and must be set to zero. The behaviour is undefined if any bits in reserved35 and reserved56 are set to 1.

## Instructions

### Call to Hypervisor Procedure

Opcodes: 0xfcb
Operands: 0

h: Reserved and Must be 0.

Supervisor Exceptions:
- PROT: If flags.XM=1
- UND: If the Processor is not in Managed Supervisor Mode

Hypervisor Exceptions
- XA: If the destination instruction pointer is not well-aligned
- PF: If paging is enabled in the hypervisor, and the destination instruction pointer is not executable.
- PF: If paging is enabled in the hypervisor, and the destiniation instruction pointer is an invalid virtual address
- PF: If paging is not enabled in the hypervisor, and the destination instruction pointer is an out of range physical address

If a supervisor exception occurs, then the instruction is aborted. Exceptions will not be recieved at the hypervisor level. 

If a hypervisor or unmanaged supervisor executes the instruction, it will directly recieve the UND Interrupt. 

Operation:
- 0xfcb (hcall): If the CPU is in Managed Supervisor Mode, the values of each register are stored in the vmregs field for the virtual machine, then control is transfered to the hypervisor. ip is loaded from scdp and r7 is loaded from scsp. A vm context identifier is placed in r0, and the physical address of the vmcs is placed in r1. 

The value of the vm context identifier is unspecified and has implementation-specific meaning and form, but the lower 32-bit is an identifier that uniquely identifies the virtual machine that executed the hcall instruction from all other active virtual machines within the hypervisor and is consistent in all control transfers from that particular virtual machine. A vm context identifier obtained from the hcall instruction does not signal a pending exception. 

The system call registers scdp and scsp are reused in the hypervisor for hypervisor procedure calls as scall is not useful to hypervisors. sccr is unused by hcall, but remains available. Hypervisors can use this register for it's own purposes, provided it does not use the scall instruction (managed supervisors and programs running inside managed supervisors are not bound by this restriction). 


### Return from Hypervisor Procedure Call

Opcodes: 0xfd6-0xfd7
Operands: 0

h: Reserved and Must be 0.

Supervisor Exceptions:
- PROT: If flags.XM=1
- UND: If the processor is not in Hypervisor Mode
- XA: If the destination instruction pointer is not well-aligned
- PF: If paging is enabled in the supervisor, and the destination instruction pointer is not executable
- PF: If paging is enabled in the supervisor, and the destination instruction pointer is an invalid virtual address
- PF: If paging is not enabled in the supervisor, and the destination instruction pointer is an invalid physical address


Hypervisor Exceptions:
- PROT: If r0 does not contain a valid vm context identifier. 
- PROT: If the vm indicated by the vm context identifier recieved RESET. 

No Hypervisor Exception occurs if the processor is not in Hypervisor mode. No Supervisor exception occurs if the processor is in Hypervisor Mode and a Hypervisor Exception occurs. 

Operation:
- 0xfd6 (hret): Transfers control back to virtual machine indicated by the vm context identifier in r0, loading the registers from the corresponding vm's vmcs.vmregs field and jumping to the address in `ip`. 
- 0xfd7 (hresume): Same as hret, but if the vm context identifier signals a pending exception, control will be transfered to that exception


hret has dual purpose: It can also be used to return to a managed supervisor from an interrupt (either a trapped exception, or a hypervisor interrupt). If hret is used, preexisting exceptions are not signalled to the managed supervisor. If a trapped exception is desired to be propagated to the managed supervisor, the hresume instruction may be used instead, or the hypervisor can set up the registers and memory to perform the exception instead. As hret does not trigger an exception, any exception that occurs while executing the interrupt handler will not trigger an ABORT or RESET. 

Neither hret or hresume may be used to transfer control to a virtual machine that has RESET. Rather, the virtual machine must be disposed of with vmstop, and a new virtual machine created if the hypervisor wishes to resume or reset such a virtual machine. 

If the vm context identifier was obtained from a virtual machine that RESET, neither hret nor hresume can be used to transfer control back to it. 

neither hret nor hresume consistute a branch within the virtual machine for the purposes of instruction execution coherence unless the `ip` register is modified prior to returning (however, an explicit hcall does, as does any interrupt resumed by the hresume instruction). 

### Virtual Machine Creation

Opcodes: 0xe00

Operands: 1

h: Reserved and must be 0

Hypervisor Exceptions:
- UND, if the processor is not in Hypervisor Mode
- PROT, if the machine-specific limit of the number of virtual machines is exceeded.

Supervisor Exceptions:
- XA, if the given ip value is not well-aligned
- PF, if paging is enabled in the new managed supervisor, and the given ip value is not executable
- PF, if paging is enabled in the new managed supervisor
- PF, if the given ip value is an out of range physical address in the supervisor

If a Hypervisor Exception occurs, no Supervisor Exception occurs. 

Operations:
- 0xe00 (vmcreate): Reads the address of a vmcs structure from the operand, then creates a new virtual machine as described by the vmcs structure. Control is transfered to that virtual machine at the value in vmcs.vmregs[16]. 

The maximum number of hypervisors supported by a CPU is machine-specific, but a machine conforming to these extensions supports at least 1 on each CPU. There may be a different limit across all CPUs and on each individual CPU. Violating either limit causes a PROT exception.

#### Other CPUs

Virtual Machines are confined to the CPU that creates them, using a vm context identifier obtained on one cpu with the hresume or hret instructions on a different cpu has undefined behaviour. Additionally, virtual machines only observe a single available CPU when directly accessing io address 0xfe80 or the CPU Intiatilization registers starting at io addresses 0xff00 and 0xff80. 

### Virtual Machine Disposal

Opcodes: 0xe01

Operands: 0

Hypervisor Exceptions:
- UND, if the processor is not in Hypervisor Mode
- PROT, if the vm context identifier in r0 is not a valid vm context identifier.

Operations:
- 0xe01 (vmdestroy): Disposes of the virtual machine given by the vm context identifier in r0. The virtual machine cannot be resumed and no longer counts against the limits described in "Virtual Machine Creation".The vm context identifier may be subsequently reused by the CPU.

 The behaviour is undefined if a hypervisor attempts to resume a Virtual Machine that was destroyed by this instruction. 

## Other Information

### Virtual Machine Conformance

Many Operations and features described by this extension permit hypervisors to trap operations of Managed Supervisors permitted by the Clever ISA Specification or by a reported extension, and may be used to alter the behaviours indicated by the Specification. 
A hypervisor is responsible for ensuring that no unintentional alterations occur, that it does not indicate the presense of a feature that neither the host cpu implements, nor it properly emulates, and that any intentional divergence is available only as a result of behaviour indicated to be undefined, or at explicit request of the Managed Supervisor within the virtual machine (which may occur via a hypervisor procedure call, machine-specific instruction, or modification of a control register, including an undefined control register, or a machine specific control register).
An exception is that Hypervisors may disable (read as zero and ignore writes to) any io address that otherwise performs an operation under the Clever ISA Specification or any indicated extension (including VIRT), or may permit Managed Supervisors to modify io address 0x7f800000 (for example, to emulate nested virtual machines). 


### Machine Specific Instructions and Machine Specific Control Registers

Unless the hypervisor indicates otherwise, use of a machine specific control register or a machine specific instruction from a Managed Supervisor shall cause UND. This allows hypervisors to handle or replace these operations. 
The mechanism for indicating msr or machine-specific instruction passthrough is not specified and hypervisors should consult machine-specific documentation. 

### vmregs

The vmregs field of the virtual machine control structure contains the values of registers of a particular virtual machine while that virtual machine is not operating (for example, as a result of transfer of control from that virtual machine to the hypervisor). While the virtual machine is executing code, the result of modifying memory pointed to by that field is undefined. Additionally, the value that results from reading from any memory address pointed to the vmregs field is undefined and may change between reads (this allows, for example, that managed supervisor registers are maintained merely by transparently accessing this memory region). 
Different virtual machines active on different cpus must have distinct (non-overlapping) regions of memory used for the vmregs field, or unpredictable results may occur. 

### Feature Emulation

Hypervisors may modify control registers, the floating-point control word (register 19 with X-float), the flags register (register 17), undefined registers, and otherwise read-only registers (such as the cpuinfo registers) accessed the context of a virtual machine, by modifying the relevant indecies in the array. 
The results of these modifications may violate constraints normally imposed on these registers. 
This allows Hypervisors to store additional state corresponding to features that may not otherwise be available on the current CPU, but are properly emulated by the hypervisor. 
Any normally accessible register will read the stored value, but writes to various bits have the behaviour described in there relevant section (for example, modifying an undefined bit of a control register will triger a UND exception in the Supervisor). 
Reading from an undefined/reserved register will still cause a UND exception even if it was modified by the hypervisor in this manner.

In the future, features may be included to faciliate this mechanism of emulation.