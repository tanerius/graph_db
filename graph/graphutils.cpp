/*graphutils.cpp*/
#include "graphutils.h"
#include <cstdlib>          /*!<  malloc,calloc and free */
#include <cstring>          /*!<  strlen, strcpy and memset */

bool global_b_head_proces=true;
static int global_thread_stack_size = PTHREAD_STACK_MIN + 65536;





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

GdbSharedMutex::~GdbSharedMutex ( ){
    m_storage_buff.deAllocateMemory ();
    pthread_mutex_destroy(m_mutex_ptr);
}


/*
    Implementation of the mutex wrapper (using the implest posix mutex)
*/
bool GdbMutex::init ()
{
    m_is_init = ( pthread_mutex_init ( &m_mutex, NULL )==0 );
    return m_is_init;
}

bool GdbMutex::finished ()
{
    if ( !m_is_init )
        return true;

    m_is_init = false;
    return pthread_mutex_destroy ( &m_mutex )==0;
}

bool GdbMutex::lock ()
{
    return ( pthread_mutex_lock ( &m_mutex )==0 );
}

bool GdbMutex::unlock ()
{
    return ( pthread_mutex_unlock ( &m_mutex )==0 );
}



/*
    Implementation of the lock wrapper (posix works best for nGdbLockow)
*/

GdbLock::GdbLock ()
{}

bool GdbLock::init ()
{
    return pthread_rwlock_init ( &m_lock, NULL )==0;
}

bool GdbLock::done ()
{
    return pthread_rwlock_destroy ( &m_lock )==0;
}

bool GdbLock::readLock ()
{
    return pthread_rwlock_rdlock ( &m_lock )==0;
}

bool GdbLock::writeLock ()
{
    return pthread_rwlock_wrlock ( &m_lock )==0;
}

bool GdbLock::unlock ()
{
    return pthread_rwlock_unlock ( &m_lock )==0;
}


/*
    Thread related implementations
*/
GdbThreadKey_t global_thread_cleanup_key;
GdbThreadKey_t global_thread_stack;

struct GdbThreadCaller_t{
    void (*m_pCallingFn)(void *_arg);
    void *m_argument;
    GdbThreadCaller_t * m_next_caller;
};

GDB_THREAD_FN gdbThreadFnWrapper(void*_arguments){
    //char c_top_of_the_stack;
    GdbThreadCaller_t *p_call = (GdbThreadCaller_t*)_arguments;
    // we should remember the top of the stack somehow
    //...
    // call the actual function we want to run
    p_call->m_pCallingFn(p_call->m_argument);

    return 0; // double check the return cuz i think we are wrong!
}


bool threadKeyCreate ( GdbThreadKey_t * _p_key )
{
    return pthread_key_create ( _p_key, NULL )==0;
}


void * threadInit ( bool _detached ){
    static bool b_init = false;
    static pthread_attr_t joinable_attr;
    static pthread_attr_t detached_attr;
    if ( !b_init )
    {
        // we're single-threaded yet, right?!
        if ( !::threadKeyCreate ( &global_thread_cleanup_key ) )
            exit(1);

        if ( !::threadKeyCreate ( &global_thread_stack ) )
            exit(1);

        if ( pthread_attr_init ( &joinable_attr ) )
            exit(1);

        if ( pthread_attr_init ( &detached_attr ) )
            exit(1);

        if ( pthread_attr_setdetachstate ( &detached_attr, PTHREAD_CREATE_DETACHED ) )
            exit(1);

        b_init = true;
    }

    if ( pthread_attr_setstacksize ( &joinable_attr, global_thread_stack_size ) )
        exit(1);

    return _detached ? &detached_attr : &joinable_attr;

}

/*
    Actually initialize a thread here. True if thread was created normally else false!
*/  
bool gdbThreadCreate(GdbThread_t *_thread, void (*_threadFn)(GDB_THREAD_FN), void *_arg, bool _is_detached ){
    // Need to make sure that the thread doesnt go out of 
    // scope or die before the wrapper is able to see it
    GdbThreadCaller_t *p_caller = ::allocMem<GdbThreadCaller_t>(sizeof(GdbThreadCaller_t));
    p_caller->m_next_caller=NULL;
    p_caller->m_pCallingFn=_threadFn;
    p_caller->m_argument=_arg;
    // now we want to actually create the thread
    void *tmp_attrs=::threadInit ( _is_detached );
    // TODO: CREATE THIS NOW gdbProcedureWrapper
    int creation_result = pthread_create(_thread,(pthread_attr_t*) tmp_attrs, gdbThreadFnWrapper, p_caller);

    //will p_caller leak here????

    return (creation_result==0);
}