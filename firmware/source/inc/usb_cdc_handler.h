#include "conf_usb.h"

#ifndef _USB_CDC_HANDLER_H_
#define _USB_CDC_HANDLER_H_

/* ====================================================================================== */

bool usb_cdc_enable( uint8_t port );
void usb_cdc_disable( uint8_t port );
void usb_cdc_set_dtr( uint8_t port, bool b_enable );
void usb_cdc_uart_rx_notify( uint8_t port );
void usb_cdc_uart_config( uint8_t port, usb_cdc_line_coding_t *cfg );

/* ====================================================================================== */

#endif
