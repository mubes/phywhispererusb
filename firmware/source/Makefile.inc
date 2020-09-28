# -----------------------------------------------------------------------------

# List C application source files here.
# Header files (.h) are automatically pulled in.

# Handlers for each type of USB message
APPSRC += usb_main_handler.c
APPSRC += usb_vendor_handler.c
APPSRC += usb_cdc_handler.c
APPSRC += usb_hid_generic_handler.c

# PHY Hardware drvier
APPSRC += phyw_driver.c

# USB transaction memory
APPSRC += usb_xmem.c

# FPGA Access
APPSRC += fpga_program.c

# Main app
APPSRC += main.c

SRC += $(patsubst %,$(SOURCEPATH)/%,$(APPSRC))

# -----------------------------------------------------------------------------
