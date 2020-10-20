# Firmware for SAM3U

This directory includes the firmware for the SAM3U of the PhyWhisperer-UDT.

## Build
To build, run `make` in this directory. Magic will happen.

## Prerequisites
You will need the gcc-arm-none-eabi installed for your system (Linux, Windows, etc). This can be obtained from [ARM GCC](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm). . The latest version tested is `gcc version 9.2.1 20191025 (release)` on which it builds without warnings.

Day to day this code is built with codium (the open source version of Visual Studio Code) on Linux and a suitable codium workspace is provided in `support/pw.ws.code-workspace`. You'll need `cortex-debug` active too. The provided configuration supports both [JLink](https://www.segger.com/products/debug-probes/j-link/)  and [Blackmagic](https://github.com/blacksphere/blackmagic) debuggers, with [Orbuculum](https://github.com/orbcode/orbuculum) integration for both (although you probably will need to edit the path to your `gdbtrace.init` script).

OTOH, It's just makefile based so should work with anything.

# Directory Structure
The contents of each of the directories is;

```
   firmware
      |
      +--- config:     Subdirectories containing configuration (i.e. editable elements) 
      |                and Makefiles for each of the third party modules.
      +--- fpga:       Prebuilt bitfiles for the fpga.
      +--- libs:       NewAE created library code to support development.
      +--- source:     NewAE application source.
      +--- support:    Supporting files and scripts to allow the application to be built.
      +--- thirdparty: Code originating from outside of the project.
              +--- hal: HAL files for the supported processor(s)
```
The build process creates a number of artefacts;
```
   firmware
      |
      +--- objdir: Dynamically created build scratchpad area.
      +--- .dep:   Dynamically created dependency files.
      :
      : build output files.
```
The philosophy behind this layout is to keep everything logically separate and so that thirdparty code can be incorporated without edits (and thus can be updated to a later version) by means of separating its configuration from the code itself.  If you _do_ need to modify code in a thirdparty module then move that code into the right part of the config tree so it doesn't get whacked when the thirdparty code is updated.

# Using
You _must_ load a bitfile into the FPGA before you do much with the device. This is most easily done with something like the following;
```
$cd phwhisperer/software
$python3
Python 3.8.5 (default, Jul 28 2020, 12:59:40) 
[GCC 9.3.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import phywhisperer.usb as pw
>>> p = pw.Usb()
>>> p.con()
```
After this the red error led will stop flashing and the blue leds will go out too. You are, of course, able to use the whole of the phywhisperer-usb functionality from this point, but we're assuming you're here 'cos you want to use the cmsis-dap capabilities. Install `pyocd` and then, for the example of a bluepill board;
```
$ pyocd erase --chip -u <your phywhisperer id> -t STM32F103RC
0001477:INFO:eraser:Erasing chip...
0001527:INFO:eraser:Done
```
Congratulations, it's working. Now to use it for gdb;
```
$ pyocd gdbserver -u <your phywhisperer id> -t stm32f103rc --persist 
0001400:INFO:board:Target type is stm32f103rc
0001475:INFO:dap:DP IDR = 0x1ba01477 (v1 rev1)
0001485:INFO:ap:AHB-AP#0 IDR = 0x14770011 (AHB-AP var1 rev1)
0001528:INFO:rom_table:AHB-AP#0 Class 0x1 ROM table #0 @ 0xe00ff000 (designer=020 part=410)
0001532:INFO:rom_table:[0]<e000e000:SCS-M3 class=14 designer=43b part=000>
0001534:INFO:rom_table:[1]<e0001000:DWT class=14 designer=43b part=002>
0001537:INFO:rom_table:[2]<e0002000:FPB class=14 designer=43b part=003>
0001539:INFO:rom_table:[3]<e0000000:ITM class=14 designer=43b part=001>
0001541:INFO:rom_table:[4]<e0040000:TPIU-M3 class=9 designer=43b part=923 devtype=11 archid=0000 devid=ca0:0:0>
0001542:INFO:cortex_m:CPU core #0 is Cortex-M3 r1p1
0001546:INFO:dwt:4 hardware watchpoints
0001548:INFO:fpb:6 hardware breakpoints, 4 literal comparators
0001556:INFO:server:Semihost server started on port 4444 (core 0)
0001580:INFO:gdbserver:GDB server started on port 3333 (core 0)
```
You can now start gdb elsewhere and connect to it, something like this;
```
$arm-none-eabi-gdb
GNU gdb (GNU Tools for Arm Embedded Processors 9-2019-q4-major) 8.3.0.20190709-git
Copyright (C) 2019 Free Software Foundation, Inc.

<SNIP>

(gdb) target extended-remote localhost:3333
Remote debugging using localhost:3333
warning: No executable has been specified and target does not support
determining executable automatically.  Try using the "file" command.
0xfffffffe in ?? ()
(gdb) file ofiles/firmware.elf 
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Reading symbols from ofiles/firmware.elf...
(gdb) load
Loading section .isr_vector, size 0x10c lma 0x8000000
Loading section .inits, size 0x28 lma 0x800010c
Loading section .text, size 0xaa14 lma 0x8000140
Loading section .data, size 0x364 lma 0x800ab54
Start address 0x80095d4, load size 44716
Transfer rate: 18 KB/sec, 1788 bytes/write.
(gdb) c
Continuing.
^C[New Thread 536873720]
[New Thread 536877872]
[New Thread 536873464]
[New Thread 536872168]
[New Thread 536876056]
[New Thread 536874648]

Thread 2 "IDLE" received signal SIGINT, Interrupt.
[Switching to Thread 536873720]
0x08001d06 in prvCheckTasksWaitingTermination () at thirdparty/FreeRTOS/src/tasks.c:3420
3420	{
(gdb)
```

# Adding new code
If you add a new thirdparty subsystem this should go under `thirdparty` with its configuration under a subdirectory of `config` and makefiles in `support`, and integrated into the top level makefile.
If you add new core code this obviously goes in the source directory, but consider splitting it up into subdirectories if the changes are significant and easily partitioned. Standard libraries can go under `libs` although it's an exercise for the reader to decide if a particular chunk of code belongs under `libs` or `thirdparty`...mostly that decision is size dependent.

# Licences 
All code is licenced according to files and headers you will find in the various files. Code which is specific to Phywhisperer-UDT is licenced under GPL v2 or later.
