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



Gdb_logger::Gdb_logger(){
    init();
}

void Gdb_logger::init(){
    m_messages.allocate(MAX_RETURN_TYPES);
    m_messages[OK] = "Success";
    m_messages[BAD_PAYLOAD] = "Illegal payload was provided";
    m_messages[PAGE_FULL] = "The adjacency array is full";
    m_messages[SRC_GONE] = "Source element has been deleted or isnt assigned";
    m_messages[DST_GONE] = "Destination element has been deleted or isnt assigned";
    m_messages[MEM_FULL] = "Memory could not be allocated";
    m_messages[NO_INDEX] = "Index out of bounds";
    m_messages[MAX_NODES] = "Maximum elements reached";
    m_messages[INVALID_SERIAL] = "The serializer is not in a proper state";
    m_messages[FILE_I_ERROR] = "There was an error trying to write the file";
    m_messages[FILE_O_ERROR] = "There was an error trying to read the file";
    m_messages[FILE_ERROR] = "There was an error trying to open the file";
    m_messages[LOCK_ERR] = "Locking mutex failed!";
    m_messages[MUTEX_ERR] = "Mutex init error";
    m_messages[MUTEX_IDLE] = "A mutex is idle";
    m_messages[MUTEX_LOCKED] = "A mutex is locked";
    m_messages[PROC_IDLE] = "Thread/process is idle";
    m_messages[PROC_OK] = "Thread/process is finished";
    m_messages[PROC_RUNNING] = "Thread/process running";
    m_messages[TH_C_ERROR] = "Cannot create a thread";
    m_messages[EDGE_DUPLICATE] = "Did not add edge since it exists";


}