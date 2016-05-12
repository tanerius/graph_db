/*graphutils.h*/
#ifndef _GDBSTRING_H_
#define _GDBSTRING_H_

#ifndef _CONFIG_H_
#include "config.h"
#endif

#ifndef _PRIMITIVES_H_
#include "primitives.h"
#endif

#include <cstring>          /* for strlen, strcpy and memset */
#include <climits>
#include <assert.h>


#define safeDeleteArray(_x) { if (_x) { delete [] (_x); (_x) = nullptr; } }


/*!
    This is the GdbString class. It should be the only reporesentation of text that 
    should be used for this server. It has everything needed.
    These strings are thread safe and ready to travel across sockets.

    Some stuff that i have problems remembering :

    char       *      mutable_pointer_to_mutable_char;
    char const *      mutable_pointer_to_constant_char;
    char       *const constant_pointer_to_mutable_char;
    char const *const constant_pointer_to_constant_char;

    TODO: check problem with memory deallocation. In some cases, it is released more than once!
*/
class GdbString {
    protected:
        char *m_string = NULL;              /* This is the actual string */
        Gdb_N_t m_length = 0;               /* The strings length including the terminator */

    private:
        /* Introduce a 4 byte gap which wont affect strlen. Needed for multibyte chars */
        static const int    BYTE_GAP  = 4;

        //methods
        char* allocateMemory(Gdb_N_t); 

    public: // Methods
        
        bool beginsWith ( const char * _string ) const
        {
            if ( !m_string || !_string )
                return false;
            return strncmp ( m_string, _string, strlen(_string) )==0;
        }

        bool endsWith ( const char * _string ) const
        {
            if ( !m_string || !_string )
                return false;

            int i_val = strlen ( m_string );
            int i_prefix = strlen ( _string );
            if ( i_val<i_prefix )
                return false;
            return strncmp ( m_string+i_val-i_prefix, _string, i_prefix )==0;
        }

        /* Checks to see if string is empty */
        bool isEmpty () const
        {
            if ( !m_string )
                return true;
            return ( (*m_string)=='\0' );
        }

        bool reallocate(Gdb_N_t nSize);

        /* Return a cstr immutable representation of the stored string */
        const char * cstr () const {return m_string; }
        
        Gdb_N_t length() { return m_string?strlen(m_string):0; } 
        Gdb_N_t length_serialized() { return m_string?strlen(m_string)+sizeof(char):0; } 

        // Construction and destruction
        GdbString() {m_string = NULL, m_length = 0;}; /* Defult constructor */
        GdbString(const char *_string){m_string = NULL; m_length = 0; operator=(_string); };
        ~GdbString(){ safeDeleteArray(m_string); m_length = 0;};

        GdbString subString ( int i_start, int i_count ) const
        {
            assert ( i_start>=0 );
            assert ( i_count>0 );
            assert ( (i_start+i_count)>=0  );

            GdbString new_str;
            new_str.m_string = (char*)malloc(m_length*sizeof(char) + BYTE_GAP);
            strncpy ( new_str.m_string, m_string+i_start, i_count );
            memset ( new_str.m_string+i_count, 0, 1+BYTE_GAP );
            return new_str;
        }

        // Operators for assignment
        GdbString& operator=(const GdbString& rhs) { return operator=(rhs.m_string); }; /* Assigning a GdbString */
        GdbString& operator=(const char*); /* Assigning a const char* */

        // Operators for concatenating
        GdbString& operator+=(const char*);
        GdbString& operator+=(GdbString& rhs) { return operator+=(rhs.m_string); }
        
        GdbString operator+(const char*);
        GdbString operator+(GdbString& rhs) { return operator+(rhs.cstr()); }

        // Operators for comparison - Make sure that they output constant objects so we can use them in 
        // maps as keys. Planning to hash here too...i.e. have a hash function 
        bool operator<(const char*_string) const { return strcmp(operator const char*(), _string) < 0;  }
        bool operator<=(const char*_string) const {
            int res = operator<(_string);
            return res < 0 || res == 0;
        }
        bool operator<(const GdbString& _string) const { return operator<(_string.cstr()); }
        bool operator<=(const GdbString& _string) const { 
            return operator<=(_string.cstr());
        }
        
        bool operator>(const char*_string) const { return strcmp(operator const char*(), _string) > 0; }
        bool operator>=(const char*_string) const { 
            int res = strcmp(operator const char*(), _string);
            return res > 0 || res == 0;
        }
        bool operator>(const GdbString& _string) const { return operator>(_string.cstr()); }
        bool operator>=(const GdbString& _string) const { return operator>=(_string.cstr());} 

        inline bool operator==(const char* _string) const {
            if ( !_string || !m_string )
                return ( !_string && !m_string );
            return strcmp ( m_string, _string )==0;
        }
        inline bool operator==(GdbString& _string) const { return operator==(_string.cstr()); }
        inline bool operator!=(const char* _string) const { return !operator==(_string); }
        inline bool operator!=(GdbString& _string) const { return !operator==(_string.cstr()); }

        // Cast operator ... 
        // TODO: check performance of this
        operator const char*() const { return m_string ? m_string: ""; }

        void clear();

};


#endif