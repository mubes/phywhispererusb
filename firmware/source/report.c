#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

#include "sam3u.h"
#include "report.h"

#define MAX_STRLEN 64
static char _scratchpad[MAX_STRLEN];

/* ========================================================================================================================= */
/* ========================================================================================================================= */
/* ========================================================================================================================= */
/* Internal routines                                                                                                         */
/* ========================================================================================================================= */
/* ========================================================================================================================= */
/* ========================================================================================================================= */
static __INLINE uint32_t _SendChar ( uint32_t c, uint32_t ch )
{
    if ( ( CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk ) && /* Trace enabled */
            ( ITM->TCR & ITM_TCR_ITMENA_Msk ) && /* ITM enabled */
            ( ITM->TER & ( 1ul << c ) ) /* ITM Port c enabled */
       )
    {
        while ( ITM->PORT[c].u32 == 0 ); // Port available?

        ITM->PORT[c].u8 = ( uint8_t ) ch; // Write data
    }

    return ( ch );
}

/* ========================================================================================================================= */
/* ========================================================================================================================= */
/* ========================================================================================================================= */
/* Externally available routines                                                                                             */
/* ========================================================================================================================= */
/* ========================================================================================================================= */
/* ========================================================================================================================= */
void report( const char *fmt, ... )

/* Debug reporting stream */

{
    char *s = _scratchpad;

    va_list va;
    va_start( va, fmt );
    vsnprintf( _scratchpad, MAX_STRLEN, fmt, va );
    va_end( va );

    while ( *s )
    {
        _SendChar( 0, *s++ );
    }
}

/* ========================================================================================================================= */