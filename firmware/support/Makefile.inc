# Hey Emacs, this is a -*- makefile -*-
#----------------------------------------------------------------------------
#
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

# Make things quiet unless we've been told not to
ifdef VERBOSE
cmd = $1
Q :=
else
cmd = @$(if $(value 2),echo "$2";)$1
Q := @
endif

# Add the platform to the output filenames
ifeq ($(TARGET-PLAT),)
TARGET-PLAT = $(TARGET)-$(PLATFORM)
endif

# Object files directory
#     To put object files in current directory, use a dot (.), do NOT make
#     this an empty or blank macro!
OBJDIR ?= objdir

##########################################################################
##########################################################################

# Optimization level, can be [0, 1, 2, 3, s]. 
#     0 = turn off optimization. s = optimize for size.
#     (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
  OPT ?= 0

# Debugging format.
#     Native formats for AVR-GCC's -g are dwarf-2 [default] or stabs.
#     AVR Studio 4.10 requires dwarf-2.
#     AVR [Extended] COFF format requires stabs, plus an avr-objcopy run.
DEBUGTYPE ?= dwarf-2

# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRAINCDIRS += $(patsubst %,%/inc,$(SOURCEPATH))

# Compiler flag to set the C Standard level.
#     c89   = "ANSI" C
#     gnu89 = c89 plus GCC extensions
#     c99   = ISO C99 standard (not yet fully implemented)
#     gnu99 = c99 plus GCC extensions
CSTANDARD = -std=gnu99

# Place -D or -U options here for C sources
CDEFS += -DF_CPU=$(F_CPU)UL

# Place -D or -U options here for ASM sources
ADEFS += -DF_CPU=$(F_CPU)

# Place -D or -U options here for C++ sources
CPPDEFS += -DF_CPU=$(F_CPU)UL
#CPPDEFS += -D__STDC_LIMIT_MACROS
#CPPDEFS += -D__STDC_CONSTANT_MACROS



#---------------- Compiler Options C ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CFLAGS += -g$(DEBUGTYPE)
CFLAGS += $(CDEFS)
CFLAGS += -O$(OPT)
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -fshort-enums
CFLAGS += -fdata-sections
CFLAGS += -ffunction-sections
CFLAGS += -Wall
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wmissing-prototypes
CFLAGS += -Wchar-subscripts
CFLAGS += -Wformat=2 
CFLAGS += -mlong-calls
CFLAGS += -fno-strict-aliasing
CFLAGS += -Wcomment
CFLAGS += -Wundef
CFLAGS += -Wunreachable-code
CFLAGS += -Wsign-compare
CFLAGS += -Wa,-adhlns=$(addprefix $(OBJDIR)/,$(notdir $(<:%.c=%.lst)))
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
CFLAGS += $(CSTANDARD)

MCU_FLAGS = -mcpu=cortex-m3

#---------------- Compiler Options C++ ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CPPFLAGS += -g$(DEBUGTYPE)
CPPFLAGS += $(CPPDEFS)
CPPFLAGS += -O$(OPT)
CPPFLAGS += -funsigned-char
CPPFLAGS += -funsigned-bitfields
CPPFLAGS += -fpack-struct
CPPFLAGS += -fshort-enums
CPPFLAGS += -fno-exceptions
CPPFLAGS += -Wall
CPPFLAGS += -Wundef
CPPFLAGS += -Wstrict-prototypes
CPPFLAGS += -Wunreachable-code
CPPFLAGS += -Wsign-compare
CPPFLAGS += -Wa,-adhlns=$(addprefix $(OBJDIR)/,$(notdir $(<:%.cpp=%.lst)))
CPPFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))


#---------------- Assembler Options ----------------
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -adhlns:   create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
#  -listing-cont-lines: Sets the maximum number of continuation lines of hex 
#       dump that will be displayed for a given single line of source input.

#-adhlns=$(<:%.S=$(OBJDIR)/%.lst),
#,--listing-cont-lines=100

ASFLAGS += $(ADEFS) -Wa,-gstabs,-adhlns=$(addprefix $(OBJDIR)/,$(notdir $(<:%.S=%.lst)))
ASFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))


#---------------- Library Options ----------------
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

# If this is left blank, then it will use the Standard printf version.
PRINTF_LIB = 
#PRINTF_LIB = $(PRINTF_LIB_MIN)
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)


# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB = 
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)


#MATH_LIB ?= -lm


# List any extra directories to look for libraries here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRALIBDIRS += 

#---------------- External Memory Options ----------------

EXTMEMOPTS +=



#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS += -Wl,-Map=$(TARGET-PLAT).map,--cref

LDFLAGS += -mthumb
LDFLAGS += -Wl,--start-group -L $(HALPATH)/$(PLATFORM)/ -lm -Wl,--end-group
LDFLAGS += -T $(LINKERFILE)
LDFLAGS += -Wl,--gc-sections -Wl,--entry=Reset_Handler -Wl,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(patsubst %,-L%,$(EXTRALIBDIRS))
LDFLAGS += $(MATH_LIB)
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB)

#============================================================================

#Depending on if echo is unix or windows, they respond differently to no arguments. Windows will annoyingly
#print "echo OFF", so instead we're forced to give it something to echo. The windows one will also print
#passed ' or " symbols, so we use a . as it's pretty small...
ifeq ($(OS),Windows_NT)
ECHO_BLANK ?= echo:
else
ECHO_BLANK ?=
endif


# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_SIZE_BEFORE = Size before: 
MSG_SIZE_AFTER = Size after:
MSG_COFF = Converting to AVR COFF:
MSG_EXTENDED_COFF = Converting to AVR Extended COFF:
MSG_FLASH = Creating load file for Flash:
MSG_BIN = Creating binary:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking
MSG_COMPILING = Compiling C:
MSG_COMPILING_CPP = Compiling C++:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project
MSG_CREATING_LIBRARY = Creating library

# Define all object files.
OBJ = $(SRC:%.c=$(OBJDIR)/%.o) $(CPPSRC:%.cpp=$(OBJDIR)/%.o) $(ASRC:%.S=$(OBJDIR)/%.o) 

# Define all listing files.
LST = $(SRC:%.c=$(OBJDIR)/%.lst) $(CPPSRC:%.cpp=$(OBJDIR)/%.lst) $(ASRC:%.S=$(OBJDIR)/%.lst) 


# Compiler flags to generate dependency files.
GENDEPFLAGS = -MMD -MP -MF .dep/$(@F).d

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = $(MCU_FLAGS) -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_CPPFLAGS = $(MCU_FLAGS) -I. -x c++ $(CPPFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = $(MCU_FLAGS) -I. -x assembler-with-cpp $(ASFLAGS)

##################################################################################################
##################################################################################################
##################################################################################################

# Default target.
all: .dep gccversion build sizeafter

# Change the build target to build a HEX file or a library.
build: elf hex eep lss sym bin

elf: $(TARGET-PLAT).elf
hex: $(TARGET-PLAT).hex
eep: $(TARGET-PLAT).eep
lss: $(TARGET-PLAT).lss
sym: $(TARGET-PLAT).sym
bin: $(TARGET-PLAT).bin
LIBNAME=lib$(TARGET-PLAT).a
lib: $(LIBNAME)

# Display size of file.
HEXSIZE = $(SIZE) --target=ihex $(TARGET-PLAT).hex

# Note: custom ELFSIZE command can be specified in Makefile.platform
# See avr/Makefile.avr for example
ifeq ($(ELFSIZE),)
  ELFSIZE = $(SIZE) $(TARGET-PLAT).elf
endif

sizeafter: build
	@echo $(MSG_SIZE_AFTER)
	@$(ELFSIZE)

$(OBJDIR):
	$(Q)$(MD) $(OBJDIR)

.dep:
	$(Q)$(MD) .dep

# Display compiler version information.
gccversion :
	$(Q)@$(CC) --version



# Program the device.  
program: $(TARGET-PLAT).hex $(TARGET-PLAT).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)


# Generate avr-gdb config/init file which does the following:
#     define the reset signal, load the target file, connect to target, and set 
#     a breakpoint at main().
gdb-config: 
	@$(RM) $(GDBINIT_FILE)
	@echo define reset >> $(GDBINIT_FILE)
	@echo SIGNAL SIGHUP >> $(GDBINIT_FILE)
	@echo end >> $(GDBINIT_FILE)
	@echo file $(TARGET-PLAT).elf >> $(GDBINIT_FILE)
	@echo target remote $(DEBUG_HOST):$(DEBUG_PORT)  >> $(GDBINIT_FILE)
ifeq ($(DEBUG_BACKEND),simulavr)
	@echo load  >> $(GDBINIT_FILE)
endif
	@echo break main >> $(GDBINIT_FILE)

debug: gdb-config $(TARGET-PLAT).elf
ifeq ($(DEBUG_BACKEND), avarice)
	@echo Starting AVaRICE - Press enter when "waiting to connect" message displays.
	@$(WINSHELL) /c start avarice --jtag $(JTAG_DEV) --erase --program --file \
	$(TARGET-PLAT).elf $(DEBUG_HOST):$(DEBUG_PORT)
	@$(WINSHELL) /c pause

else
	@$(WINSHELL) /c start simulavr --gdbserver --device $(MCU) --clock-freq \
	$(DEBUG_MFREQ) --port $(DEBUG_PORT)
endif
	@$(WINSHELL) /c start avr-$(DEBUG_UI) --command=$(GDBINIT_FILE)




# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT = $(OBJCOPY) --debugging
COFFCONVERT += --change-section-address .data-0x800000
COFFCONVERT += --change-section-address .bss-0x800000
COFFCONVERT += --change-section-address .noinit-0x800000
COFFCONVERT += --change-section-address .eeprom-0x810000



coff: $(TARGET-PLAT).elf
	@$(ECHO_BLANK)
	@echo $(MSG_COFF) $(TARGET-PLAT).cof
	$(Q)$(COFFCONVERT) -O coff-avr $< $(TARGET-PLAT).cof


extcoff: $(TARGET-PLAT).elf
	@$(ECHO_BLANK)
	@echo $(MSG_EXTENDED_COFF) $(TARGET-PLAT).cof
	$(Q)$(COFFCONVERT) -O coff-ext-avr $< $(TARGET-PLAT).cof



# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	@$(ECHO_BLANK)
	@echo $(MSG_FLASH) $@
	$(Q)$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

%.bin: %.hex
	@$(ECHO_BLANK)
	@echo $(MSG_BIN) $@
	$(Q)$(OBJCOPY) --input-target=ihex --output-target=binary $< $@

%.eep: %.elf
	@$(ECHO_BLANK)
	@echo $(MSG_EEPROM) $@
	$(Q)$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 --no-change-warnings -O ihex $< $@ || exit 0

# Create extended listing file from ELF output file.
%.lss: %.elf
	@$(ECHO_BLANK)
	@echo $(MSG_EXTENDED_LISTING) $@
	$(Q)$(OBJDUMP) -h -S -z $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@$(ECHO_BLANK)
	@echo $(MSG_SYMBOL_TABLE) $@
	$(Q)$(NM) -n $< > $@



# Create library from object files.
.SECONDARY : $(TARGET-PLAT).a
.PRECIOUS : $(OBJ)
%.a: $(OBJ)
	@$(ECHO_BLANK)
	@echo $(MSG_CREATING_LIBRARY) $@
	$(Q)$(AR) $@ $(OBJ)


# Link: create ELF output file from object files.
.SECONDARY : $(TARGET-PLAT).elf
.PRECIOUS : $(OBJ)
%.elf: $(OBJ)
	@$(ECHO_BLANK)
	@echo $(MSG_LINKING) $@
	$(Q)$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)


# Compile: create object files from C source files.
$(OBJDIR)/%.o : %.c clean_objs $(OBJDIR) .dep gccversion
	@$(ECHO_BLANK)
	@echo $(MSG_COMPILING) $<
	$(Q)$(MP) $(basename $@)
	$(Q)$(CC) -c $(ALL_CFLAGS) $< -o $@
	$(Q)$(CC) -S $(ALL_CFLAGS) $< -o $@.s


# Compile: create object files from C++ source files.
$(OBJDIR)/%.o : %.cpp clean_objs $(OBJDIR) .dep gccversion
	@$(ECHO_BLANK)
	$(Q)$(MP) $(basename $@)
	@echo $(MSG_COMPILING_CPP) $<
	$(Q)$(CC) -c $(ALL_CPPFLAGS) $< -o $@ 


# Compile: create assembler files from C source files.
%.s : %.c
	$(Q)$(MP) $(basename $@)
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Compile: create assembler files from C++ source files.
%.s : %.cpp
	$(Q)$(MP) $(basename $@)
	$(CC) -S $(ALL_CPPFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
$(OBJDIR)/%.o : %.S clean_objs $(OBJDIR) .dep gccversion
	@$(ECHO_BLANK)
	@echo $(MSG_ASSEMBLING) $<
	$(Q)$(CC) -c $(ALL_ASFLAGS) $< -o $@


# Create preprocessed source for use in sending a bug report.
%.i : %.c
	$(Q)$(MP) $(basename $@)
	$(Q)$(CC) -E $(MCU_FLAGS) -I. $(CFLAGS) $< -o $@ 

# Clean all object files specific to this platform
clean_objs :
	$(Q)$(RMD) $(OBJDIR)
	$(Q)$(RM) $(SRC:.c=.s)
	$(Q)$(RM) $(SRC:.c=.d)
	$(Q)$(RM) $(SRC:.c=.i)

# Target: clean project.
clean: clean_objs
	@$(ECHO_BLANK)
	@echo $(MSG_CLEANING)
	$(Q)$(RM) $(TARGET-PLAT).hex
	$(Q)$(RM) $(TARGET-PLAT).eep
	$(Q)$(RM) $(TARGET-PLAT).cof
	$(Q)$(RM) $(TARGET-PLAT).elf
	$(Q)$(RM) $(TARGET-PLAT).map
	$(Q)$(RM) $(TARGET-PLAT).sym
	$(Q)$(RM) $(TARGET-PLAT).lss
	$(Q)$(RM) $(TARGET-PLAT).bin

clean_all : clean
	$(Q)$(RMD) .dep

pretty:
	$(Q)-$(ASTYLE) --options=$(SUPPORTPATH)/astyle.conf -i "$(SOURCEPATH)/inc/*.h" "$(SOURCEPATH)/*.c" "$(LIBRARYPATH)/*.c" "$(LIBRARYPATH)/inc/*.h"

# Include the dependency files.
-include $(wildcard .dep/*)


# Listing of phony targets.
.PHONY : all finish sizeafter gccversion \
build elf hex eep lss sym coff extcoff \
clean clean_list clean_print clean_objs program debug gdb-config

# saveplatform: Save the platform into the file Makefile.target
saveplatform:
	-@rm -f Makefile.platform
	@echo "Saving Makefile.platform"
	@echo >Makefile.platform "PLATFORM = $(PLATFORM)"

print-%:
	@echo $* is $($*)
