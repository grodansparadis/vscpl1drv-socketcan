% vscpl1drv-socketcan(7) VSCP Level I Logger Driver
% Åke Hedman, Grodans Paradis AB
% September 27, 2019

# NAME

vscpl1drv-socketcan - VSCP Level I Logger Driver

# SYNOPSIS

vscpl1drv-socketcan

# DESCRIPTION

VSCP level I driver (CANAL driver) for diagnostic logging. It makes it possible to log (VSCP Level I events to a text file. Several drivers can be loaded logging data to different output files and using different filter/masks.

## Configuration string

path;filter;mask

The absolute or relative path including the file name to the file that log data should be written to. The filename is followed by the optional 32-bit filter and mask. The default is all events logged.

Note that the filter/mask looks at the CAN ID. If you work with VSCP look at format of the 29 bit CAN identifier in VSCP Bit 32 is set to one for an extended frame (all VSCP frames) and bit 30 is set to one for RTR frames (never for VSCP).

## Flags

* 0 - Append data to an existing file (create it if it's not available).
* 1 - Create a new file or rewrite an old file with new data.

## Status return

The CanalGetStatus call returns the status structure with the channel_status member having the following meaning:

* 0 is always returned.

## Log file format

The log file have the following format and consist of the following parts

* Time when frame was received
* Timestamp
* Flags
* ID
* Number of databytes
* Databytes

There are many Level I drivers (CANAL drivers) available in VSCP & Friends framework that can be used with both VSCP Works and the VSCP Daemon (vscpd) and other tools that interface the drivers using the CANAL standard interface. Added to that many Level II and Level III drivers are available that can be used with the VSCP Daemon.

Level I drivers is documented [here](https://grodansparadis.gitbooks.io/the-vscp-daemon/level_i_drivers.html).

Level II drivers is documented [here](https://grodansparadis.gitbooks.io/the-vscp-daemon/level_ii_drivers.html)

Level III drivers is documented [here](https://grodansparadis.gitbooks.io/the-vscp-daemon/level_iii_drivers.html)

# SEE ALSO

`vscpd` (8).
`uvscpd` (8).
`vscpworks` (1).
`vscpcmd` (1).
`vscp-makepassword` (1).
`vscphelperlib` (1).

The VSCP project homepage is here <https://www.vscp.org>.

The [manual](https://grodansparadis.gitbooks.io/the-vscp-daemon) for vscpd contains full documentation. Other documentation can be found here <https://grodansparadis.gitbooks.io>.

The vscpd source code may be downloaded from <https://github.com/grodansparadis/vscp>. Source code for other system components of VSCP & Friends are here <https://github.com/grodansparadis>

# COPYRIGHT
Copyright 2000-2019 Åke Hedman, Grodans Paradis AB - MIT license.