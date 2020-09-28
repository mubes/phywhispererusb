#ifndef _REPORT_H_
#define _REPORT_H_

#ifndef EOL
    #define EOL "\r\n"
#endif

/* ========================================================================================================================= */
void report( const char *fmt, ... );

#ifdef DEBUG
    #define DBG(format, ...) report(format, ## __VA_ARGS__)
#else
    #define DBG(format, ...)
#endif

/* ========================================================================================================================= */

#endif