/*graphutils.cpp*/
#include "graphutils.h"
#include <cstdlib>          /*!<  malloc,calloc and free */
#include <cstring>          /*!<  strlen, strcpy and memset */

bool global_b_head_proces=true;

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

// consider making this a global!!!
char* GdbString::allocateMemory(Gdb_N_t string_length){
    return ::allocMem<char>(string_length*sizeof(char) + BYTE_GAP);
    //return (char*)malloc(string_length*sizeof(char) + BYTE_GAP);
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
        safeFree(m_string);
    m_length = 0;
}



// **************************************************************************
// LOGGER STUFF
// **************************************************************************

GdbLoggerEvent::GdbLoggerEvent() : GdbLoggerBase(){
    init();
}

void GdbLoggerEvent::Log(const char*_msg){
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
    m_log_file = fopen(GDB_LOG_FILE, "a");
    if(m_log_file==NULL) {
        perror("Error opening log file.");
        assert(false);
    }
    fprintf(m_log_file,"%s : [Info] Initializing\n",getTimeStamp());
    fclose(m_log_file);
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






// **************************************************************************
// Shared mutex implementation
// this mutex will survive a fork()
// **************************************************************************


GdbSharedMutex::GdbSharedMutex ( int _size_plus ){
    m_mutex_ptr = NULL;

    pthread_mutexattr_t mutex_attribute;
    int r = pthread_mutexattr_init ( &mutex_attribute );
    if ( r )
    {
        //pthread_mutexattr_init, errno=%d
        return;
    }
    r = pthread_mutexattr_setpshared ( &mutex_attribute, PTHREAD_PROCESS_SHARED );
    if ( r )
    {
        // "pthread_mutexattr_setpshared, errno = %d"
        m_err_str="pthread_mutexattr_setpshared error";
        return;
    }

    GdbString s_notify;
    // write mutexes to shared memory so all processes can use them
    if ( !m_storage_buff.allocateMemory ( sizeof(pthread_mutex_t) + _size_plus, s_notify) )
    {
        //"storage.alloc, error='%s', warning='%s'"
        m_err_str=s_notify;
        return;
    }

    m_mutex_ptr = (pthread_mutex_t*) m_storage_buff.getBlockHeadPtr();
    r = pthread_mutex_init ( m_mutex_ptr, &mutex_attribute );
    if ( r )
    {
        m_err_str="pthread_mutex_init error";
        m_mutex_ptr = NULL;
        m_storage_buff.deAllocateMemory (s_notify);
        return;
    }
}

void GdbSharedMutex::lock () const
{
    if ( m_mutex_ptr )
        pthread_mutex_lock ( m_mutex_ptr );
}


void GdbSharedMutex::unlock () const
{
    if ( m_mutex_ptr )
        pthread_mutex_unlock ( m_mutex_ptr );
}

const char* GdbSharedMutex::getError()const {
    const char *err_str = NULL;
    err_str = m_err_str.cstr();
    return err_str;
}

GdbSharedMutex::~GdbSharedMutex ( ){}