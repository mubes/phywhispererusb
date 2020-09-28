
#include <stdint.h>
#include <stdbool.h>
#include "usb_cdc_handler.h"
#include "report.h"

/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
/* Publically accessible routines                                                         */
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
bool usb_cdc_enable( uint8_t port )
{
    DBG( "CDC Enable" EOL );
    return true;
}
/* ====================================================================================== */
void usb_cdc_disable( uint8_t port )
{
    DBG( "CDC Disable" EOL );
}
/* ====================================================================================== */
void usb_cdc_set_dtr( uint8_t port, bool b_enable )
{
    DBG( "Set DTR %d" EOL, b_enable );
}
/* ====================================================================================== */
void usb_cdc_uart_rx_notify( uint8_t port )
{

}
/* ====================================================================================== */
void usb_cdc_uart_config( uint8_t port, usb_cdc_line_coding_t *cfg )
{
    uint32_t stopbits, parity, databits;
    UNUSED( port );
    UNUSED( stopbits );
    UNUSED( databits );
    UNUSED( parity );

    switch ( cfg->bCharFormat )
    {
        case CDC_STOP_BITS_2:
            stopbits = US_MR_NBSTOP_2_BIT;
            break;

        case CDC_STOP_BITS_1_5:
            stopbits = US_MR_NBSTOP_1_5_BIT;
            break;

        case CDC_STOP_BITS_1:
        default:
            // Default stop bit = 1 stop bit
            stopbits = US_MR_NBSTOP_1_BIT;
            break;
    }

    switch ( cfg->bParityType )
    {
        case CDC_PAR_EVEN:
            parity = US_MR_PAR_EVEN;
            break;

        case CDC_PAR_ODD:
            parity = US_MR_PAR_ODD;
            break;

        case CDC_PAR_MARK:
            parity = US_MR_PAR_MARK;
            break;

        case CDC_PAR_SPACE:
            parity = US_MR_PAR_SPACE;
            break;

        default:
        case CDC_PAR_NONE:
            parity = US_MR_PAR_NO;
            break;
    }

    switch ( cfg->bDataBits )
    {
        case 5:
        case 6:
        case 7:
            databits = cfg->bDataBits - 5;
            break;

        default:
        case 8:
            databits = US_MR_CHRL_8_BIT;
            break;
    }
}
/* ====================================================================================== */
