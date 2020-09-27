#ifndef _PHYW_DRIVER_H_
#define _PHYW_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

/* ====================================================================================== */

void phyw_driver_no_pwr(void);
void phyw_driver_5V_pwr(void);
void phyw_driver_host_pwr(void);
void phyw_driver_switch_usb_pwr(void);
uint8_t phyw_driver_get_pwr_st_from_io(void);
void phyw_driver_setup_pins(void);

/* ====================================================================================== */

#endif
