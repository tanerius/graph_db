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

#include <numeric>
#include <chrono>

#include <cstdarg> // for declaring variadic function

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
    A general class from which almost all Gdb all classes must inherit to use advanced functionalities. 

    Things to consider to add here:
    - version number
*/
class GdbObject /* wish it wont sound like Java */
{
    public:
        virtual const char* objDisplay() = 0;
        virtual const char* objGetID() = 0;

        inline void timer_start() 
        {
            event_start = std::chrono::high_resolution_clock::now();
        }

        inline void timer_stop()
        {
            event_end = std::chrono::high_resolution_clock::now();
            event_diff = event_end - event_start;
        }

        inline GdbVariant get_timer() 
        {
            return event_diff.count(); 
        }

        inline const char* getTimestamp()
        {
            time_t now = time (NULL);
            char* date = ctime(&now);
            return date;
        }
        
        // Throw in basic logging
        static bool restart_log()
        {
            FILE* file = fopen(GDB_LOCAL_LOG_FILE, "w");
            if (!file) 
            {
                fprintf(
                    stderr,
                    "ERROR: Could not open GDB_LOCAL_LOG_FILE log file %s for writing\n",
                    GDB_LOCAL_LOG_FILE
                );
                return false;
            }
            time_t now = time (NULL);
            char* date = ctime(&now);
            fprintf(file, "GDB_LOCAL_LOG_FILE log. Local time %s \n", date);
            fclose(file);
            return true;
        }
        //bool gl_log(const char* message, ...);
        /* add a message to the log file. arguments work the same way as printf() */
        static bool log (const char* message, ...) 
        {
            va_list argptr;
            FILE* file = fopen (GDB_LOCAL_LOG_FILE, "a");
            if (!file) {
                fprintf (
                    stderr,
                    "ERROR: could not open GDB_LOCAL_LOG_FILE %s file for appending\n",
                    GDB_LOCAL_LOG_FILE
                );
                return false;
            }
            va_start (argptr, message);
            vfprintf (file, message, argptr);
            va_end (argptr);
            fclose (file);
            return true;
        }

        static bool gl_log_err (const char* message, ...) 
        {
            va_list argptr;
            FILE* file = fopen (GDB_LOCAL_LOG_FILE, "a");
            if (!file) {
                fprintf (
                    stderr,
                    "ERROR: could not open GDB_LOCAL_LOG_FILE %s file for appending\n",
                    GDB_LOCAL_LOG_FILE
                );
                return false;
            }
            va_start (argptr, message);
            vfprintf (file, message, argptr);
            va_end (argptr);
            va_start (argptr, message);
            vfprintf (stderr, message, argptr);
            va_end (argptr);
            fclose (file);
            return true;
        }
    private:
        // Make some stuff for being able to measure time
        std::chrono::time_point<std::chrono::high_resolution_clock> event_start;
        std::chrono::time_point<std::chrono::high_resolution_clock> event_end;
        std::chrono::duration<double> event_diff;
};



class GdbTester : public GdbObject 
{
    public:
        const char* objDisplay() 
        {
            return "\nobjDisplay() method called...\n";
        }

        const char* objGetID()
        {
            return "\nobjGetID() method called...\n";
        }
};



}


#endif