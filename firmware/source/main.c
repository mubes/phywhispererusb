#include <asf.h>
#include "genclk.h"
#include "fpga_program.h"
#include "phyw_driver.h"
#include "systick.h"
#include "DAP_config.h"
#include "DAP.h"
#include "led_states.h"

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
    bool fpga_active = false;
    uint32_t serial_number[4];
    uint8_t rb[4];

    uint32_t rc = 0, st = 0;


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

    ERR( "Firmware starting =========================================" EOL );

    /* Initialize the DAP */
    DAP_Setup();

    udc_start();

    phyw_driver_set_pwr_on( false );
    FPGA_reset();

    /* Main application loop */
    while ( 1 )
    {
        if ( ( !fpga_active ) && ( check_fpga() ) )
        {
            /* fpga came to life, so switch on trace monitoring */
            GRAB_FPGA();
            //unsafe_writeuint8( FPGA_REG_ARM, 3 );
            unsafe_writeuint8(0xa1,7);
            unsafe_writeuint8(0xa0,1);
            while (unsafe_readuint8(0x10)!=2)
            ERR("*");
            RELEASE_FPGA();
            fpga_active = true;
            DBG( "FPGA Awake" EOL );
        }

        sleepmgr_enter_sleep();

        if ( phyw_driver_button_pressed() )
        {
            phyw_driver_pwr_toggle();
        }

        if ( fpga_active )
        {
            while ( 1 )
            {
                GRAB_FPGA();
                unsafe_readbytes( FPGA_REG_SNIFF_FIFO_RD, rb, 4 );
                RELEASE_FPGA();

                if (rb[0] | rb[1] | rb[2]) ERR("*****************************************");
                ERR("%02x %02x %02x %02x" EOL,rb[0],rb[1],rb[2],rb[3]);

                if ( ( rb[FPGA_FIFO_BYTE_STAT] & FPGA_FE_FIFO_CMD_MASK ) != FPGA_FE_FIFO_CMD_STAT )
                {
                    break;
                }

                rc++;
                /* This is a valid data byte */
#ifndef SWV_SEPARATE
                usb_cmsisdap_swo_send( rb[FPGA_FIFO_BYTE_DATA] );
#else
                usb_swv_swo_send( rb[FPGA_FIFO_BYTE_DATA] );
#endif
            }

            /* Patch code to tell us how many bytes are being sent per second */
            if ( ( systick_value() - st ) >= MILLIS_TO_TICKS( 1000 ) )
            {
                ERR( "%d" EOL, rc );
                rc = 0;
                st = systick_value();
            }

            /* Check we don't need to re-arm */
            if ( ( rb[FPGA_FIFO_BYTE_STAT] & FE_FIFO_STAT_OVERFLOW_BLOCKED ) )
            {
                ERR( "Overflow Blocked " EOL );
                fpga_active = false;
            }
        }

        led_states_update();
    }
}
/* ====================================================================================== */
