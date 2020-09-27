# -----------------------------------------------------------------------------

# List C source files here.
# Header files (.h) are automatically pulled in.
APPSRC += main.c usb.c usb_xmem.c fpga_program.c
APPSRC += report.c ui_cw610.c

SRC += $(patsubst %,$(SOURCEPATH)/%,$(APPSRC))

# -----------------------------------------------------------------------------
