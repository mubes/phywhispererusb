#include <asf.h>
#include "genclk.h"
#include "fpga_program.h"
#include "phyw_driver.h"
#include "systick.h"
#include "DAP_config.h"
#include "DAP.h"
#include "led_states.h"

#define REPORT_LEVEL 4
#include "generics.h"

char g_usb_serial_number[USB_DEVICE_GET_SERIAL_NAME_LENGTH + 1]; /* Serial Number - will be read by device ID */

/* ====================================================================================== */
static void _delay_ms( uint32_t ms )

{
    uint32_t endT = systick_value() + MILLIS_TO_TICKS( ms );

    while ( systick_value() < endT )
        ;
}
/* ====================================================================================== */
static inline void _genclk_enable_config( unsigned int id, enum genclk_source src, unsigned int divider )
{
    struct genclk_config gcfg;

    genclk_config_defaults( &gcfg, id );
    genclk_enable_source( src );
    genclk_config_set_source( &gcfg, src );
    genclk_config_set_divider( &gcfg, divider );
    genclk_enable( &gcfg, id );
}
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
/* Externally available routines                                                          */
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
int main( void )
{
    uint32_t serial_number[4];

    // Read Device-ID from SAM3U. Do this before enabling interrupts etc.
    flash_read_unique_id( serial_number, sizeof( serial_number ) );

    irq_initialize_vectors();
    cpu_irq_enable();

    // Initialize the sleep manager
    sleepmgr_init();
    sleepmgr_lock_mode( SLEEPMGR_WAIT_FAST );

    sysclk_init();
    systick_init();
    phyw_driver_setup_pins();
    led_states_init();

    // Convert serial number to ASCII for USB Serial number
    snprintf( g_usb_serial_number, USB_DEVICE_GET_SERIAL_NAME_LENGTH + 1, "%08lx%08lx%08lx%08lx",
              serial_number[0], serial_number[1], serial_number[2], serial_number[3] );

    _genclk_enable_config( GENCLK_PCK_1, GENCLK_PCK_SRC_MCK, GENCLK_PCK_PRES_1 );

    /* Stop the USB unterface (needed for debug restarts without power cycle) */
    udc_stop();
    _delay_ms( 200 );

    DBG( "Firmware starting =========================================" EOL );

    /* Initialize the DAP */
    DAP_Setup();

    udc_start();

    phyw_driver_set_pwr_on( false );
    check_fpga();

    /* Main application loop */
    while ( 1 )
    {
        sleepmgr_enter_sleep();

        if ( phyw_driver_button_pressed() )
        {
            phyw_driver_pwr_toggle();
        }
        led_states_update();
    }
}
/* ====================================================================================== */
