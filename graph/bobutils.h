/* C version of graphutils.h*/
#ifndef _BOBUTILS_H_
#define _BOBUTILS_H_

#ifndef _BOBPRIMITIVES_H_
#include "bobprimitives.h"
#endif

#include <sys/mman.h>               // for mmap and mlock
#include <errno.h>                  // needed for errno
#include <stdlib.h>                 // malloc, calloc, atoi etc.
#include <stdio.h>                  // FILE
#include <string.h>                 // for strlen, strcpy and memset
#include <pthread.h>                // In order to use thread magic
#include <assert.h>                 // for assert()
#include <limits.h>                 // for assert()
#include <arpa/inet.h>              // for htonl()
#include <time.h>                   // for assert()
#include <unistd.h>                 // fork()

/* Helper macros */
#define MinElement(a,b)            ((a)<(b)?(a):(b))
#define MaxElement(a,b)            ((a)>(b)?(a):(b))
#define SafeFree(_x)        { if (_x) { free (_x); (_x) = NULL; } }

inline bool is_big_endian() { return htonl(47) == 47; }
inline bool is_64bit() { return sizeof(int *) == 8; }

/*
    Custom string definition together with initialzer
*/
typedef struct Bobstring_s {
    char *str;
    Bobuint_t len; 
} Bobstring_t;

int bobstring_concat(Bobstring_t*, const char*);
int bobstring_init(Bobstring_t*, const char*);

// Popular hashing functions for strings
unsigned int hash_djb2(const char*);
unsigned long hash_djb2_l(const char*);
unsigned long hash_sdbm(const char*);

#endif