#ifndef _USB_MAIN_HANDLER_
#define _USB_MAIN_HANDLER_

#include "conf_usb.h"

/* ====================================================================================== */

/*! \brief Enters the application in low power mode
 * Callback called when USB host sets USB line in suspend state
 */
void usb_main_suspend_action( void );

/*! \brief Turn on a led to notify active mode
 * Called when the USB line is resumed from the suspend state
 */
void usb_main_resume_action( void );

/*! \brief Manages the leds behaviors
 * Called when a start of frame is received on USB line each 1ms.
 */
void usb_main_sof_action( void );
bool usb_main_extra_string( void );
/* ====================================================================================== */

#endif
