# I/O Subsystem

This document lists standard mechanisms for interacting with System Devices from Clever-ISA processors. 

Compliance with this document is optional, but the io address space defined herein is reserved and must only be be used with the behaviour described here.

## Patent Notice

Some technologies for which interfaces are exposed by this technical document are generally protected by patents owned by groups that have not granted a general license to Clever-ISA implementors. 
Implementations of such technologies may require licenses from groups other than those allifiated with the Clever-ISA maintainers, under terms dictated by those groups.

## I/O Information

A supervisor may obtain information about the I/O capabilities of the system by reading a 64-bit value from I/O address 0x7f000000.

The structure of the result is as follows:

| Bit | Name | Description |
|-----|------|-------------|
| 0   | EN   | I/O Subsystem Enabled. Always set to 1 by machines complying with the spec |
| 1   | TIMER| CPU Interrupt Timer Available |
| 2   | ACPI | Advanced Control and Power Interface available | 
| 3   | PCI  | Peripheral Component Interconnect Available |
| 4   | GPIO | General Purpose I/O pins Available |
| 5   | SCSI | Small Computer System Interface available |
| 6   | VGA  | VGA Video Buffer Available |

Other bits are reserved for future use and shall be set to `0`.

## CPU Interrupt Timer

The CPU Interrupt Timer Registers are available via the I/O Addresses 0x7f000001 and 0x7f000002.

0x7f000001 is a read-only register containing the 64-bit CPU Interrupt Timer Information. The second is the 64-bit CPU Interrupt Timer Configuration Register.

CPU Interrupt Timer Information Register:

| Bits    |  Name   | Description |
|---------|---------|-------------|
| 0-35    | FREQ    | Integer Frequency (in Hz) of the timer's tick|
| 36-55   | MAXINT  | The maximum supported interval (in ticks). 0 means no limit|


All other bits are reserved for future use and shall be set to `0`.

CPU Interrupt Timer Configuration Register:

| Bits   | Name    | Description |
|--------|---------|-------------|
| 0-1    | MODE    | The Mode of the CPU Interrupt Timer. See enumeration below |
| 2-7    | CTRL    | Field for Controlling the resulting operation (depending on mode)|
| 32-63  | INTERVAL| The Interval (in ticks) between operations, minus 1.|

Other bits are reserved, modifying them has undefined behaviour.

The MODE field takes on one of the following values:

| Value | Name  | Description |
|-------|-------|-------------|
| 0     | DISABLE| No operation is performed. CTRL is unused |
| 1     | INT   | Hardware Interrupt Issued. CTRL is value between 0 and 31 which corresponds to the hardware interrupt issued (32-interrupt table index)|

Other values are reserved. Setting the MODE field to such values has undefined behaviour.

The current configuration may be read from the CPU Interrupt Timer Configuration Register, it will have the value last written by the supervisor.
If read before being written by the supervisor, CTRL and INTERVAL both have undefined values, all reserved bits are clear, and MODE is set to `0`.

## General Purpose I/O

Attached General Purpose I/O pins may be controlled via I/O Addresses 0x7f000008-0x7f00000f. 

Reading from 0x7f000008 (GPIO Info) yields the following Value:

| Bits  | Name   | Description  |
|-------|--------|--------------|
| 0-9   | PINCT  | Number of GPIO pins attached to the system |
| 10-15 | SELKD  | The Kind of the selected configuration pin |
| 16-19 | MAXCFG | The maximum number of pins that can be configured at once |

Other bits are reserved and shall be set to 0.

SELKD is `0` when no pin or an undefined pin is selected by 0x7f000009. Otherwise, it's set to the kind of pin, given as follows:

| Value | Name |
|-------|------|
| 0     | Vcc (power) pin |
| 1     | Digital High/Low Value only |
| 2     | Digital Logic Value |
| 3     | Analog |
| 63    | Ground pin |

Other values are reserved for future use and not be set. Supervisors should treat reserved values as being semantically equivalent to Digital High/Low Value Only.

Writing to 0x7f000009 is GPIO Configuration and Selection Register. 

The lower 10 bits of of this register can be written to for configuration. The bitfield is as follows:

| Bits  | Name  | Description |
|-------|-------|-------------|
| 0-9   | PINNO | The Number of the pin to select/configure|
| 10-15 | CFGMD | The mode to configure the pin to |
| 16-19 | CFGPOS| The position in the configuration array for the pin|
| 20    | RDEN | Read Enable. |
| 21    | WREN | Write Enable.

Other bits are reserved. CFGMD uses the same values as SELKD, except that `0` leaves the pin in its current configuration and does not effect the configuration slot, and `63` disables to pin.

CFGMD can only configure a pin to a mode that its capable of using. Vcc and Ground pins cannot be configured at all.
Digital High/Low Value pins can only be configured to High/Low value mode. Digial Logic Pins can be configured to either Logic mode or High/Low Mode.
Analog pins can be configured to either any mode. If a pin is configured to an invalid mode, it and the configuration slot is disabled.

Digital High/Low pins must be set to a lower configuration number than other pins, and Digital Logic Pins before Analog pins. Violating this requirement causes undefined behaviour

Registers from 0x7f00000a to 0x7f00000d contains value registers for Configured GPIO pins.
The lowest registers contains individual bits for Digial High/Low pins. 
After all High/Low pins are placed in the configured positions, the next register contains Digital Logic pins, in 3 bit groups.
The lowest configured Logic pin is placed in the lowest position, then each remaining one are allocated sparsely according to the configuration slot, accross up to 2 registers.
Subsequent registers contain analog pins as 8-bit values. These values are allocated accross two registers in the same manner as logic pins.

Registers are not allocated for modes that are not used by any active configuration.

The values for logic pins are as follows:

| Value | Name       |
|-------|------------|
| 0     | Strong Logic Low  |
| 1     | Weak Logic Low |
| 2     | High Impedance |
| 3     | Uninitialized |
| 6     | Weak Logic High |
| 7     | Strong Logic High |

Setting any other value is the same as setting Uniniitlaized. Uninitialized can be observed as an indeterminate value when read.

Analog pins are configured by voltage with values from 0-255. 

## PCI Memory Base Address

The I/O Address 0x7f00000e is a read-write address that contains the base physical address for the PCIe configuration address space. The default value shall be at least 0x80000000 and it shall be set to a value that is aligned to 256 MiB. When modified, it shall be set to a value aligned to 256 MiB. 

256 MiB of contiguous memory starting from the address last written to PCIe configuration base address is reserved for use by the PCIe configuration space.

## VGA Video Buffer

The I/O Address 0x7f00000f is a read-write address that contains the base physical address for the VGA Video Buffer. The default value shall be at least 0x80000000 at shall be aligned to 256 KiB, and shall not overlap the default PCI Memory Base Address. If either value is set in a manner such that the memory regions overlap, which devices recieves reads/writes in the overlapping memory location is unspecified. 

128 KiB of contiguous memory starting from the address last written to the VGA Video Buffer Base Address is reserved for use by the VGA Video Buffer.