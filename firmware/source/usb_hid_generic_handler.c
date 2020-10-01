#include <stdint.h>
#include <stdbool.h>
#include "conf_usb.h"
#include "usb_hid_generic_handler.h"

#define REPORT_LEVEL 4
#include "generics.h"

static bool _enabled;     // Indicator of if HID GENERIC is currently running

/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
/* Publically accessible routines                                                         */
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
bool usb_hid_generic_enable( void )
{
    _enabled = true;
    DBG( "Generic enable" EOL );
    return true;
}
/* ====================================================================================== */
void usb_hid_generic_disable( void )
{
    DBG( "Generic disable" EOL );
    _enabled = false;
}
/* ====================================================================================== */
void usb_hid_generic_set_feature( uint8_t *r )
{
    DBG( "Set feature" EOL );
}
/* ====================================================================================== */
bool usb_hid_generic_report_out( void *ptr )

{
    DBG( "HID Report out" EOL );
    return true;
}
/* ====================================================================================== */
