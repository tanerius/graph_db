#include "gdbstring.h"
#include <cstring>
#include <iostream>


Gdb_N_t GdbString::hashDjb2(){
    Gdb_N_t h = 5381;
    const char *p = m_string;

    while (*p != '\0')
        h = ((h << 5) + h) + (*p++); /* hash * 33 + c */

    return h;
}

Gdb_N_t GdbString::hashSdbm(){
    Gdb_N_t hash = 0;
    const char *p = m_string;

    while (*p != '\0')
        hash = (*p++) + (hash << 6) + (hash << 16) - hash;

    return hash;
}

bool GdbString::reallocate(Gdb_N_t new_size){
    // check that we want a positive size
    if(new_size <= 0)
        return false;
    
    char *old_string = m_string;
    m_length = new_size;
    m_string = allocateMemory(new_size);

    if (!m_string)   // Reallocation failed!!! panic 
    {
        if (old_string)
            safeDeleteArray(old_string); // free mem from old string
        m_length = 0;
        return false; // reallocation failed but old string lost too
    }

    if (old_string)
    {
        strcpy(m_string, old_string);
        safeDeleteArray(old_string);
    }
    else
        m_string[0] = '\0'; // old strong was bogus 

    return true;
}

// consider making this a global!!!
char* GdbString::allocateMemory(Gdb_N_t string_length){
    return new char[string_length + BYTE_GAP];
}

GdbVector <GdbString> GdbString::tokenize() {
    assert ( m_length > 0 );
    assert (m_string != nullptr);

    GdbVector <GdbString> tokens(5);
    GdbString to_tokenize = cstr();
    
    char *pch = std::strtok (to_tokenize.m_string," ,.\n\t");
    while (pch != NULL)
    {
        GdbString token = pch;
        tokens.pushBack(token);
        pch = strtok (NULL, " ,.\n\t");
    }

    return tokens;
}


GdbString& GdbString::operator=(const char *_string){
    if (!_string){
        clear();
        return *this;
    }

    if ( m_string==_string ) return *this;

    Gdb_N_t n_len = strlen(_string);

    if (n_len < m_length)    //If allocated space is enough
        strcpy(m_string, _string);
    else{
        if (m_string){
            safeDeleteArray(m_string);
        }

        m_length = n_len+1;
        m_string = allocateMemory(m_length);
        memset ( m_string+m_length, 0, BYTE_GAP ); // introduce a safety gap

        if (!m_string){   //Couldn't allocate
            assert(false); // cant continue like this ever! wooden PC?!
            m_length = 0;
        }
        else{
            strcpy(m_string, _string);
        }
    }

    return *this;
}

GdbString& GdbString::operator+=(const char *_string){
    Gdb_N_t n_len = length() + strlen(_string);

    if (n_len >= m_length)   //Reallocation needed
        if (!reallocate(n_len+1))
            return *this;

    strcat(m_string, _string);

    return *this;
}

GdbString GdbString::operator+(const char* _string){
    GdbString new_string = cstr();
    new_string += _string;
    return new_string;
}



void GdbString::clear(){
    if (m_string)
        safeDeleteArray(m_string);
    m_length = 0;
}