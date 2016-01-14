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
    Our very own counter that is thread safe
*/
struct ConcurrentSafeCounter {
    std::mutex mutex;
    Counter counter;

    void increment(){
        std::lock_guard<std::mutex> guard(mutex);
        counter.increment();
    }

    void decrement(){
        std::lock_guard<std::mutex> guard(mutex);
        counter.decrement();
    }
};

#endif