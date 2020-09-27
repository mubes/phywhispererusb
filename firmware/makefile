# Hey Emacs, this is a -*- makefile -*-
#----------------------------------------------------------------------------
#
# Makefile for ChipWhisperer UDT
#
#----------------------------------------------------------------------------
# On command line:
#
# make all = Make software.
#
# make clean = Clean out built project files.
#
# make coff = Convert ELF to AVR COFF.
#
# make extcoff = Convert ELF to AVR Extended COFF.
#
# make program = Download the hex file to the device, using avrdude.
#                Please customize the avrdude settings below first!
#
# make debug = Start either simulavr or avarice as specified for debugging, 
#              with avr-gdb or avr-insight as the front end for debugging.
#
# make filename.s = Just compile filename.c into the assembler code only.
#
# make filename.i = Create a preprocessed source file for use in submitting
#                   bug reports to the GCC project.
#
# To rebuild project do "make clean" then "make all".
#----------------------------------------------------------------------------

# Path to support files (meta-files for building the app)
SUPPORTPATH=support

# Path to third party stuff (external code that does not change)
THIRDPARTY=thirdparty

# Path to hal files (standard multi-project files that don't change per app)
HALPATH=$(THIRDPARTY)/hal

# Path to config (files relating to app that setup hal or thirdparty materials)
CONFIG=config

# Target file name (without extension). This is the base name of the compiled .hex file.
TARGET = phywhisperer

# Build files directory. This is where the compiler generated intermediates go
OBJDIR = objdir

# Path to application source code (our custom code for the app)
SOURCEPATH=source

# Options and target configuration
  # Set target code optimisation level
  OPT=0
  # Enable for debug configuration
  DEBUG=1
  # Target platform
  PLATFORM=sam3u2c

#----------------------------------------------------------------------------

# Source code for our app
include $(SOURCEPATH)/Makefile.inc

# Other files indicating what is to be built
include $(SUPPORTPATH)/Makefile.thirdparty.hal.sam3u

# ...and finally the build rules
include $(SUPPORTPATH)/Makefile.inc
