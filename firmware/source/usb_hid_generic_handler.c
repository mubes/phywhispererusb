#include <stdint.h>
#include <stdbool.h>
#include "conf_usb.h"
#include "usb_hid_generic_handler.h"
#include "generics.h"

static bool _enable;     // Indicator of if HID GENERIC is currently running

/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
/* Publically accessible routines                                                         */
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
bool usb_hid_generic_enable( void )
{
    _enable = true;
    DBG( "Generic enable" EOL );
    return true;
}
/* ====================================================================================== */
void usb_hid_generic_disable( void )
{
    DBG( "Generic disable" EOL );
    _enable = false;
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
