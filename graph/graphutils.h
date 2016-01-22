/*graphutils.h*/
#ifndef _GRAPHUTILS_H_
#define _GRAPHUTILS_H_

#ifndef _PRIMITIVES_H_
#include "primitives.h"
#endif

#include <cstring>          /* for strlen, strcpy and memset */
#include <cstdlib>          /* for itoa */
#include <iostream>
#include <pthread.h>
#include <assert.h>
#include <vector>


/* Helper macros */
#define minElement(a,b)            ((a)<(b)?(a):(b))
#define maxElement(a,b)            ((a)>(b)?(a):(b))
#define safeFree(_x)        { if (_x) { free (_x); (_x) = NULL; } }
#define safeDelete(_x)      { if (_x) { delete (_x); (_x) = NULL; } }
#define safeDeleteArray(_x) { if (_x) { delete [] (_x); (_x) = NULL; } }
#define safeRelease(_x)     { if (_x) { (_x)->Release(); (_x) = NULL; } }

/* so windows ppl can be more familiar */
typedef unsigned char BYTE;
typedef unsigned int DWORD;

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
        ~GdbString(){ if (m_string) free(m_string); m_length = 0;};

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

        // Operators for comparison
        bool operator<(GdbString& _string) { return strcmp(operator const char*(), (const char*)_string.cstr()) < 0; }
        bool operator<=(GdbString& _string) { int res = strcmp(operator const char*(), (const char*)_string.cstr()); return res < 0 || res == 0; }

        bool operator>(GdbString& _string) { return strcmp(operator const char*(), (const char*)_string.cstr()) > 0; }
        bool operator>=(GdbString& _string) { int res = strcmp(operator const char*(), (const char*)_string.cstr()); return res > 0 || res == 0; }

        inline bool operator==(const char* _string) {
            if ( !_string || !m_string )
                return ( !_string && !m_string );
            return strcmp ( m_string, _string )==0;
        }
        inline bool operator==(GdbString& _string) { return operator==(_string.cstr()); }
        inline bool operator!=(const char* _string) { return !operator==(_string); }
        inline bool operator!=(GdbString& _string) { return !operator==(_string.cstr()); }



        // Cast operator
        operator const char*() { return m_string ? m_string: ""; }

        void clear();

};


/*!
    This is the GdbNumeric class derived from the GdbString class. 
    It represents easily printable immutable numbers
*/
class GdbNumeric : public GdbString
{
    protected:
        int             m_int;
        float           m_float;
    public:
        /* default */
        GdbNumeric () : GdbString ()
            , m_int ( 0 )
            , m_float ( 0.0f )
        {}

        /* ctor for char* as param */
        GdbNumeric ( const char * _string ) : GdbString ( _string )
            , m_int ( m_string ? atoi ( m_string ) : 0 )
            , m_float ( m_string ? (float)atof ( m_string ) : 0.0f )
        {}

        /* ctor for int as param  */
        GdbNumeric ( int _int ) : GdbString ( std::to_string(_int).c_str() )
            , m_int ( _int )
            , m_float ( (float)_int )
        {}

        /* ctor for float as param  */
        GdbNumeric ( float _float ) : GdbString ( std::to_string(_float).c_str() )
            , m_int ( (int)_float )
            , m_float ( _float )
        {}

        /* copy ctor */
        GdbNumeric ( const GdbNumeric & rhs ) : GdbString ()
            , m_int ( 0 )
            , m_float ( 0.0f )
        {
            *this = rhs;
        }

        /* Get the int value */
        int int_val () const {return m_int;}

        /* Get the float value */
        float float_val () const { return m_float; }

        /* Assignment operator */
        const GdbNumeric & operator = ( const GdbNumeric & rhs ) {
            GdbString::operator = ( rhs );
            m_int = rhs.m_int;
            m_float = rhs.m_float;
            return *this;
        }
};



/*!
    This is a representation of vector qhich we will need a lot so it eneds to be minimal and fast. 
    Elements should be immutable (i think!!!) but not exactly sure
*/
template <typename T> 
class GdbVector {
    private:
        T *m_arr_elements; 
        Gdb_N_t m_length;
        Gdb_N_t m_max_length;

        /* Member fn used to grow the size of the vector as needed by Gdb_N_t */
        Gdb_N_t resize_vector(const Gdb_N_t _new_max_length){
            assert(_new_max_length > m_length);
            T *_arr_resized = (T*)malloc(sizeof(T) * _new_max_length);
            if(_arr_resized){
                m_max_length = _new_max_length;
                for(Gdb_N_t i=0;i<m_length;i++){
                    _arr_resized[i]=m_arr_elements[i];
                }
                safeFree(m_arr_elements);
                m_arr_elements = _arr_resized;
                return _new_max_length;
            }
            else{
                return 0;
            }
        }

    public:
        /*
            Implementation of default constructor for vector
        */
        GdbVector(){
            m_arr_elements = NULL;
            m_length=0;
            m_max_length=0;
        }
        /*
            Implementation of copy constructor for vector initialized with 
            const Gdb_N_t
        */
        GdbVector(const Gdb_N_t _max_length){
            assert(_max_length>0);
            m_length=0;
            m_max_length=_max_length;
            m_arr_elements = (T*)malloc(sizeof(T) * m_max_length);
        }
        /*
            Implementation of copy constructor for vector initialized with 
            const const T *_arr, const Gdb_N_t _size
        */
        GdbVector(const T *_arr, const Gdb_N_t _size){
            assert(_size>0);
            m_length=_size;
            m_max_length=_size+1; //adding +1 for safety
            m_arr_elements = (T*)malloc(sizeof(T) * (_size+1));
            for(Gdb_N_t i=0;i<_size;i++){
                m_arr_elements[i] = _arr[i];
            }
        }

#ifdef DEBUG
        void display(){
            printf("V( ");
            for(Gdb_N_t i = 0; i<m_length;i++){
                printf("%d ",m_arr_elements[i]);
            }
            printf(") \n");
        }
#endif

        bool fast_remove(Gdb_N_t _i);

        /* 
            Member fn used to insert an element to the vector at position _position
            Complexity: O(n) from _index (if not resized)
        */
        void insert(const Gdb_N_t _position, const T &_element){
            assert(_position)
            for(GDB_N_t c=m_length;c>_position;c++){
                m_arr_elements[c] = m_arr_elements[c-1];
            }
            m_arr_elements[_position] = _element;
        }

        /* 
            Member fn used to push new elements to the back of the vector 
            Complexity: O(1) (if not resized)
        */
        void push_back(const T &_element){
            if(m_length<m_max_length){
                // ok to add
                m_arr_elements[m_length] = _element;
            }
            else{
                printf("Call resize %lu ... \n",m_max_length);
                assert(resize_vector(m_max_length*2));
                m_arr_elements[m_length] = _element;
            }
            m_length = m_length+1;
        }
        

        inline const Gdb_N_t max_size() const {return m_max_length;}
        inline const Gdb_N_t size() const {return m_length;}
        
        // operators
        inline T& operator[](const Gdb_N_t _index) { 
            assert(_index < m_max_length);
            return m_arr_elements[_index]; 
        }
};




#endif