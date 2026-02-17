PLBRAM-Ultra96
=======================================================================

This Repository provides example for uiomem and ZynqMP-FPGA-Linux.

# Requirement

 * Board: Ultra96 or Ultra96-V2
 * OS: ZynqMP-FPGA-Linux
   - Debian13  https://github.com/ikwzm/ZynqMP-FPGA-Debian13/tree/v3.1.0
   - Debian12  https://github.com/ikwzm/ZynqMP-FPGA-Debian12/tree/v7.0.0
   - v2019.2.1 https://github.com/ikwzm/ZynqMP-FPGA-Linux/tree/v2019.2.1
   - v2020.1.1 https://github.com/ikwzm/ZynqMP-FPGA-Linux/tree/v2020.1.1
 * uiomem (v1.0.0-alpha.7) https://github.com/ikwzm/uiomem/tree/v1.0.0-alpha.7

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

## Build uiomem

```console
fpga@debian-fpga:~/examples/PLBRAM-Ultra96$ git submodule init
Submodule 'uiomem' (https://github.com/ikwzm/uiomem.git) registered for path 'uiomem'
fpga@debian-fpga:~/examples/PLBRAM-Ultra96$ git submodule update
Cloning into '/home/fpga/examples/PLBRAM-Ultra96.tmp2/u-dma-buf'...
Cloning into '/home/fpga/examples/PLBRAM-Ultra96.tmp2/uiomem'...
Submodule path 'u-dma-buf': checked out '80e19cb551f6c6f611245433b346212b8e9d17cd'
Submodule path 'uiomem': checked out '52546591eb1c4d6ab0e7ce965d6e4661fdaf81e1'
```

```console
fpga@debian-fpga:~/examples/PLBRAM-Ultra96$ cd uiomem
fpga@debian-fpga:~/examples/PLBRAM-Ultra96/uiomem$ make
make -C /lib/modules/6.12.60-zynqmp-fpga-generic/build ARCH=arm64 CROSS_COMPILE= M=/home/fpga/examples/PLBRAM-Ultra96/uiomem CONFIG_UIOMEM=m modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.60-zynqmp-fpga-generic'
warning: the compiler differs from the one used to build the kernel
  The kernel was built by: aarch64-linux-gnu-gcc (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
  You are using:           gcc (Debian 14.2.0-19) 14.2.0
  CC [M]  /home/fpga/examples/PLBRAM-Ultra96/uiomem/uiomem.o
  MODPOST /home/fpga/examples/PLBRAM-Ultra96/uiomem/Module.symvers
  CC [M]  /home/fpga/examples/PLBRAM-Ultra96/uiomem/uiomem.mod.o
  CC [M]  /home/fpga/examples/PLBRAM-Ultra96/uiomem/.module-common.o
  LD [M]  /home/fpga/examples/PLBRAM-Ultra96/uiomem/uiomem.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.60-zynqmp-fpga-generic'
```

## Load uiomem

```console
fpga@debian-fpga:~/examples/PLBRAM-Ultra96/uiomem$ sudo insmod uiomem.ko
```

## Load FPGA and Device Tree

```console
fpga@debian-fpga:~/examples/PLBRAM-Ultra96$ sudo BASE_DTS=plbram_v2.dts rake install
./dtbocfg.rb --install plbram_256k --dts plbram_256k_dbg.dts
<stdin>:36.18-40.20: Warning (unit_address_vs_reg): /fragment@2/__overlay__/uiomem_plbram: node has a reg or ranges property, but no unit name
<stdin>:27.13-42.5: Warning (avoid_unnecessary_addr_size): /fragment@2: unnecessary #address-cells/#size-cells without "ranges", "dma-ranges" or child "reg" property
[  790.314570] fpga_manager fpga0: writing plbram_256k_dbg.bit to Xilinx ZynqMP FPGA Manager
[  790.693181] OF: overlay: WARNING: memory leak will occur if overlay removed, property: /fpga-region/firmware-name
[  790.706143] uiomem uiomem0: driver version = 1.0.0-alpha.7
[  790.711660] uiomem uiomem0: major number   = 235
[  790.716328] uiomem uiomem0: minor number   = 0
[  790.720838] uiomem uiomem0: range address  = 0x0000000400000000
[  790.726798] uiomem uiomem0: range size     = 262144
[  790.731691] uiomem 400000000.uiomem_plbram: driver installed.
[  790.744420] fclkcfg amba_pl@0:fclk0: driver version : 1.9.1
[  790.750065] fclkcfg amba_pl@0:fclk0: device name    : amba_pl@0:fclk0
[  790.756572] fclkcfg amba_pl@0:fclk0: clock  name    : pl0_ref
[  790.762370] fclkcfg amba_pl@0:fclk0: clock  rate    : 99999999
[  790.768307] fclkcfg amba_pl@0:fclk0: clock  enabled : 1
[  790.773558] fclkcfg amba_pl@0:fclk0: remove rate    : 1000000
[  790.779317] fclkcfg amba_pl@0:fclk0: remove enable  : 0
[  790.784560] fclkcfg amba_pl@0:fclk0: driver installed.
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
mmap write test : sync=1 time=0.000530 sec (0.000530 sec)
mmap read  test : sync=1 time=0.003468 sec (0.003468 sec)
compare = ok
mmap write test : sync=0 time=0.000475 sec (0.000301 sec)
mmap read  test : sync=1 time=0.003495 sec (0.003495 sec)
compare = ok
mmap write test : sync=1 time=0.000531 sec (0.000530 sec)
mmap read  test : sync=0 time=0.000396 sec (0.000300 sec)
compare = ok
mmap write test : sync=0 time=0.000419 sec (0.000246 sec)
mmap read  test : sync=0 time=0.000342 sec (0.000299 sec)
compare = ok
file write test : sync=1 time=0.000269 sec (0.000269 sec)
mmap read  test : sync=0 time=0.000345 sec (0.000300 sec)
compare = ok
file write test : sync=0 time=0.000290 sec (0.000267 sec)
mmap read  test : sync=0 time=0.000341 sec (0.000300 sec)
compare = ok
mmap write test : sync=0 time=0.000380 sec (0.000239 sec)
file read  test : sync=1 time=0.000400 sec (0.000399 sec)
compare = ok
mmap write test : sync=0 time=0.000419 sec (0.000247 sec)
file read  test : sync=0 time=0.000379 sec (0.000338 sec)
compare = ok
```

# Clean up

```console
fpga@debian-fpga:~/examples/PLBRAM-Ultra96$ sudo rake uninstall
./dtbocfg.rb --remove plbram_256k
[  932.968554] uiomem 400000000.uiomem_plbram: driver removed.
[  932.974863] fclkcfg amba_pl@0:fclk0: driver removed.
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

