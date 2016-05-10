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
#define FALSE                     0
#define TRUE                      1
#define SH_MSG_SIZE             100     // Shared message size for IPC

#define SYS_INT         0
#define SYS_CHAR        1
#define BOB_STRING      2

typedef unsigned char Byte_t;
typedef size_t Bobuint_t;

typedef unsigned char bool;
typedef char Types_t;

#endif