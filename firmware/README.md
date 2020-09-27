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

# Adding new code
If you add a new thirdparty subsystem this should go under `thirdparty` with its configuration under a subdirectory of `config` and makefiles in `support`, and integrated into the top level makefile.
If you add new core code this obviously goes in the source directory, but consider splitting it up into subdirectories if the changes are significant and easily partitioned. Standard libraries can go under `libs` although it's an exercise for the reader to decide if a particular chunk of code belongs under `libs` or `thirdparty`...mostly that decision is size dependent.

# Licences 
All code is licenced according to files and headers you will find in the various files. Code which is specific to Phywhisperer-UDT is licenced under GPL v2 or later.
