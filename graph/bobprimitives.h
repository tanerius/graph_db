#ifndef _BOBPRIMITIVES_H_
#define _BOBPRIMITIVES_H_

#include <stddef.h>

/**
 * Universal stuff used almost everywhere.
 * Only primitive definitions
 */

#define MAX_PAGE_SIZE            20     /* MAX_PAGE_SIZE defines how much should the adjacency list size grow/page */
#define VERSION_MAJOR             0     /* Major version */
#define VERSION_MINOR             1     /* Minor version */
#define VERSION_REV              13     /* Revision version */
#define THEORETICAL_MAX     1000000     /* Maximums used in generating IDs for nodes */
#define GDB_ENGINE_MEMORY         0     /* Memory engine type */



typedef unsigned char Byte_t;
typedef size_t Bobuint_t;

typedef enum {
    FALSE=0, 
    TRUE
} bool;

typedef enum {
    SYS_INT,
    SYS_CHAR,
    BOB_STRING
} Types_t;

#endif