# -----------------------------------------------------------------------------

# List C source files here.
# Header files (.h) are automatically pulled in.

APPSRC += usb_main_handler.c
APPSRC += usb_vendor_handler.c
APPSRC += usb_cdc_handler.c
APPSRC += usb_hid_generic_handler.c

APPSRC += phyw_driver.c
APPSRC += main.c 
APPSRC += usb_xmem.c fpga_program.c
APPSRC += report.c ui_cw610.c

SRC += $(patsubst %,$(SOURCEPATH)/%,$(APPSRC))

# -----------------------------------------------------------------------------
