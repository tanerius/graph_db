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



// **************************************************************************
// LOGGER STUFF
// **************************************************************************

GdbLoggerEvent::GdbLoggerEvent() : GdbLoggerBase(){
    init();
}

void GdbLoggerEvent::Log(const char*_msg) {
    printf("%s \n",_msg);
}

void GdbLoggerEvent::init(){
    m_messages.allocate((int)MAX_RETURN_TYPES);
    m_messages.pushBack("Success. ");
    m_messages.pushBack("Illegal payload was provided. ");
    m_messages.pushBack("The adjacency array is full. ");
    m_messages.pushBack("Source element has been deleted or isnt assigned. ");
    m_messages.pushBack("Destination element has been deleted or isnt assigned. ");
    m_messages.pushBack("Memory could not be allocated. ");
    m_messages.pushBack("Index out of bounds. ");
    m_messages.pushBack("Maximum elements reached. ");
    m_messages.pushBack("The serializer is not in a proper state. ");
    m_messages.pushBack("There was an error trying to write the file. ");
    m_messages.pushBack("There was an error trying to read the file. ");
    m_messages.pushBack("There was an error trying to open the file. ");
    m_messages.pushBack("Locking mutex failed. ");
    m_messages.pushBack("Mutex init error. ");
    m_messages.pushBack("A mutex is idle. ");
    m_messages.pushBack("A mutex is locked. ");
    m_messages.pushBack("Thread/process is idle. ");
    m_messages.pushBack("Thread/process is finished. ");
    m_messages.pushBack("Thread/process running. ");
    m_messages.pushBack("Cannot create a thread. ");
    m_messages.pushBack("Did not add edge since it exists. ");
    m_messages.pushBack("Server initialization error. ");
    m_messages.pushBack("Generic error. ");
}

void GdbLoggerEvent::writeLog(const Gdb_ret_t _code, const char* _msg,int f){
    //for now just print
    if (f==1){
        //info
        printf("[INFO]: %s\n",_msg);
    } 
    else if (f==0){
        if(_code != OK){
            printf("[ERROR]: %s\n",(m_messages[_code]+_msg).cstr());
        }
        else{
            printf("[SUCCESS]: %s\n",_msg);
        }
    }
}



GdbLoggerFile::GdbLoggerFile() : GdbLoggerBase(){
    //init the ctor
    if(m_write_mutex.state() == MUTEX_IDLE){
        m_log_file = fopen(GDB_LOG_FILE, "a");
        if(m_log_file==NULL) {
            perror("Error opening log file.");
            assert(false);
        }
        fprintf(m_log_file,"%s : [Info] Initializing\n",getTimeStamp());
        fclose(m_log_file);
    }
    else{
        perror("Cant initialize mutexes.");
        assert(false); // cant init mutextes 
    }
}
void GdbLoggerFile::Log(const char* _msg) {
    m_has_error = false;
    m_write_mutex.lock();
    m_log_file = fopen(GDB_LOG_FILE, "a");
    if(m_log_file==NULL) {
        perror("Error writing log file.");
    }
    else{
        fprintf(m_log_file,"%s : %s\n",getTimeStamp(),_msg);
        fclose(m_log_file);
    }
    m_write_mutex.unlock();
}

// **************************************************************************
// PID CREATION
// **************************************************************************

void createPID(){
    if( access( GDB_PID_FILE, F_OK ) != -1 ){
        // file exists
        printf("\nGraphDB gdbd instance already running. \n");
        printf("Use \"gdbd -s\" or \"gdbd --stop\"  to terminate instance first. \n");
        exit(1);
    } else {
        FILE *f_pid = fopen(GDB_PID_FILE, "w");
        if(!f_pid){
            printf("Cannot create PID. \n");
            exit(1);
        }
        else{
            fprintf(f_pid,"%d\n",::getpid());
            fclose(f_pid);
        }
    }
}

void removePID(){
    remove(GDB_PID_FILE);
}