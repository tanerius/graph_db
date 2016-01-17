/*graphutils.h*/
#ifndef _GRAPHUTILS_H_
#define _GRAPHUTILS_H_

#ifndef _PRIMITIVES_H_
#include "primitives.h"
#endif

#include <cstring>          /* for strlen, strcpy and memset */
#include <iostream>
#include <pthread.h>
#include <vector>

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

    Plus since everyone is implementing their own strings...here it goes!

    Some stuff that i have problems remembering :

    char       *      mutable_pointer_to_mutable_char;
    char const *      mutable_pointer_to_constant_char;
    char       *const constant_pointer_to_mutable_char;
    char const *const constant_pointer_to_constant_char;
*/
class GdbString {
    private:
        char *m_string = NULL;              /* This is the actual string */
        Gdb_N_t m_length = 0;               /* The strings length including the terminator */
        
    public: // Methods
        bool reallocate(Gdb_N_t nSize);
        char* get() {return m_string;}
        Gdb_N_t length() { return m_string?strlen(m_string):0; } 

        // Construction and destruction
        GdbString() {m_string = NULL, m_length = 0;}; /* Defult constructor */
        GdbString(const char *_string){m_string = NULL; m_length = 0; operator=(_string); };
        ~GdbString(){ if (m_string) free(m_string); m_length = 0;};

        // Operators for assignment
        GdbString& operator=(const GdbString& rhs) { return operator=(rhs.m_string); }; /* Assigning a GdbString */
        GdbString& operator=(const char*); /* Assigning a const char* */

        // Operators for concatenating
        GdbString& operator+=(const char*);
        GdbString& operator+=(GdbString& rhs) { return operator+=(rhs.m_string); }
        GdbString operator+(const char*);
        GdbString operator+(GdbString& rhs) { return operator+(rhs.get()); }

        // Operators for comparison
        bool operator<(GdbString _string) { return strcmp(operator const char*(), (const char*)_string) < 0; }
        bool operator<=(GdbString _string) { int res = strcmp(operator const char*(), (const char*)_string); return res < 0 || res == 0; }

        bool operator>(GdbString _string) { return strcmp(operator const char*(), (const char*)_string) > 0; }
        bool operator>=(GdbString _string) { int res = strcmp(operator const char*(), (const char*)_string); return res > 0 || res == 0; }

        bool operator==(GdbString _string) { return strcmp(operator const char*(), (const char*)_string) == 0; }
        bool operator!=(GdbString _string) { return !operator==(_string); }

        // Cast operator
        operator const char*() { return m_string ? m_string: ""; }

        void clear();

};

#endif