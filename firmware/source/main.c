#include <asf.h>
#include "conf_usb.h"
#include "stdio_serial.h"
#include "ui.h"
#include "genclk.h"
#include "tasks.h"
#include "usb_xmem.h"
#include "fpga_program.h"
#include "usb.h"
#include "sysclk.h"
#include <string.h>
#include "report.h"

//Serial Number - will be read by device ID
char usb_serial_number[33] = "000000000000DEADBEEF";


bool main_extra_string(void);
void phywhisperer_no_pwr(void);
void phywhisperer_5V_pwr(void);
void phywhisperer_host_pwr(void);
void phywhisperer_switch_usb_pwr(void);

void phywhisperer_no_pwr(void)
{
    PIOA->PIO_CODR = (1 << F_VBHOST); //disable sniff power
    PIOA->PIO_CODR = (1 << F_VB5V); //disable host power
}

void phywhisperer_5V_pwr(void)
{
    PIOA->PIO_CODR = (1 << F_VBHOST); //disable sniff power
    PIOA->PIO_SODR = (1 << F_VB5V); //enable host power
}

void phywhisperer_host_pwr(void)
{
    PIOA->PIO_CODR = (1 << F_VB5V); //disable host power
    PIOA->PIO_SODR = (1 << F_VBHOST); //enable sniff power
}

void phywhisperer_switch_usb_pwr(void)
{
    if ((PIOA->PIO_ODSR & (1 << F_VBHOST))) {
        //Switch to host power mode
        phywhisperer_5V_pwr();
    } else {
        //Switch to sniff power mode
        phywhisperer_host_pwr();
    }
}

bool main_cdc_enable(uint8_t port)
{
    DBG("CDC Enable" EOL);
    return true;
}

void main_cdc_disable(uint8_t port)
{
    DBG("CDC Disable" EOL);
}

void main_cdc_set_dtr(uint8_t port, bool b_enable)
{
    DBG("Set DTR %d" EOL, b_enable);
}

bool main_generic_enable(void)
{
//        main_b_generic_enable = true;
    DBG("Generic enable" EOL);
    return true;
}

void main_generic_disable(void)
{
    DBG("Generic disable" EOL);
//        main_b_generic_enable = false;
}

void main_hid_set_feature(uint8_t* r)
{
DBG("Set feature" EOL);
        if (r[0] == 0xAA && r[1] == 0x55
                        && r[2] == 0xAA && r[3] == 0x55) {
                // Disconnect USB Device
                udc_stop();
                ui_powerdown();
        }
}

bool main_hid_report_out( void *ptr)

{
    DBG("HID Report out" EOL);
 return true;
}

static uint8_t pwr_st_from_io(void)
{
    if (!(PIOA->PIO_ODSR & (1 << F_VBHOST)) && !(PIOA->PIO_ODSR & (1 << F_VB5V))) {
        //USB off
        return 0;
    } else if (!(PIOA->PIO_ODSR & (1 << F_VBHOST)) && (PIOA->PIO_ODSR & (1 << F_VB5V))) {
        //Host power
        return 1;
    } else if ((PIOA->PIO_ODSR & (1 << F_VBHOST)) && !(PIOA->PIO_ODSR & (1 << F_VB5V))) {
        //Sniffer power
        return 2;
    } else {
        //Everything's on...
        return 0xFF;
    }

}

void phywhisperer_setup_pins(void)
{
    board_init();
    ioport_set_pin_mode(BUTTON_IN, IOPORT_MODE_PULLUP); //(1 << 2) = PULL UP
    PIOA->PIO_ODR = (1 << BUTTON_IN);
    PIOA->PIO_PUER = (1 << BUTTON_IN); // enable pullup
    PIOA->PIO_DIFSR = (1 << BUTTON_IN); //enable debounce

    PIOA->PIO_OER = (1 << F_VB5V) | (1 << F_VBHOST); //enable output mode on VBHOST/VBSNIFF pins

    phywhisperer_host_pwr();
    
    //Configure FPGA to allow programming via USB
    fpga_program_init();

    /* Enable SMC */
    pmc_enable_periph_clk(ID_SMC);
    gpio_configure_pin(PIN_EBI_DATA_BUS_D0, PIN_EBI_DATA_BUS_FLAG1);
    gpio_configure_pin(PIN_EBI_DATA_BUS_D1, PIN_EBI_DATA_BUS_FLAG1);
    gpio_configure_pin(PIN_EBI_DATA_BUS_D2, PIN_EBI_DATA_BUS_FLAG1);
    gpio_configure_pin(PIN_EBI_DATA_BUS_D3, PIN_EBI_DATA_BUS_FLAG1);
    gpio_configure_pin(PIN_EBI_DATA_BUS_D4, PIN_EBI_DATA_BUS_FLAG1);
    gpio_configure_pin(PIN_EBI_DATA_BUS_D5, PIN_EBI_DATA_BUS_FLAG1);
    gpio_configure_pin(PIN_EBI_DATA_BUS_D6, PIN_EBI_DATA_BUS_FLAG1);
    gpio_configure_pin(PIN_EBI_DATA_BUS_D7, PIN_EBI_DATA_BUS_FLAG1);
    gpio_configure_pin(PIN_EBI_NRD, PIN_EBI_NRD_FLAGS);
    gpio_configure_pin(PIN_EBI_NWE, PIN_EBI_NWE_FLAGS);
    gpio_configure_pin(PIN_EBI_NCS0, PIN_EBI_NCS0_FLAGS);

    gpio_configure_group(FPGA_ADDR_PORT, FPGA_ADDR_PINS, (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT));
    pio_enable_output_write(FPGA_ADDR_PORT, FPGA_ADDR_PINS);

    /*
    gpio_configure_pin(PIN_EBI_ADDR_BUS_A0, PIN_EBI_ADDR_BUS_FLAG1);
    gpio_configure_pin(PIN_EBI_ADDR_BUS_A1, PIN_EBI_ADDR_BUS_FLAG1);
    gpio_configure_pin(PIN_EBI_ADDR_BUS_A2, PIN_EBI_ADDR_BUS_FLAG1);
    gpio_configure_pin(PIN_EBI_ADDR_BUS_A3, PIN_EBI_ADDR_BUS_FLAG1);
    gpio_configure_pin(PIN_EBI_ADDR_BUS_A4, PIN_EBI_ADDR_BUS_FLAG1);
    gpio_configure_pin(PIN_EBI_ADDR_BUS_A5, PIN_EBI_ADDR_BUS_FLAG1);
    gpio_configure_pin(PIN_EBI_ADDR_BUS_A6, PIN_EBI_ADDR_BUS_FLAG1);
    gpio_configure_pin(PIN_EBI_ADDR_BUS_A7, PIN_EBI_ADDR_BUS_FLAG1);
    */

    gpio_configure_pin(PIN_EBI_USB_SPARE0, PIN_EBI_USB_SPARE0_FLAGS);
    gpio_configure_pin(PIN_EBI_USB_SPARE1, PIN_EBI_USB_SPARE1_FLAGS);

    smc_set_setup_timing(SMC, 0, SMC_SETUP_NWE_SETUP(0)
                         | SMC_SETUP_NCS_WR_SETUP(1)
                         | SMC_SETUP_NRD_SETUP(1)
                         | SMC_SETUP_NCS_RD_SETUP(1));
    smc_set_pulse_timing(SMC, 0, SMC_PULSE_NWE_PULSE(1)
                         | SMC_PULSE_NCS_WR_PULSE(1)
                         | SMC_PULSE_NRD_PULSE(3)
                         | SMC_PULSE_NCS_RD_PULSE(1));
    smc_set_cycle_timing(SMC, 0, SMC_CYCLE_NWE_CYCLE(2)
                         | SMC_CYCLE_NRD_CYCLE(4));
    smc_set_mode(SMC, 0, SMC_MODE_READ_MODE | SMC_MODE_WRITE_MODE
                 | SMC_MODE_DBW_BIT_8);
}

bool main_extra_string(void)
{
        static uint8_t udi_cdc_name[] = "Serial";
        static uint8_t udi_comm_name[] = "CDC-COMM";
        static uint8_t udi_data_name[] = "CDC-DATA";
        static uint8_t udi_vendor_name[] = "Phywhisperer-USB";
        static uint8_t udi_hid_generic[] = "CMSIS-DAP";

        struct extra_strings_desc_t{
                usb_str_desc_t header;
                le16_t string[20];
        };
        static UDC_DESC_STORAGE struct extra_strings_desc_t extra_strings_desc = {
                .header.bDescriptorType = USB_DT_STRING
        };

        uint8_t i;
        uint8_t *str;
        uint8_t str_lgt=0;

        // Link payload pointer to the string corresponding at request
        switch (udd_g_ctrlreq.req.wValue & 0xff) {
        case UDI_CDC_IAD_STRING_ID:
                DBG("Request UDI_CDC_IAD_STRING_ID" EOL);
                str_lgt = sizeof(udi_cdc_name)-1;
                str = udi_cdc_name;
                break;

        case UDI_CDC_COMM_STRING_ID_0:
                DBG("Request UDI_CDC_COMM_STRING_ID_0" EOL);
                str_lgt = sizeof(udi_comm_name)-1;
                str = udi_comm_name;
                break;

        case UDI_CDC_DATA_STRING_ID_0:
                DBG("Request UDI_CDC_DATA_STRING_ID_0" EOL);
                str_lgt = sizeof(udi_data_name)-1;
                str = udi_data_name;
                break;

        case UDI_VENDOR_STRING_ID:
                DBG("Request UDI_VENDOR_STRING_ID" EOL);
                str_lgt = sizeof(udi_vendor_name)-1;
                str = udi_vendor_name;
                break; 

        case UDI_HID_GENERIC_STRING_ID:
                DBG("Request for UDI_HID_GENERIC_STRING_ID" EOL);
                str_lgt = sizeof(udi_hid_generic)-1;
                str = udi_hid_generic;
                break;

        default:
                DBG("Request for unknown element %d" EOL, udd_g_ctrlreq.req.wValue & 0xff);
                return false;
        }

        if (str_lgt!=0) {
                for( i=0; i<str_lgt; i++) {
                        extra_strings_desc.string[i] = cpu_to_le16((le16_t)str[i]);
                }
                extra_strings_desc.header.bLength = 2+ (str_lgt)*2;
                udd_g_ctrlreq.payload_size = extra_strings_desc.header.bLength;
                udd_g_ctrlreq.payload = (uint8_t *) &extra_strings_desc;
        }

        // if the string is larger than request length, then cut it
        if (udd_g_ctrlreq.payload_size > udd_g_ctrlreq.req.wLength) {
                udd_g_ctrlreq.payload_size = udd_g_ctrlreq.req.wLength;
        }
        return true;
}

void hacky_delay(void);

void hacky_delay(void)
{
    for (volatile uint32_t i = 0; i < 250000; i++);
}

static inline void genclk_enable_config(unsigned int id, enum genclk_source src, unsigned int divider)
{
    struct genclk_config gcfg;

    genclk_config_defaults(&gcfg, id);
    genclk_enable_source(src);
    genclk_config_set_source(&gcfg, src);
    genclk_config_set_divider(&gcfg, divider);
    genclk_enable(&gcfg, id);
}

void (*pwr_list[])(void) = {phywhisperer_no_pwr, phywhisperer_5V_pwr, phywhisperer_host_pwr};

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
    phywhisperer_setup_pins();

	//Convert serial number to ASCII for USB Serial number
	for(unsigned int i = 0; i < 4; i++){
		sprintf(usb_serial_number+(i*8), "%08x", (unsigned int)serial_number[i]);	
	}
	usb_serial_number[32] = 0;

    genclk_enable_config(GENCLK_PCK_1, GENCLK_PCK_SRC_MCK, GENCLK_PCK_PRES_1);

    udc_stop();
    hacky_delay();hacky_delay();hacky_delay();hacky_delay();

    udc_start();
    gpio_set_pin_high(LED0_GPIO);
    gpio_set_pin_low(LED1_GPIO);

    phywhisperer_no_pwr();
    USB_PWR_STATE = 0;

    while(1) {
       // sleepmgr_enter_sleep();
        uint8_t button_status = !(PIOA->PIO_PDSR & (1 << BUTTON_IN));
        if (button_status){
            hacky_delay(); //delay to try to debounce
            while (!(PIOA->PIO_PDSR & (1 << BUTTON_IN))); //wait for trigger to be unpressed

            if (USB_PWR_STATE && USB_PWR_STATE <= 2) {
                if (pwr_st_from_io()) { //currently on
                    phywhisperer_no_pwr();
                } else {
                    pwr_list[USB_PWR_STATE]();
                }
            }
            hacky_delay();
        }
    }
}
