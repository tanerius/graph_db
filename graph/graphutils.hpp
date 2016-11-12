/*graphutils.h*/
#ifndef _GRAPHUTILS_H_
#define _GRAPHUTILS_H_

#ifndef _CONFIG_H_
#include "config.hpp"
#endif

#ifndef _PRIMITIVES_H_
#include "primitives.hpp"
#endif

#ifndef _GDBSTRING_H_
#include "gdbstring.hpp"
#endif


#include <sys/mman.h> // for mmap and mlock

#include <cstring>          /* for strlen, strcpy and memset */
#include <cstdlib>          /* for itoa */
#include <iostream>
#include <pthread.h>
#include <assert.h>
#include <vector>
#include <ctime>
#include <climits>
#include <csignal>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h> // for htonl()

#include <numeric>
#include <chrono>


/* Helper macros */
#define minElement(a,b)            ((a)<(b)?(a):(b))
#define maxElement(a,b)            ((a)>(b)?(a):(b))
#define safeDelete(_x)      { if (_x) { delete (_x); (_x) = nullptr; } }
#define safeDeleteArray(_x) { if (_x) { delete [] (_x); (_x) = nullptr; } }
#define safeRelease(_x)     { if (_x) { (_x)->Release(); (_x) = nullptr; } }



/*
    A simple function to check endianness
*/
inline bool isBigEndian(){
    return htonl(47) == 47;
}



/*
    A general class from which almost all Gdb all classes must inherit to use advanced functionalities. 
*/
class GdbObject /* wish it wont sound like Java */
{
    public:
        virtual const char* objDisplay() = 0;
        virtual const char* objGetID() = 0;
        // That's it (for now)
    public:
        bool gdb_thread_safe;

};








#endif