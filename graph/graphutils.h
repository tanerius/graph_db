/*graphutils.h*/
#ifndef _GRAPH_H_
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "graph.h"
#endif

#ifndef _GRAPHUTILS_H_
#define _GRAPHUTILS_H_
#include <iostream>
#include <thread>
#include <thread>
#include <vector>
 /* Utility stuff which are needed especially for threadding */

// Our very own counter
struct Counter {
    big_number value;
    Counter() : value(0) {} // ctor
    void increment() { ++value; }

    void decrement(){
        // dtor
        if(value > 0){ --value; }
    }
};

/* Our very own counter that is thread safe */
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