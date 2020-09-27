#include <asf.h>
#include "genclk.h"
#include "fpga_program.h"
#include "phyw_driver.h"
#include "report.h"

//Serial Number - will be read by device ID
char usb_serial_number[33] = "000000000000DEADBEEF";
void (*pwr_list[])(void) = {phyw_driver_no_pwr, phyw_driver_5V_pwr, phyw_driver_host_pwr};
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
static void _hacky_delay(void)
{
    for (volatile uint32_t i = 0; i < 250000; i++);
}
/* ====================================================================================== */

static inline void _genclk_enable_config(unsigned int id, enum genclk_source src, unsigned int divider)
{
    struct genclk_config gcfg;

    genclk_config_defaults(&gcfg, id);
    genclk_enable_source(src);
    genclk_config_set_source(&gcfg, src);
    genclk_config_set_divider(&gcfg, divider);
    genclk_enable(&gcfg, id);
}

/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
/* Externally available routines                                                          */
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */

int main(void)
{
    uint32_t serial_number[4];

    DBG("Firmware starting =========================================" EOL);

    // Read Device-ID from SAM3U. Do this before enabling interrupts etc.
    flash_read_unique_id(serial_number, sizeof(serial_number));

    irq_initialize_vectors();
    cpu_irq_enable();

    // Initialize the sleep manager
    sleepmgr_init();

    sysclk_init();
    phyw_driver_setup_pins();

	//Convert serial number to ASCII for USB Serial number
	for(unsigned int i = 0; i < 4; i++){
		sprintf(usb_serial_number+(i*8), "%08x", (unsigned int)serial_number[i]);	
	}
	usb_serial_number[32] = 0;

    _genclk_enable_config(GENCLK_PCK_1, GENCLK_PCK_SRC_MCK, GENCLK_PCK_PRES_1);

    /* Stop the USB unterface (needed for debug restarts without power cycle) */
    udc_stop();
    _hacky_delay(); _hacky_delay(); _hacky_delay(); _hacky_delay();

    udc_start();
    gpio_set_pin_high(LED0_GPIO);
    gpio_set_pin_low(LED1_GPIO);

    phyw_driver_no_pwr();
    USB_PWR_STATE = 0;

    while(1) {
       // sleepmgr_enter_sleep();
        uint8_t button_status = !(PIOA->PIO_PDSR & (1 << BUTTON_IN));
        if (button_status){
            _hacky_delay(); //delay to try to debounce
            while (!(PIOA->PIO_PDSR & (1 << BUTTON_IN))); //wait for trigger to be unpressed

            if (USB_PWR_STATE && USB_PWR_STATE <= 2) {
                if (phyw_driver_get_pwr_st_from_io()) { //currently on
                    phyw_driver_no_pwr();
                } else {
                    pwr_list[USB_PWR_STATE]();
                }
            }
            _hacky_delay();
        }
    }
}
/* ====================================================================================== */
