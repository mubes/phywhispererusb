
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "usb_cdc_handler.h"

#define REPORT_LEVEL 4
#include "generics.h"

static usb_cdc_line_coding_t _storedCoding;
static bool _deviceOpen;

/* For some reason these defines aren't in the cdc file, although the routines are... */
void udi_cdc_data_enable( void );
void udi_cdc_data_disable( void );

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
    udi_cdc_multi_ctrl_signal_dcd( port, true );
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
    _deviceOpen = b_enable;
    udi_cdc_multi_ctrl_signal_dsr( port, b_enable );

    if ( b_enable )
    {
        udi_cdc_data_enable();
        udi_cdc_write_buf( usb_cdc_uart_tx_notify, 512 );
    }
    else
    {
        udi_cdc_data_disable();
    }
}
/* ====================================================================================== */
void usb_cdc_uart_rx_notify( uint8_t port )
{
    //DBG( "RX Notify %d" EOL,port );
    while ( udi_cdc_is_rx_ready() )
    {
        udi_cdc_getc();
    }
}
/* ====================================================================================== */
void usb_cdc_uart_tx_notify( uint8_t port )
{
    //DBG( "TX Notify" EOL );
    udi_cdc_multi_write_buf( 0, usb_cdc_uart_tx_notify, 512 );
}
/* ====================================================================================== */
void usb_cdc_uart_config( uint8_t port, usb_cdc_line_coding_t *cfg )
{
    DBG( "CDC Config delivered" EOL );
    memcpy( &_storedCoding, cfg, sizeof( usb_cdc_line_coding_t ) );
}
/* ====================================================================================== */
bool usb_cdc_has_room( iram_size_t size )

{
    return ( udi_cdc_get_free_tx_buffer() >= size );
}
/* ====================================================================================== */
bool usb_cdc_open( void )

{
    return _deviceOpen;
}
/* ====================================================================================== */
