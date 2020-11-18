#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "asf.h"
#include "generics.h"

#define MAX_STRLEN 64
static char _scratchpad[MAX_STRLEN];

/* ======================================================================================= */
/* ======================================================================================= */
/* ======================================================================================= */
/* Internal routines                                                                       */
/* ======================================================================================= */
/* ======================================================================================= */
/* ======================================================================================= */
static __INLINE uint32_t _SendCharn ( uint8_t c, uint32_t n, uint32_t *ch )

{
    if ( ( CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk ) && /* Trace enabled */
            ( ITM->TCR & ITM_TCR_ITMENA_Msk ) && /* ITM enabled */
            ( ITM->TER & ( 1ul << c ) ) /* ITM Port c enabled */
       )
    {
        while ( ITM->PORT[c].u32 == 0 ); // Port available?

        switch ( n )
        {
            case 0:
                assert( false );
                n = 0;
                break;

            case 1:
                ITM->PORT[c].u8 = *ch;
                n = 1;
                break;

            case 2:
            case 3:
                ITM->PORT[c].u16 = *ch;
                n = 2;
                break;

            default:
            case 4:
                ITM->PORT[c].u32 = *ch;
                n = 4;
                break;
        }
    }

    return ( n );
}
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
/* Publically accessible routines                                                         */
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
void genericsITMSendInt ( uint8_t c, uint32_t n )
{
    if ( ( CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk ) && /* Trace enabled */
            ( ITM->TCR & ITM_TCR_ITMENA_Msk ) && /* ITM enabled */
            ( ITM->TER & ( 1ul << c ) ) /* ITM Port c enabled */
       )
    {
        while ( ITM->PORT[c].u32 == 0 ); // Port available?

        ITM->PORT[c].u32 = n;
    }
}
/* ====================================================================================== */
void genericsITMSendSint ( uint8_t c, uint16_t n )
{
    if ( ( CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk ) && /* Trace enabled */
            ( ITM->TCR & ITM_TCR_ITMENA_Msk ) && /* ITM enabled */
            ( ITM->TER & ( 1ul << c ) ) /* ITM Port c enabled */
       )
    {
        while ( ITM->PORT[c].u32 == 0 ); // Port available?

        ITM->PORT[c].u16 = n;
    }
}
/* ====================================================================================== */
void genericsITMSendChar ( uint8_t c, char ch )

{
    _SendCharn( c, 1, ( uint32_t * )&ch );
}
/* ====================================================================================== */
void genericsReport( const char *fmt, ... )

/* Debug reporting stream */

{
    char *s = _scratchpad;

    va_list va;
    va_start( va, fmt );
    uint32_t l = vsnprintf( _scratchpad, MAX_STRLEN, fmt, va );
    va_end( va );

    while ( l )
    {
        uint8_t d = _SendCharn( 0, l, ( uint32_t * )s );
        l -= d;
        s += d;
    }
}
/* ====================================================================================== */
void genericsAssertDA( const char *file, uint32_t line, const char *function, const char *test )

/* This is a locking Assert loop (print error message and lockup tighter than a Ducks A...) */
{

    uint32_t spin = 1; /* This is provided so you can set it to zero in a debugger and get */
    /* back out to the caller without having to jump stackframes */

    while ( spin )
    {
        ERR( "%s:%d (%s) %s" EOL, file, line, function, test );
    }
}
/* ====================================================================================== */
