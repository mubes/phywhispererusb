# Hey Emacs, this is a -*- makefile -*-
#----------------------------------------------------------------------------
#
# Makefile for ChipWhisperer UDT
#
#----------------------------------------------------------------------------
#
# See Makefile.inc in the support directory for available targets.
# This file contains the project configuration and paths.
#
#----------------------------------------------------------------------------
# Enable for debug configuration
DEBUG=1

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

# Path to linker file
LINKERFILE = $(SUPPORTPATH)/flash-$(PLATFORM).ld

# Path to application source code (our custom code for the app)
SOURCEPATH=source

# Options and target configuration
  # Set target code optimisation level for use in non-debug case
  OPT=2
  # Target platform
  PLATFORM=sam3u2c

#----------------------------------------------------------------------------

# Build tools to be used
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size
AR = arm-none-eabi-ar rcs
NM = arm-none-eabi-nm
SHELL = sh
RM = rm -f --
RMD = rm -rf
MD = mkdir
MP = mkdir -p
ASTYLE=astyle
WINSHELL = cmd

# Flags (note that these are mostly set in the main Makefile.inc)
CFLAGS += -D__SAM3U2C__ -DBOARD=CW610
LDFLAGS +=

# If in debug then override optimisation
ifdef DEBUG
CFLAGS += -DDEBUG
OPT=g
endif

#----------------------------------------------------------------------------

# Source code for our app
include $(SOURCEPATH)/Makefile.inc

# Other files indicating what is to be built
include $(SUPPORTPATH)/Makefile.thirdparty.hal.sam3u

# ...and finally the build rules & compiler options
include $(SUPPORTPATH)/Makefile.inc
