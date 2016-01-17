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

