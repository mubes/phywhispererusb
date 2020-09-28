#ifndef _PHYW_DRIVER_H_
#define _PHYW_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

enum phyw_driver_pwr { PWR_OFF, PWR_5V, PWR_HOST, PWR_MAX_OPTION, PWR_ALL_ON };

/* ====================================================================================== */

bool phyw_driver_get_pwr_on( void );
void phyw_driver_set_pwr_on( bool isOn );
bool phyw_driver_pwr_toggle( void );

enum phyw_driver_pwr phyw_driver_get_pwr_source( void );
void phyw_driver_set_pwr_source( enum phyw_driver_pwr e );

bool phyw_driver_button_down( void );
bool phyw_driver_button_pressed( void );

void phyw_driver_setup_pins( void );

/* ====================================================================================== */

#endif
