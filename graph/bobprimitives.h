#ifndef _BOBPRIMITIVES_H_
#define _BOBPRIMITIVES_H_

#include <stddef.c> // for some really big numbers and

/**
 * Universal stuff used almost everywhere.
 * All explicit graphdb variables will be prefixed with Gdb_
 */
#define TRUE        1                   /* Custom true */
#define FALSE       0                   /* Custom false */
#define MAX_PAGE_SIZE            20     /* MAX_PAGE_SIZE defines how much should the adjacency list size grow/page */
#define VERSION_MAJOR             0     /* Major version */
#define VERSION_MINOR             1     /* Minor version */
#define VERSION_REV              13     /* Revision version */
#define THEORETICAL_MAX     1000000     /* Maximums used in generating IDs for nodes */
#define GDB_ENGINE_MEMORY         0     /* Memory engine type */