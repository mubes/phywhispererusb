#ifndef SOURCE_INC_GENERICS_H_
#define SOURCE_INC_GENERICS_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#define EOL "\r\n"

// --- System Level Glue
// ---------------------

#ifndef REPORT_LEVEL
    #define REPORT_LEVEL 1
#endif

#if (REPORT_LEVEL>3)
    #define DBG(x...) genericsReport(x)
#else
    #define DBG(x...) {}
#endif

#if (REPORT_LEVEL>2)
    #define INFO(x...) genericsReport(x)
#else
    #define INFO(x...) {}
#endif

#if (REPORT_LEVEL>1)
    #define WARN(x...) genericsReport(x)
#else
    #define WARN(x...) {}
#endif

#if (REPORT_LEVEL>0)
    #define ERR(x...) genericsReport(x)
#else
    #define ERR(x...) {}
#endif

#ifdef DEBUG
    #define ASSERT(x) if (!x) genericsAssertDA(__FILE__, __LINE__, __FUNCTION__, #x)
    #define dbgprint(x...) genericsReport(x)
    #define CHECK(a,b) if (a!=b) { genericsReport("Check failed"); ASSERT(false); }
#else
    #define ASSERT(x) {}
    #define dbgprint(x) {}
    #define CHECK(a,b) b
#endif

// String printing macro ... XSTR(VAR) will preprint VAR value
// -----------------------------------------------------------
#define XSTR(x) STR(x)
#define STR(x) #x

// --- Simple event callbacks
// --------------------------
#define EVENT_CB(x) void (x)(uint32_t j)
#define CALLBACK(x,y) if ((x)) (x)((y))
#define EVENT_ISSET(x,y) (x&(1<<y))
#define EVENT_CLEAR(x,y) (x&=~(1<<y))

// --- Incantations for code marking
// ---------------------------------
#define PACKED __attribute__((packed))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))
#define ALWAYS_INLINE inline __attribute__((always_inline))

// ============================================================================================

void genericsAssertDA( const char *file, uint32_t line, const char *function, const char *test );
void genericsReport( const char *fmt, ... );

// ============================================================================================

#endif /* SOURCE_INC_GENERICS_H_ */
