/* C version of graphutils.h*/
#ifndef _BOBUTILS_H_
#define _BOBUTILS_H_

#ifndef _CONFIG_H_
#include "config.h"
#endif

#ifndef _PRIMITIVES_H_
#include "primitives.h"
#endif

#include <sys/mman.h>               // for mmap and mlock
#include <string.h>                 // for strlen, strcpy and memset
#include <pthread.h>                // In order to use thread magic
#include <assert.h>                 // for assert()
#include <limits.h>                 // for assert()
