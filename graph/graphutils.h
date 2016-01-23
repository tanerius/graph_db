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
        ~GdbString(){ safeFree(m_string); m_length = 0;};

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
    Elements should have their own operators implemented for = and for desctuction.

*/
template <typename T> 
class GdbVector {
    private:
        T *m_arr_elements; 
        Gdb_N_t m_length;
        Gdb_N_t m_max_length;

        /* Member fn used to grow the size of the vector as needed by Gdb_N_t */
        Gdb_N_t resizeVector(const Gdb_N_t _new_max_length){
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

        ~GdbVector(){
            if(m_arr_elements!=NULL){
                m_max_length=0;
                m_length=0;
                safeFree(m_arr_elements);
            }
        }

        /* Member fn used to grow the size of the vector as needed by Gdb_N_t */
        Gdb_N_t allocate(const Gdb_N_t _size){
            assert(_size > 0);
            assert(m_arr_elements==NULL);
            m_arr_elements = (T*)malloc(sizeof(T) * _size);
            if(m_arr_elements){
                m_length=0;
                m_max_length=_size; 
                return m_max_length;
            }
            else{
                m_arr_elements = NULL;
                return 0;
            }
        }

        /*
            Alias for maxSize()
        */
        inline const Gdb_N_t capacity() const {return maxSize();}

#ifdef DEBUG
        void display(){
            printf("V( ");
            for(Gdb_N_t i = 0; i<m_length;i++){
                printf("%d ",m_arr_elements[i]);
            }
            printf(") \n");
        }
#endif

        inline const bool empty() const {return !(size() && true);}
        
        /* 
            Erase a range of elements from the vector
            Complexity: O(n) from _position+offset onwards (move)
        */
        Gdb_N_t erase(const Gdb_N_t _position, const Gdb_N_t _offset){
            assert(_position>0); // cant have invalid positions
            assert(_offset>=0); // cant have negative offsets
            assert(_position+_offset<m_length); // cant go beyond vector size
            Gdb_N_t ret = _position;
            if((_position+_offset) == (m_length-1)){ ret = (_position+_offset)-1;}
            for(Gdb_N_t c=_position;(c+_offset)<m_length;c++){
                m_arr_elements[c]=m_arr_elements[c+_offset+1];
            }
            m_length=m_length-(_offset+1);
            return ret;
        }

        /* 
            Erase an element from the vector
            Complexity: O(n) from _position onwards (move)
        */
        Gdb_N_t erase(const Gdb_N_t _position){
            assert(_position>0);
            assert(_position<m_length);
            return erase(_position, 0);
        }

        /* 
            Member fn used to insert an element to the vector at position _position
            Complexity: O(1) constantly. This is because it
            moves the _posion element to the end of the list.
        */
        void fastInsert(const Gdb_N_t _position, const T &_element){
            assert(_position>0);
            assert(_position<m_max_length);
            if(m_length==m_max_length){
                // doule the vectors max size
                assert(resizeVector(m_max_length*2));
            }
            m_arr_elements[m_length] = m_arr_elements[_position];
            m_arr_elements[_position] = _element;
            m_length++;
        }

        /* 
            Member fn used to insert an element to the vector at position _position
            Complexity: O(n) from _index (if not resized)
        */
        void insert(const Gdb_N_t _position, const T &_element){
            assert(_position>0);
            assert(_position<m_max_length);
            if(!(m_length<m_max_length)){
                // doule the vectors max size
                assert(resizeVector(m_max_length*2));
            }
            for(Gdb_N_t c=m_length;c>_position;c--){
                m_arr_elements[c] = m_arr_elements[c-1];
            }
            m_arr_elements[_position] = _element;
            m_length++;
        }

        void popBack(){
            // pops the last element
            m_length--;
        }

        /* 
            Member fn used to push new elements to the back of the vector 
            Complexity: O(1) (if not resized)
        */
        void pushBack(const T &_element){
            if(!(m_length<m_max_length)){
                assert(resizeVector(m_max_length*2));
            }
            m_arr_elements[m_length] = _element;
            m_length++;
        }
        
        /* 
            toEnd is a function that returns the number of elements until the end of the vector
            starting from _position.
        */
        const Gdb_N_t toEnd(const Gdb_N_t _position) const {
            return m_length-_position-1;
        }

        inline const Gdb_N_t maxSize() const {return m_max_length;}
        
        /* 
            The actual size of the vector (NOT its capacity). 
            size <= capacity
        */
        inline const Gdb_N_t size() const {return m_length;}

        /* 
            Swaps two elements in the vector given their index.
        */
        void swap(const Gdb_N_t _index1, const Gdb_N_t _index2){
            assert(_index1<m_length);
            assert(_index2<m_length);
            assert(_index1>=0);
            assert(_index2>=0);
            T tmp = m_arr_elements[_index1];
            m_arr_elements[_index1] = m_arr_elements[_index2];
            m_arr_elements[_index2] = tmp;
        }
        
        // operators
        inline T& operator[](const Gdb_N_t _index) { 
            assert(_index < m_max_length);
            assert(_index >= 0);
            return m_arr_elements[_index]; 
        }

        /*
            == and != comparison operators for vector

            V1 == V2 iff for every element x of V1 there is an element y ov V2 such that x==y
        */
        bool operator==(GdbVector& _vector) { 
            if(_vector.m_length != m_length){
                return false;
            }
            for(Gdb_N_t c=0;c<m_length;c++){
                if(m_arr_elements[c]!=_vector.m_arr_elements[c])
                    return false;
            }
            return true;
        }
        bool operator!=(GdbVector& _vector) { return !operator==(_vector); }
};

/*
    Create an implementation for a mutex for threads
*/
typedef struct Gdb_mutex_s{
    private:
        Gdb_ret_t m_state;
        pthread_mutex_t m_mutex_generic;
    public:
        
        /* 
            Grabs the mutex and locks it. Others will block!
        */
        void lock(){
            pthread_mutex_lock(&m_mutex_generic);
            m_state=MUTEX_LOCKED;
        }

        /* 
            State of the mutex! It is best to always check that == MUTEX_IDLE after initialization 
            of the class. Lock and unlock do this implicitly so no need!
        */
        inline const Gdb_ret_t state() const{return m_state;}

        /* 
            Release the mutex!
        */
        void unlock(){
            pthread_mutex_unlock(&m_mutex_generic);
            m_state=MUTEX_IDLE;
        }

        Gdb_mutex_s(){
            if (pthread_mutex_init(&m_mutex_generic, NULL) != 0){
                m_state=MUTEX_ERR;
            }
            else{
                m_state=MUTEX_IDLE;
            }
        }

        ~Gdb_mutex_s(){
            pthread_mutex_destroy(&m_mutex_generic);
        }
} Gdb_mutex;

/*
    A custom class for logging messages 
*/
class Gdb_logger{
    public:
        // Standard ctor will create m_messages vector and output to stdout
        Gdb_logger();
        // error message 
        inline void log(const Gdb_ret_t _code, const char* _msg){writeLog(_code,_msg,0);};
        inline void log(const char* _msg){writeLog(GENERIC_ERR,_msg,0);} 

        // info message 
        inline void info(const char* _msg){writeLog(OK,_msg,1);}
    private:
        GdbVector<GdbString> m_messages;
        GdbString m_filename;
        void writeLog(const Gdb_ret_t, const char*,int);

        void init();
};

#endif