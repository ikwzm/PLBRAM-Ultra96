PLBRAM-Ultra96
=======================================================================

This Repository provides example for u-dma-buf and ZynqMP-FPGA-Linux.

# Requirement

 * Board: Ultra96
 * OS: ZynqMP-FPGA-Linux v2019.2.1 ([https://github.com/ikwzm/ZynqMP-FPGA-Linuxtree/v2019.2.1])
 * u-dma-buf v2.2.0-rc3 ([https://github.com/ikwzm/udmabuf/tree/v2.2.0-rc3])

# Boot Ultra96 and login fpga user

fpga'password is "fpga".

```console
debian-fpga login: fpga
Password:
fpga@debian-fpga:~$
```

# Download this repository

## Download this repository

```console
fpga@debian-fpga:~$ mkdir examples
fpga@debian-fpga:~$ cd examples
fpga@debian-fpga:~/examples$ git clone https://github.com/ikwzm/PLBRAM-Ultra96
Cloning into 'PLBRAM-Ultra96'...
remote: Enumerating objects: 30, done.
remote: Counting objects: 100% (30/30), done.
remote: Compressing objects: 100% (22/22), done.
remote: Total 30 (delta 7), reused 30 (delta 7), pack-reused 0
Unpacking objects: 100% (30/30), done.
fpga@debian-fpga:~/examples$ cd PLBRAM-Ultra96
```

# Setup

## Build u-dma-buf

```console
fpga@debian-fpga:~/examples/PLBRAM-Ultra96$ git submodule init
Submodule 'u-dma-buf' (https://github.com/ikwzm/udmabuf) registered for path 'u-dma-buf'
fpga@debian-fpga:~/examples/PLBRAM-Ultra96$ git submodule update
Cloning into '/home/fpga/examples/PLBRAM-Ultra96/u-dma-buf'...
Submodule path 'u-dma-buf': checked out '80e19cb551f6c6f611245433b346212b8e9d17cd'
fpga@debian-fpga:~/examples/PLBRAM-Ultra96$ cd u-dma-buf
fpga@debian-fpga:~/examples/PLBRAM-Ultra96/u-dma-buf$ make
make -C /lib/modules/4.19.0-xlnx-v2019.2-zynqmp-fpga/build ARCH=arm64 CROSS_COMPILE= M=/home/fpga/examples/PLBRAM-Ultra96/u-dma-buf obj-m=u-dma-buf.o modules
make[1]: Entering directory '/usr/src/linux-headers-4.19.0-xlnx-v2019.2-zynqmp-fpga'
  CC [M]  /home/fpga/examples/PLBRAM-Ultra96/u-dma-buf/u-dma-buf.o
  Building modules, stage 2.
  MODPOST 1 modules
  CC      /home/fpga/examples/PLBRAM-Ultra96/u-dma-buf/u-dma-buf.mod.o
  LD [M]  /home/fpga/examples/PLBRAM-Ultra96/u-dma-buf/u-dma-buf.ko
make[1]: Leaving directory '/usr/src/linux-headers-4.19.0-xlnx-v2019.2-zynqmp-fpga'
```

## Load u-dma-buf

```console
fpga@debian-fpga:~/examples/PLBRAM-Ultra96/u-dma-buf$ sudo insmod u-dma-buf.ko
```

## Load FPGA and Device Tree

```console
fpga@debian-fpga:~/examples/PLBRAM-Ultra96$ sudo rake install
cp plbram_256k_dbg.bit /lib/firmware/plbram_256k_dbg.bit
./dtbocfg.rb --install plbram_256k --dts plbram_256k_dbg.dts
/tmp/dtovly20200606-4045-1w0q8wf: Warning (unit_address_vs_reg): /fragment@2/__overlay__/udmabuf_plbram: node has a reg or ranges property, but no unit name
/tmp/dtovly20200606-4045-1w0q8wf: Warning (avoid_unnecessary_addr_size): /fragment@2: unnecessary #address-cells/#size-cells without "ranges" or child "reg" property
[  488.106784] fpga_manager fpga0: writing plbram_256k_dbg.bit to Xilinx ZynqMP FPGA Manager
[  488.265700] u-dma-buf udmabuf0: driver version = 2.2.0-rc3
[  488.271274] u-dma-buf udmabuf0: major number   = 241
[  488.276263] u-dma-buf udmabuf0: minor number   = 0
[  488.281065] u-dma-buf udmabuf0: phys address   = 0x0000000400000000
[  488.287333] u-dma-buf udmabuf0: buffer size    = 262144
[  488.292566] u-dma-buf udmabuf0: dma device     = 400000000.udmabuf_plbram
[  488.299354] u-dma-buf udmabuf0: dma coherent   = 0
[  488.304142] u-dma-buf 400000000.udmabuf_plbram: driver installed.
[  488.326764] fclkcfg amba_pl@0:fclk0: driver installed.
[  488.331931] fclkcfg amba_pl@0:fclk0: device name    : amba_pl@0:fclk0
[  488.338396] fclkcfg amba_pl@0:fclk0: clock  name    : pl0_ref
[  488.344160] fclkcfg amba_pl@0:fclk0: clock  rate    : 99999999
[  488.350034] fclkcfg amba_pl@0:fclk0: clock  enabled : 1
[  488.355259] fclkcfg amba_pl@0:fclk0: remove rate    : 1000000
[  488.365686] fclkcfg amba_pl@0:fclk0: remove enable  : 0
```

## Build plbram_test

```console
fpga@debian-fpga:~/examples/PLBRAM-Ultra96$ rake plbram_test
gcc  -o plbram_test plbram_test.c
```

# Run plbram_test

```console
fpga@debian-fpga:~/examples/PLBRAM-Ultra96$ ./plbram_test
size=262144
mmap write test(SIZE=262144, O_SYNC=1) : time = 0.000496 sec
mmap read  test(SIZE=262144, O_SYNC=1) : time = 0.003460 sec
compare = ok
mmap write test(SIZE=262144, O_SYNC=1) : time = 0.000492 sec
mmap read  test(SIZE=262144, O_SYNC=0) : time = 0.000409 sec
compare = ok
mmap write test(SIZE=262144, O_SYNC=0) : time = 0.000289 sec
mmap read  test(SIZE=262144, O_SYNC=1) : time = 0.003468 sec
compare = ok
mmap write test(SIZE=262144, O_SYNC=0) : time = 0.000289 sec
mmap read  test(SIZE=262144, O_SYNC=0) : time = 0.000389 sec
compare = ok
```

# Clean up

```console
fpga@debian-fpga:~/examples/PLBRAM-Ultra96$ sudo rake uninstall
./dtbocfg.rb --remove plbram_256k
[  766.988671] u-dma-buf 400000000.udmabuf_plbram: driver removed.
[  766.995479] fclkcfg amba_pl@0:fclk0: driver unloaded
```

# Build Bitstream file

## Requirement

* Vivado 2019.2

## Download this repository

```console
shell$ git clone https://github.com/ikwzm/PLBRAM-Ultra96
Cloning into 'PLBRAM-Ultra96'...
remote: Enumerating objects: 30, done.
remote: Counting objects: 100% (30/30), done.
remote: Compressing objects: 100% (22/22), done.
remote: Total 30 (delta 7), reused 30 (delta 7), pack-reused 0
Unpacking objects: 100% (30/30), done.
```

## Create Vivado Project

```console
vivado% cd project
vivado% vivado -mode batch -source create_project.tcl
```

## Build Bitstream file

```console
vivado% cd project
vivado% vivado -mode batch -source implementation.tcl
vivado% cp project.runs/impl_1/design_1_wrapper.bit ../plbram_256k_dbg.bit
```

