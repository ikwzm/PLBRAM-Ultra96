PLBRAM-Ultra96
=======================================================================

This Repository provides example for uiomem and ZynqMP-FPGA-Linux.

# Requirement

 * Board: Ultra96
 * OS: ZynqMP-FPGA-Linux v2019.2.1 ([https://github.com/ikwzm/ZynqMP-FPGA-Linuxtree/v2019.2.1])
 * uiomem (0.0.4) ([https://github.com/ikwzm/uiomem/tree/develop])

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
fpga@debian-fpga:~/examples/PLBRAM-Ultra96$ cd uiomem
fpga@debian-fpga:~/examples/PLBRAM-Ultra96/uiomem$ make
make -C /lib/modules/5.4.0-xlnx-v2020.1-zynqmp-fpga/build ARCH=arm64 CROSS_COMPILE= M=/home/fpga/examples/PLBRAM-Ultra96/uiomem obj-m=uiomem.o uiomem.ko
make[1]: Entering directory '/usr/src/linux-headers-5.4.0-xlnx-v2020.1-zynqmp-fpga'
  CC [M]  /home/fpga/examples/PLBRAM-Ultra96/uiomem/uiomem.o
  Building modules, stage 2.
  MODPOST 1 modules
  CC [M]  /home/fpga/examples/PLBRAM-Ultra96/uiomem/uiomem.mod.o
  LD [M]  /home/fpga/examples/PLBRAM-Ultra96/uiomem/uiomem.ko
make[1]: Leaving directory '/usr/src/linux-headers-5.4.0-xlnx-v2020.1-zynqmp-fpga'
```

## Load uiomem

```console
fpga@debian-fpga:~/examples/PLBRAM-Ultra96/uiomem$ sudo insmod uiomem.ko
```

## Load FPGA and Device Tree

```console
fpga@debian-fpga:~/examples/PLBRAM-Ultra96$ sudo rake install
cp plbram_256k_dbg.bit /lib/firmware/plbram_256k_dbg.bit
./dtbocfg.rb --install plbram_256k --dts plbram_256k_dbg.dts
/tmp/dtovly20200606-4045-1w0q8wf: Warning (unit_address_vs_reg): /fragment@2/__overlay__/udmabuf_plbram: node has a reg or ranges property, but no unit name
/tmp/dtovly20200606-4045-1w0q8wf: Warning (avoid_unnecessary_addr_size): /fragment@2: unnecessary #address-cells/#size-cells without "ranges" or child "reg" property
[21358.448627] fpga_manager fpga0: writing plbram_256k_dbg.bit to Xilinx ZynqMP FPGA Manager
[21358.597670] OF: overlay: WARNING: memory leak will occur if overlay removed, property: /fpga-full/firmware-name
[21358.610363] fclkcfg amba_pl@0:fclk0: driver installed.
[21358.615566] fclkcfg amba_pl@0:fclk0: device name    : amba_pl@0:fclk0
[21358.622028] fclkcfg amba_pl@0:fclk0: clock  name    : pl0_ref
[21358.627778] fclkcfg amba_pl@0:fclk0: clock  rate    : 99999999
[21358.633628] fclkcfg amba_pl@0:fclk0: clock  enabled : 1
[21358.638850] fclkcfg amba_pl@0:fclk0: remove rate    : 1000000
[21358.644592] fclkcfg amba_pl@0:fclk0: remove enable  : 0
[21358.650861] uiomem uiomem0: driver version = 0.0.1
[21358.655663] uiomem uiomem0: major number   = 508
[21358.660282] uiomem uiomem0: minor number   = 0
[21358.664722] uiomem uiomem0: range address  = 0x0000000400000000
[21358.670645] uiomem uiomem0: range size     = 262144
[21358.675519] uiomem 400000000.uiomem_plbram: driver installed.
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

