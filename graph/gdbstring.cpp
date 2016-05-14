#include "gdbstring.h"

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

GdbString* GdbString::tokenizeString() {
    Gdb_N_t len = m_length-1;
    Gdb_N_t previous = 0;
    GdbVector <GdbString> tokens(10);
    int count = 0;
    for(count=0;count < len; count++) { // the initializer may be an array}
        if ((count>previous) && (m_string[count] == ' ') && (count < (len-1)) && (count > 0)){
            tokens.pushBack(subString(previous,count-previous));
            previous = count+1;
        }
    } 
    return nullptr;
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