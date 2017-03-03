/*graphutils.h*/
#ifndef _GRAPHUTILS_H_
#define _GRAPHUTILS_H_

#ifndef _PRIMITIVES_H_
#include "primitives.hpp"
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

#include <numeric>
#include <chrono>

#include <cstdarg> // for declaring variadic function

class GdbString;
class GdbVariant;

namespace GDBUtils
{

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
        A simple function to return a string timestamp
    */
    inline const char* getTimestamp()
    {
        time_t now = time (NULL);
        char* date = ctime(&now);
        return date;
    }



    /*
        A general class from which almost all Gdb all classes must inherit to use advanced functionalities. 

        Things to consider to add here:
        - version number
    */
    class GdbObject /* wish it wont sound like Java */
    {
        public:
            // pure virtual - these must be overriden
            virtual const char*             ObjDisplay() = 0;
            virtual const char*             ObjGetID() = 0;

            // These are optional
            virtual GdbVariant*             GetTimer();
            virtual void                    TimerStart();
            virtual void                    TimerLap();
            
                                            GdbObject() {};
            virtual                         ~GdbObject() = default;

        private:
            // Make some stuff for being able to measure time
            std::chrono::time_point<std::chrono::high_resolution_clock> event_start;
            std::chrono::time_point<std::chrono::high_resolution_clock> event_end;
            std::chrono::duration<double> event_diff;
    };


    /*
        GdbBinFile thread safe implementation of a binary file manipulation class. 
        Can be used generically for binary files.
    */
    class GdbBinFile : public GdbObject
    {    
        private:
            // Properties
            FILE*                           m_pFile = nullptr;
            GdbString*                      m_fileName = nullptr;
            std::mutex                      FileMutex;
            // Member functions
            bool                            CheckOpen();
     
        public: // properties
            virtual const char*             ObjDisplay() override;
            virtual const char*             ObjGetID() override;
    };
}


#endif


