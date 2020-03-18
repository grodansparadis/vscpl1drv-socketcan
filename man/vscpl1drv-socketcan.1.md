% vscpl1drv-socketcan(7) VSCP Level I Socketcan Driver
% Åke Hedman, Grodans Paradis AB
% Mars 18, 2020

# NAME

vscpl1drv-socketcan - VSCP Level I socketcan Driver

# SYNOPSIS

vscpl1drv-socketcan

# DESCRIPTION

SocketCAN, the official CAN API of the Linux kernel, has been included in the kernel for many years now. Meanwhile, the official Linux repository has device drivers for all major CAN chipsets used in various architectures and bus types. SocketCAN offers the user a multiuser capable as well as hardware independent socket-based API for CAN based communication and configuration. Socketcan nowadays give access to the major CAN adapters that is available on the market. Note that as CAN only can handle Level I events only events up to class < 1024 can be sent to this device. Other events will be filtered out. Also received events 

## Configuration string

```
interface[;mask;filter]
```

### interface
Typically _"any"_ or something like _"vcan0"_, _"can0"_, _"can1"_, _"can2"_

### mask
is the mask for the adapter.

### filter
is the filter for the adapter. 

## Flags

Not used, set to 0.


# SEE ALSO

`vscpd` (8).
`uvscpd` (8).
`vscpworks` (1).
`vscpcmd` (1).
`vscp-makepassword` (1).
`vscphelperlib` (1).

The VSCP project homepage is here <https://www.vscp.org>.

# COPYRIGHT
Copyright 2000-2020 Åke Hedman, Grodans Paradis AB - MIT license.