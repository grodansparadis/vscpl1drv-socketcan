# vscpl1drv-socketcan - VSCP Level I Socketcan Driver

<img src="https://vscp.org/images/logo.png" width="100">

Level I driver for Linux Socketcan. SocketCAN, the official CAN API of the Linux kernel, has been included in the kernel for many years now. Meanwhile, the official Linux repository has device drivers for all major CAN chipsets used in various architectures and bus types. SocketCAN offers the user a multiuser capable as well as hardware independent socket-based API for CAN based communication and configuration. Socketcan nowadays give access to the major CAN adapters available on the market. 

Note that as CAN only can handle Level I events only events up to class < 1024 can be sent to socketcan devices.

## Platforms
  * Linux

## Driver for Linux

```bash
vscpl1drv-socketcan.so
```

## Install location

### Linux

From version 14.0.0 the driver is installed in */var/lib/vscp/drivers/level1*

### Windows
From version 14.0.0 the driver is installed in */program files/vscpd/drivers/level1*

## Configuration string

All level I drivers are configured using a driver specific semicolon separated configuration string.

```
interface[;mask;filter]
```

### interface

Typically "any" or something like "vcan0", "can0", "can1", "can2"

### mask (optional)

mask is the mask for the adapter.

### filter (optional)

filter is the filter for the adapter.


## Typical settings for VSCP daemon config

```xml
<!-- The can4vscp driver -->
<driver enable="false"
        name="socketcanl1"
        config="vcan0"
        flags="0"
        translation="0x02"
        path="/var/lib/vscp/drivers/level1/vscpl1drv-socketcan.so"
        guid="FF:FF:FF:FF:FF:FF:FF:F5:01:00:00:00:00:00:00:02"
/>
```

## Flags

Bit 32 (flags=0x80000000) is used to set debug mode. In this mode debug output is sent to syslog.


## Install the driver

Install Debian package

```bash
> sudo apt install ./vscpl2drv-socketcan_1.1.0-1_amd64.deb
```

using the latest version from the repositories [release section](https://github.com/grodansparadis/vscpl1drv-socketcan/releases).

or

```
./configure
./make
sudo make install
```

use the switch **--enable-debug** if you want a debug build.

## Install the driver on Linux using vscp private repository

**Warning !!!** *Currently this is very much experimental*

```bash
wget -O - http://apt.vscp.org/apt.vscp.org.gpg.key | sudo apt-key add -
```

then add

```bash
deb http://apt.vscp.org/debian buster main
deb http://apt.vscp.org/debian eoan main
```

to the file

```bash
/etc/apt/sources.list
```

replace **eoan** with the os-release you have installed and **debian** to *debian*, *ubuntu* or *raspian*

## Install the driver on Windows
Install using the binary install file in the release section.

## How to build the driver on Linux

```bash
git clone https://github.com/grodansparadis/vscpl1drv-socketcan.git
cd vscpl1drv-socketcan
git submodule update --init
./configure
make
make install
```

Default install folder is **/var/lib/vscp/drivers/level1**

You need *build-essentials* and *git* installed on your system.

```bash
sudo apt update && sudo apt -y upgrade
sudo apt install build-essential git
```

---

There are many Level I drivers (CANAL drivers) available in VSCP & Friends framework that can be used with both VSCP Works and the VSCP Daemon (vscpd) and other tools that interface the drivers using the CANAL standard interface. Added to that many Level II and Level III drivers are available that can be used with the VSCP Daemon.

Level I drivers is documented [here](https://docs.vscp.org/vscpd/latest/#/level_i_drivers).

Level II drivers is documented [here](https://docs.vscp.org/vscpd/latest/#/level_ii_drivers)


The VSCP project homepage is here <https://www.vscp.org>.

The [manual](https://docs.vscp.org/vscpd/latest) for vscpd contains full documentation. Other documentation can be found on the  [documentaion portal](https://docs.vscp.org).

The vscpd source code may be downloaded from <https://github.com/grodansparadis/vscp>. Source code for other system components of VSCP & Friends are here <https://github.com/grodansparadis>


Copyright (C) 2000-2021 Ake Hedman, Grodans Paradis AB - MIT license.
