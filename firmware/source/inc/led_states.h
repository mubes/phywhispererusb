#ifndef _LED_STATES_
#define _LED_STATES_

enum leds {STATE_LED, ERROR_LED, NUM_LED};

enum state_led { STATE_LED_OFF, STATE_LED_ON, STATE_LED_CONNECTED, STATE_LED_RUNNING };
enum error_led { ERROR_LED_OFF, ERROR_LED_ON, ERROR_LED_NOFPGA };

/* ====================================================================================== */
void led_states_set( enum leds led, uint8_t condition );
void led_states_update( void );
void led_states_init( void );
/* ====================================================================================== */

#endif
