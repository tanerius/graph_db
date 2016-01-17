/*graphutils.h*/
#ifndef _PRIMITIVES_H_
#include "primitives.h"
#endif

#ifndef _GRAPHUTILS_H_
#define _GRAPHUTILS_H_
#include <iostream>
#include <pthread.h>
#include <vector>


/**
 * Utility stuff which are needed especially for threadding.
 * 
 */

/*! 
    Our very own counter
*/
struct Counter {
    Gdb_N_t value;

    Counter() : value(0) {} 
    void increment() { ++value; }

    void decrement(){
        if(value > 0){ --value; }
    }
};

/*!
    This is the GdbString class. It should be the only reporesentation of text that 
    should be used for this server. It has everything needed.
    These strings are thread safe and ready to travel across sockets.
*/
class GdbString{
    private: // Members
        char *s;                /* This is the actual string */
        Gdb_N_t s_length;       /* The strings length not including the terminator */
        


    public: // Methods
        GdbString(); /* Defult constructor */
        GdbString(const char*);

        GdbString& operator=(const GdbString &rhs);

        const Gdb_N_t length() { return s_length; }
        const Gdb_N_t length_raw() { return s_length; }
};

#endif