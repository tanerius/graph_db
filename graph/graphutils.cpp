/*graphutils.cpp*/
#include "graphutils.h"
#include <cstdlib>          /*!<  malloc,calloc and free */
#include <cstring>          /*!<  strlen, strcpy and memset */

bool GdbString::reallocate(Gdb_N_t new_size)
{
    // check that we want a positive size
    if(new_size <= 0)
        return false;
    
    char *old_string = m_string;
    m_length = new_size;
    m_string = (char*)malloc(new_size*sizeof(char));

    if (!m_string)   // Reallocation failed!!! panic 
    {
        if (old_string)
            safeFree(old_string); // free mem from old string
        m_length = 0;
        return false; // reallocation failed but old string lost too
    }

    if (old_string)
    {
        strcpy(m_string, old_string);
        safeFree(old_string);
    }
    else
        m_string[0] = '\0'; // old strong was bogus 

    return true;
}

char* GdbString::allocateMemory(Gdb_N_t string_length){
    return (char*)malloc(string_length*sizeof(char) + BYTE_GAP);
}


GdbString& GdbString::operator=(const char *_string)
{
    if (!_string)
    {
        clear();
        return *this;
    }

    if ( m_string==_string )
        return *this;

    Gdb_N_t n_len = strlen(_string);

    if (n_len < m_length)    //If allocated space is enough
        strcpy(m_string, _string);
    else
    {
        if (m_string){
            safeFree(m_string);
        }

        m_length = n_len+1;
        m_string = allocateMemory(m_length);
        memset ( m_string+m_length, 0, BYTE_GAP ); // introduce a safety gap

        if (!m_string)   //Couldn't allocate
            m_length = 0;
        else
            strcpy(m_string, _string);
    }

    return *this;
}

GdbString& GdbString::operator+=(const char *_string)
{
    Gdb_N_t n_len = length() + strlen(_string);

    if (n_len >= m_length)   //Reallocation needed
        if (!reallocate(n_len+1))
            return *this;

    strcat(m_string, _string);

    return *this;
}

GdbString GdbString::operator+(const char* _string)
{
    GdbString new_string = cstr();
    new_string += _string;
    return new_string;
}



void GdbString::clear()
{
    if (m_string)
        safeFree(m_string);
    m_length = 0;
}












template <typename T> Gdb_N_t GdbVector<T>::resize_vector(const Gdb_N_t _new_max_length){
    printf("Growing to %lu ... ",_new_max_length);
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

template <typename T> void GdbVector<T>::push_back(const T &_element){
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

template <typename T> void GdbVector<T>::display(){
    printf("V( ");
    for(Gdb_N_t i = 0; i<m_length;i++){
        printf("%d ",m_arr_elements[i]);
    }
    printf(") \n");
}

/*
    Implementation of default constructor for vector
*/
template <typename T> GdbVector<T>::GdbVector(){
    m_arr_elements = NULL;
    m_length=0;
    m_max_length=0;
}

/*
    Implementation of copy constructor for vector initialized with 
    const Gdb_N_t
*/
template <typename T> GdbVector<T>::GdbVector(const Gdb_N_t _max_length){
    assert(_max_length>0);
    m_length=0;
    m_max_length=_max_length;
    m_arr_elements = (T*)malloc(sizeof(T) * m_max_length);
}

/*
    Implementation of copy constructor for vector initialized with 
    const const T *_arr, const Gdb_N_t _size
*/
template <typename T> GdbVector<T>::GdbVector(const T *_arr, const Gdb_N_t _size){
    assert(_size>0);
    m_length=_size;
    m_max_length=_size+1; //adding +1 for safety
    m_arr_elements = (T*)malloc(sizeof(T) * (_size+1));
    for(Gdb_N_t i=0;i<_size;i++){
        m_arr_elements[i] = _arr[i];
    }
}