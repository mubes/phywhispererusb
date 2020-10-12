#include <stdint.h>
#include <stdbool.h>
#include "conf_usb.h"
#include "usb_hid_generic_handler.h"
#include "DAP_config.h"
#include "DAP.h"
#include "DAP_queue.h"

#undef REPORT_LEVEL
#define REPORT_LEVEL 3
#include "generics.h"

static bool _enabled;           // Indicator of if HID GENERIC is currently running
static DAP_queue DAP_Cmd_queue;

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
    DAP_queue_init( &DAP_Cmd_queue );
    return true;
}
/* ====================================================================================== */
void usb_hid_generic_disable( void )
{
    _enabled = false;
}
/* ====================================================================================== */
void usb_hid_generic_set_feature( uint8_t *r )
{

}
/* ====================================================================================== */
bool usb_hid_generic_report_out( void *ptr )

{
    uint8_t *rbuf;
    uint8_t *sbuf;
    int slen;

    DAP_queue_execute_buf( &DAP_Cmd_queue, ptr, 64, &rbuf );

    if ( DAP_queue_get_send_buf( &DAP_Cmd_queue, &sbuf, &slen ) )
    {
        ASSERT( ( slen <= UDI_HID_REPORT_IN_SIZE ) );
        udi_hid_generic_send_report_in( sbuf );
    }
    else
    {
        udi_hid_generic_send_report_in( NULL );
    }

    return true;
}
/* ====================================================================================== */
