PLBRAM-Ultra96
=======================================================================

This Repository provides example for uiomem and ZynqMP-FPGA-Linux.

## Requirement

 * Board: Ultra96 or Ultra96-V2
 * OS: ZynqMP-FPGA-Linux
   - Debian13  https://github.com/ikwzm/ZynqMP-FPGA-Debian13/tree/v3.1.0
   - Debian12  https://github.com/ikwzm/ZynqMP-FPGA-Debian12/tree/v7.0.0
   - v2019.2.1 https://github.com/ikwzm/ZynqMP-FPGA-Linux/tree/v2019.2.1
   - v2020.1.1 https://github.com/ikwzm/ZynqMP-FPGA-Linux/tree/v2020.1.1
 * uiomem (v1.1.0-beta.1) https://github.com/ikwzm/uiomem/tree/1.1.0-beta.1
 * fclkcfg (v1.7.3) https://github.com/ikwzm/fclkcfg/tree/v1.7.3

## Boot Ultra96 and login fpga user

fpga'password is "fpga".

```console
debian-fpga login: fpga
Password:
fpga@debian-fpga:~$
```

## Download this repository

```console
fpga@debian-fpga:~$ mkdir work
fpga@debian-fpga:~$ cd work
fpga@debian-fpga:~/work$ git clone https://github.com/ikwzm/PLBRAM-Ultra96
fpga@debian-fpga:~/work$ cd PLBRAM-Ultra96
```

## Setup

### Build uiomem and uiomem-test

#### Update submodules

```console
fpga@debian-fpga:~/work/PLBRAM-Ultra96$ git submodule init
Submodule 'u-dma-buf' (https://github.com/ikwzm/udmabuf) registered for path 'u-dma-buf'
Submodule 'uiomem' (https://github.com/ikwzm/uiomem.git) registered for path 'uiomem'
Submodule 'uiomem-test' (https://github.com/ikwzm/uiomem-test.git) registered for path 'uiomem-test'
fpga@debian-fpga:~/work/PLBRAM-Ultra96$ git submodule update
Cloning into '/home/fpga/work/PLBRAM-Ultra96/u-dma-buf'...
Cloning into '/home/fpga/work/PLBRAM-Ultra96/uiomem'...
Cloning into '/home/fpga/work/PLBRAM-Ultra96/uiomem-test'...
Submodule path 'u-dma-buf': checked out '80e19cb551f6c6f611245433b346212b8e9d17cd'
Submodule path 'uiomem': checked out 'd9ddd8035832ea77b4cbfec69d3a4aac5893dc14'
Submodule path 'uiomem-test': checked out '9023c48045d853cc6b6202859dd00848f7949dbc'
```

#### Build uiomem kenrel module

```console
fpga@debian-fpga:~/work/PLBRAM-Ultra96$ cd uiomem
fpga@debian-fpga:~/work/PLBRAM-Ultra96/uiomem$ make
make -C /lib/modules/6.12.60-zynqmp-fpga-generic/build ARCH=arm64 CROSS_COMPILE= M=/home/fpga/work/PLBRAM-Ultra96/uiomem CONFIG_UIOMEM=m modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.60-zynqmp-fpga-generic'
warning: the compiler differs from the one used to build the kernel
  The kernel was built by: aarch64-linux-gnu-gcc (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
  You are using:           gcc (Debian 14.2.0-19) 14.2.0
  CC [M]  /home/fpga/work/PLBRAM-Ultra96/uiomem/uiomem.o
  MODPOST /home/fpga/work/PLBRAM-Ultra96/uiomem/Module.symvers
  CC [M]  /home/fpga/work/PLBRAM-Ultra96/uiomem/uiomem.mod.o
  CC [M]  /home/fpga/work/PLBRAM-Ultra96/uiomem/.module-common.o
  LD [M]  /home/fpga/work/PLBRAM-Ultra96/uiomem/uiomem.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.60-zynqmp-fpga-generic'
fpga@debian-fpga:~/work/PLBRAM-Ultra96/uiomem$ cd ..
```

#### Build uiomem test programs

```console
fpga@debian-fpga:~/work/PLBRAM-Ultra96$ cd uiomem-test
fpga@debian-fpga:~/work/PLBRAM-Ultra96/uiomem-test$ make
gcc -O2 -DUSE_UIOMEM_IOCTL -o uiomem-file-test uiomem-file-test.c uiomem.c
gcc -O2 -DUSE_UIOMEM_IOCTL -o uiomem-ioctl-test uiomem-ioctl-test.c
gcc -O2 -DUSE_UIOMEM_IOCTL -o uiomem-throughput-test uiomem-throughput-test.c uiomem.c
fpga@debian-fpga:~/work/PLBRAM-Ultra96/uiomem-test$ cd ..
```

### Load uiomem

```console
fpga@debian-fpga:~/work/PLBRAM-Ultra96$ sudo insmod uiomem/uiomem.ko
```

### Load FPGA and Device Tree

```console
fpga@debian-fpga:~/work/PLBRAM-Ultra96$ sudo rake install
gzip -d -f -c plbram_256k_dbg.bit.gz > /lib/firmware/plbram_256k_dbg.bit
./dtbo-config --install plbram_256k --dts plbram_256k_dbg.dts
<stdin>:36.18-40.20: Warning (unit_address_vs_reg): /fragment@2/__overlay__/uiomem_plbram: node has a reg or ranges property, but no unit name
<stdin>:27.13-42.5: Warning (avoid_unnecessary_addr_size): /fragment@2: unnecessary #address-cells/#size-cells without "ranges", "dma-ranges" or child "reg" property```
```

```console
fpga@debian-fpga:~/work/PLBRAM-Ultra96$ dmesg | tail -21
[ 1552.834684] fpga_manager fpga0: writing plbram_256k_dbg.bit to Xilinx ZynqMP FPGA Manager
[ 1553.000376] OF: overlay: WARNING: memory leak will occur if overlay removed, property: /fpga-region/firmware-name
[ 1553.013457] fclkcfg amba_pl@0:fclk0: driver version : 1.9.1
[ 1553.019082] fclkcfg amba_pl@0:fclk0: device name    : amba_pl@0:fclk0
[ 1553.025598] fclkcfg amba_pl@0:fclk0: clock  name    : pl0_ref
[ 1553.031378] fclkcfg amba_pl@0:fclk0: clock  rate    : 99999999
[ 1553.037253] fclkcfg amba_pl@0:fclk0: clock  enabled : 1
[ 1553.042484] fclkcfg amba_pl@0:fclk0: remove rate    : 1000000
[ 1553.048228] fclkcfg amba_pl@0:fclk0: remove enable  : 0
[ 1553.053453] fclkcfg amba_pl@0:fclk0: driver installed.
[ 1553.060203] uiomem uiomem0: driver version = 1.1.0-beta.1
[ 1553.065663] uiomem uiomem0: ioctl version  = 1
[ 1553.070126] uiomem uiomem0: major number   = 235
[ 1553.074745] uiomem uiomem0: minor number   = 0
[ 1553.079189] uiomem uiomem0: range address  = 0x0000000400000000
[ 1553.085109] uiomem uiomem0: range size     = 262144
[ 1553.089987] uiomem uiomem0: cached         = 1
[ 1553.094431] uiomem uiomem0: coherent       = 0
[ 1553.098875] uiomem uiomem0: sync_operation = ARM64 Native
[ 1553.104274] uiomem uiomem0: shareable      = 0
[ 1553.108719] uiomem 400000000.uiomem_plbram: driver installed.
```

## Run uiomem-file-test

```console
fpga@debian-fpga:~/work/PLBRAM-Ultra96$ sudo ./uiomem-test/uiomem-file-test
device=uiomem0
driver_version=1.1.0-beta.1
sync_operation=ARM64 Native
ioctl_version=1
size=262144
shareable=0
cached=1
coherent=0
mmap write test : sync=1 time=0.000733 sec (0.000732 sec)
mmap read  test : sync=1 time=0.003153 sec (0.003152 sec)
compare = ok
mmap write test : sync=0 time=0.000616 sec (0.000360 sec)
mmap read  test : sync=1 time=0.002878 sec (0.002877 sec)
compare = ok
mmap write test : sync=1 time=0.000597 sec (0.000597 sec)
mmap read  test : sync=0 time=0.000347 sec (0.000300 sec)
compare = ok
mmap write test : sync=0 time=0.000587 sec (0.000347 sec)
mmap read  test : sync=0 time=0.000337 sec (0.000299 sec)
compare = ok
file write test : sync=1 time=0.000374 sec (0.000374 sec)
mmap read  test : sync=0 time=0.000346 sec (0.000301 sec)
compare = ok
file write test : sync=0 time=0.000294 sec (0.000272 sec)
mmap read  test : sync=0 time=0.000338 sec (0.000300 sec)
compare = ok
mmap write test : sync=0 time=0.000588 sec (0.000345 sec)
file read  test : sync=1 time=0.000337 sec (0.000336 sec)
compare = ok
mmap write test : sync=0 time=0.000632 sec (0.000272 sec)
file read  test : sync=0 time=0.000375 sec (0.000337 sec)
```

## Run uiomem-throughput-test

* sync=0: mmap is cacheable, and cache synchronization is performed before and after each access.
* sync=1: mmap is non-cacheable, and cache synchronization is not performed before or after accesses.

```console
fpga@debian-fpga:~/work/PLBRAM-Ultra96$
device=uiomem0
driver_version=1.1.0-beta.1
sync_operation=ARM64 Native
ioctl_version=1
size=262144
shareable=0
cached=1
coherent=0
mmap write test : sync=0 throughput=790.5 MBytes/sec
mmap read  test : sync=0 throughput=730.6 MBytes/sec
mmap write test : sync=1 throughput=255.6 MBytes/sec
mmap read  test : sync=1 throughput= 37.8 MBytes/sec
```

## Clean up

```console
fpga@debian-fpga:~/work/PLBRAM-Ultra96$ sudo rake uninstall
./dtbo-config --remove plbram_256k
[ 1789.303161] uiomem 400000000.uiomem_plbram: driver removed.
[ 1789.312704] fclkcfg amba_pl@0:fclk0: driver removed.
```

## Build Bitstream file

### Requirement

* Vivado 2019.2

### Download this repository

```console
shell$ git clone https://github.com/ikwzm/PLBRAM-Ultra96
Cloning into 'PLBRAM-Ultra96'...
remote: Enumerating objects: 30, done.
remote: Counting objects: 100% (30/30), done.
remote: Compressing objects: 100% (22/22), done.
remote: Total 30 (delta 7), reused 30 (delta 7), pack-reused 0
Unpacking objects: 100% (30/30), done.
```

### Create Vivado Project

```console
vivado% cd project
vivado% vivado -mode batch -source create_project.tcl
```

or

```
Vivado > Tools > Run Tcl Script... > project/create_project.tcl
```

### Implementation

```console
vivado% cd project
vivado% vivado -mode batch -source implementation.tcl
vivado% cp project.runs/impl_1/design_1_wrapper.bit ../plbram_256k_dbg.bit
```

or

```
Vivado > Tools > Run Tcl Script... > project/implementation.tcl
```

### Convert from Bitstream File to Binary File

```console
vivado% cd project
vivado% bootgen -image plbram_256k_dbg.bif -arch zynqmp -w -o ../plbram_256k_dbg.bin
vivado% cd ..
```

### Compress plbram_256k_dbg.bin to plbram_256k_dbg.bin.gz

```console
vivado% gzip plbram_256k_dbg.bin
```

