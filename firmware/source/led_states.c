#include <stdint.h>
#include <stdbool.h>

#include "compiler.h"
#include "board.h"
#include "conf_board.h"
#include "gpio.h"
#include "ioport.h"
#include "systick.h"

#define REPORT_LEVEL 4
#include "generics.h"
#include "led_states.h"

#define MAX_STATES   (4)
#define MAX_ELEMENTS (5)

struct ledStat
{
  uint32_t led;

  uint8_t  state;
  uint8_t  step;
  uint32_t nextTicks;
  uint16_t stateTable[MAX_STATES][MAX_ELEMENTS];
};

struct ledStat _s[NUM_LED] =
  {
   {
    /* Status LED */
    .led = LED0_GPIO,
    .stateTable = {
                   { MILLIS_TO_TICKS(1000), 0 }, /* OFF */
                   { 0, MILLIS_TO_TICKS(1000), 0 }, /* ON */          
                   { MILLIS_TO_TICKS(1000), MILLIS_TO_TICKS(100), 0 },
                   { MILLIS_TO_TICKS(100), MILLIS_TO_TICKS(100), 0 }
     }
   },
   {
    /* Error LED */
    .led = LED1_GPIO,
    .stateTable = {
                   { 0, MILLIS_TO_TICKS(1000), 0 }, /* OFF */                   
                   { MILLIS_TO_TICKS(1000), 0 }, /* ON */
                   { MILLIS_TO_TICKS(100), MILLIS_TO_TICKS(100), MILLIS_TO_TICKS(200), MILLIS_TO_TICKS(100), 0 }, 
                   { MILLIS_TO_TICKS(100), MILLIS_TO_TICKS(100), 0 }
     }
   }
};
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
/* Externally available routines                                                          */
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
void led_states_set(enum leds led, uint8_t condition)

{
  ASSERT (led<NUM_LED);
  ASSERT (condition < MAX_STATES);

  _s[led].state = condition;
  _s[led].nextTicks = 0;
}
/* ====================================================================================== */
void led_states_update(void)

{
  for (uint32_t l=0; l<NUM_LED; l++)
    {
      if (systick_value()>=_s[l].nextTicks)
        {
          /* Move to next element of table, if it's zero wrap around */
          ++_s[l].step;
          if ((_s[l].step>=MAX_ELEMENTS) || (!_s[l].stateTable[_s[l].state][_s[l].step]))
            {
              _s[l].step=0;
              /* ...and if this entry is zero move to next one to accomodate always on */
              if (!_s[l].stateTable[_s[l].state][_s[l].step])
                {
                  _s[l].step++;
                }
            }
          ASSERT(_s[l].stateTable[_s[l].state][_s[l].step]);
      _s[l].nextTicks=systick_value()+_s[l].stateTable[_s[l].state][_s[l].step];
      if (!(_s[l].step&1 ))
        {
          gpio_set_pin_high( _s[l].led );
        }
      else
        {
          gpio_set_pin_low( _s[l].led );
        }
    }
    }
}
/* ====================================================================================== */
void led_states_init(void)

{
  /* We assume that LEDs have already been set as GPIO */
  led_states_update();
}
/* ====================================================================================== */
