#ifndef _USB_HID_GENERIC_HANDLER_H_
#define _USB_HID_GENERIC_HANDLER_H_

#include <stdint.h>
#include <stdbool.h>

/* ====================================================================================== */

bool usb_hid_generic_enable(void);
void usb_hid_generic_disable(void);
void usb_hid_generic_set_feature(uint8_t* r);
bool usb_hid_generic_report_out( void *ptr);

/* ====================================================================================== */
#endif
