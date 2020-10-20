#include <asf.h>

#include "phyw_driver.h"
#include "fpga_program.h"
#include "usb_xmem.h"
#include "systick.h"

/* Storage for power source and if it's switched on or not */
static enum phyw_driver_pwr _powerSource;
static bool _powerOn;


#define BUTTON_IN PIO_PA24_IDX
#define F_VB5V PIO_PA26_IDX
#define F_VBHOST PIO_PA25_IDX

/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
/* Publically accessible routines                                                         */
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
enum phyw_driver_pwr phyw_driver_get_pwr_source( void )

{
    return _powerSource;
}
/* ====================================================================================== */
bool phyw_driver_get_pwr_on( void )

{
    return _powerOn;
}
/* ====================================================================================== */
void phyw_driver_set_pwr_source( enum phyw_driver_pwr e )

{
    if ( e < PWR_MAX_OPTION )
    {
        if ( _powerOn )
        {
            phyw_driver_set_pwr_on( false );
            _powerSource = e;
            phyw_driver_set_pwr_on( true );
        }
        else
        {
            _powerSource = e;
        }
    }
}
/* ====================================================================================== */
void phyw_driver_set_pwr_on( bool isOn )

{
    enum phyw_driver_pwr e = isOn ? _powerSource : PWR_OFF;

    switch ( e )
    {
        case PWR_OFF: /* No power supplied ------------------------------------------ */
            PIOA->PIO_CODR = ( 1 << F_VBHOST ); /* disable sniff power */
            PIOA->PIO_CODR = ( 1 << F_VB5V );   /* disable host power */
            _powerOn = false;

            break;

        case PWR_5V: /* Power from 5v rail of PHYW ---------------------------------- */
            PIOA->PIO_CODR = ( 1 << F_VBHOST ); /* disable sniff power */
            PIOA->PIO_SODR = ( 1 << F_VB5V ); /* enable host power */
            _powerOn = true;

            break;

        case PWR_HOST: /* Power from host ------------------------------------------- */
            PIOA->PIO_CODR = ( 1 << F_VB5V ); /* disable host power */
            PIOA->PIO_SODR = ( 1 << F_VBHOST ); /* enable sniff power */
            _powerOn = true;
            break;

        default: /* ----------------------------------------------------------------- */
            break;
    }
}
/* ====================================================================================== */
bool phyw_driver_pwr_toggle( void )

{
    phyw_driver_set_pwr_on( !_powerOn );
    return ( _powerOn );
}
/* ====================================================================================== */
bool phyw_driver_button_down( void )

{
    return ( !( PIOA->PIO_PDSR & ( 1 << BUTTON_IN ) ) );
}
/* ====================================================================================== */
bool phyw_driver_button_pressed( void )

{
#define BUTTON_HOLD_MIN_TIME_MS (50)

    static enum buttonState
    {
        BUTTON_UP,
        BUTTON_DOWN_WAIT,
        BUTTON_UP_WAIT
    } b;                             /* Current state of the button press machine */
    static uint32_t _buttonDownTime; /* When the button was depressed */

    switch ( b )
    {
        case BUTTON_UP: /* We are waiting for the button to go down -------------------- */
            if ( phyw_driver_button_down() )
            {
                _buttonDownTime = systick_value() + MILLIS_TO_TICKS( BUTTON_HOLD_MIN_TIME_MS );
                b = BUTTON_DOWN_WAIT;
            }

            break;

        case BUTTON_DOWN_WAIT: /* We are waiting to get a stable reading --------------- */
            if ( systick_value() > _buttonDownTime )
            {
                if ( !phyw_driver_button_down() )
                {
                    /* Wasn't held for minimum time, forget it */
                    b = BUTTON_UP;
                }
                else
                {
                    /* Was held, we'll return the press as soon as its released */
                    b = BUTTON_UP_WAIT;
                }
            }

            break;

        case BUTTON_UP_WAIT: /* We are waiting for the button to be released ---------- */
            if ( !phyw_driver_button_down() )
            {
                b = BUTTON_UP;
                return true;
            }

            break;

        default: /* ------------------------------------------------------------------- */
            break;
    }

    return false;
}
/* ====================================================================================== */
void phyw_driver_setup_pins( void )
{
    board_init();
    ioport_set_pin_mode( BUTTON_IN, IOPORT_MODE_PULLUP ); //(1 << 2) = PULL UP
    PIOA->PIO_ODR = ( 1 << BUTTON_IN );
    PIOA->PIO_PUER = ( 1 << BUTTON_IN ); // enable pullup
    PIOA->PIO_DIFSR = ( 1 << BUTTON_IN ); //enable debounce

    PIOA->PIO_OER = ( 1 << F_VB5V ) | ( 1 << F_VBHOST ); //enable output mode on VBHOST/VBSNIFF pins

    phyw_driver_set_pwr_source( PWR_HOST );
    phyw_driver_set_pwr_on( false );

    //Configure FPGA to allow programming via USB
    fpga_program_init();

    /* Enable SMC */
    pmc_enable_periph_clk( ID_SMC );
    gpio_configure_pin( PIN_EBI_DATA_BUS_D0, PIN_EBI_DATA_BUS_FLAG1 );
    gpio_configure_pin( PIN_EBI_DATA_BUS_D1, PIN_EBI_DATA_BUS_FLAG1 );
    gpio_configure_pin( PIN_EBI_DATA_BUS_D2, PIN_EBI_DATA_BUS_FLAG1 );
    gpio_configure_pin( PIN_EBI_DATA_BUS_D3, PIN_EBI_DATA_BUS_FLAG1 );
    gpio_configure_pin( PIN_EBI_DATA_BUS_D4, PIN_EBI_DATA_BUS_FLAG1 );
    gpio_configure_pin( PIN_EBI_DATA_BUS_D5, PIN_EBI_DATA_BUS_FLAG1 );
    gpio_configure_pin( PIN_EBI_DATA_BUS_D6, PIN_EBI_DATA_BUS_FLAG1 );
    gpio_configure_pin( PIN_EBI_DATA_BUS_D7, PIN_EBI_DATA_BUS_FLAG1 );
    gpio_configure_pin( PIN_EBI_NRD, PIN_EBI_NRD_FLAGS );
    gpio_configure_pin( PIN_EBI_NWE, PIN_EBI_NWE_FLAGS );
    gpio_configure_pin( PIN_EBI_NCS0, PIN_EBI_NCS0_FLAGS );

    gpio_configure_group( FPGA_ADDR_PORT, FPGA_ADDR_PINS, ( PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT ) );
    pio_enable_output_write( FPGA_ADDR_PORT, FPGA_ADDR_PINS );

    gpio_configure_pin( PIN_EBI_USB_SPARE0, PIN_EBI_USB_SPARE0_FLAGS );
    gpio_configure_pin( PIN_EBI_USB_SPARE1, PIN_EBI_USB_SPARE1_FLAGS );

    smc_set_setup_timing( SMC, 0, SMC_SETUP_NWE_SETUP( 0 )
                          | SMC_SETUP_NCS_WR_SETUP( 1 )
                          | SMC_SETUP_NRD_SETUP( 1 )
                          | SMC_SETUP_NCS_RD_SETUP( 1 ) );
    smc_set_pulse_timing( SMC, 0, SMC_PULSE_NWE_PULSE( 1 )
                          | SMC_PULSE_NCS_WR_PULSE( 1 )
                          | SMC_PULSE_NRD_PULSE( 3 )
                          | SMC_PULSE_NCS_RD_PULSE( 1 ) );
    smc_set_cycle_timing( SMC, 0, SMC_CYCLE_NWE_CYCLE( 2 )
                          | SMC_CYCLE_NRD_CYCLE( 4 ) );
    smc_set_mode( SMC, 0, SMC_MODE_READ_MODE | SMC_MODE_WRITE_MODE
                  | SMC_MODE_DBW_BIT_8 );
}
/* ====================================================================================== */
