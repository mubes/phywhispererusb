
#ifndef _SYSTICK_HANDLER_
#define _SYSTICK_HANDLER_

#define TICKS_PER_SEC 250

#define MILLIS_TO_TICKS(x) (((x)*TICKS_PER_SEC)/1000)

#define TICKS_TO_MILLIS(x) (((x)*1000)/TICKS_PER_SEC)

#define SECS(x)  ((x)/TICKS_PER_SEC)
#define MSECS(x) (((x)%TICKS_PER_SEC)*(1000/TICKS_PER_SEC))

/* ====================================================================================== */

uint32_t systick_value(void);
void systick_init(void);

/* ====================================================================================== */

#endif
