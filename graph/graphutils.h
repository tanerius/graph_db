/*graphutils.h*/
#ifndef _GRAPHUTILS_H_
#define _GRAPHUTILS_H_

#ifndef _CONFIG_H_
#include "config.h"
#endif

#ifndef _PRIMITIVES_H_
#include "primitives.h"
#endif


#include <sys/mman.h> // for mmap and mlock

#include <cstring>          /* for strlen, strcpy and memset */
#include <cstdlib>          /* for itoa */
#include <iostream>
#include <pthread.h>
#include <assert.h>
#include <vector>
#include <ctime>
#include <climits>
#include <csignal>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h> // for htonl()


/* Helper macros */
#define minElement(a,b)            ((a)<(b)?(a):(b))
#define maxElement(a,b)            ((a)>(b)?(a):(b))
#define safeFree(_x)        { if (_x) { free (_x); (_x) = nullptr; } }
#define safeDelete(_x)      { if (_x) { delete (_x); (_x) = nullptr; } }
#define safeDeleteArray(_x) { if (_x) { delete [] (_x); (_x) = nullptr; } }
#define safeRelease(_x)     { if (_x) { (_x)->Release(); (_x) = nullptr; } }

/* so windows ppl can be more familiar */
typedef unsigned char BYTE;
typedef unsigned int DWORD;


template <typename T>  
T* allocMem(Gdb_N_t _size){ return (T*)malloc(_size * sizeof(T)); }

/*
    A simple function to check endianness
*/
inline bool isBigEndian(){
    return htonl(47) == 47;
}











/*
    IPC Mechanizm!

    Implementation for a shared memory buffer - for process to process chat
    Make it template for generics

    WARNING: mapping over 4GB requires 64 bit system or this will be impossible

    TODO: GdbSharedBuffer test required!!!
*/
template <class T> class GdbSharedBuffer{
public:
    // Construction
    GdbSharedBuffer(): 
        m_data_pointer(NULL),
        m_length(0),
        m_entities(0),
        m_lock_ram(false){}

    ~GdbSharedBuffer(){
        deAllocateMemory();
    }
    void setMemLock(bool _block){ m_lock_ram=_block; } // setter for lock mode!

    // Storage allocation
    bool allocateMemory(
        int64_t _entities, 
        GdbString &_s_notify
    ){
        // fail if data not present/unitialized pointer
        assert(!m_data_pointer);
        int64_t element_size = sizeof(T); // get the size in bytes of the element requested
        element_size = element_size*_entities; // how much total space we need to allocate
        m_length = (Gdb_N_t)element_size;
        if(element_size !=(int64_t)m_length){
            // Cant map over 4gb on 32bits
            _s_notify="Cannot map over 4GB.";
            m_length=0;
            return false;
        }
        //else map it!
        m_data_pointer = (BYTE*)mmap(NULL,m_length, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1 ,0);
        if(m_data_pointer==MAP_FAILED){
            _s_notify="mmap() failed.";
            m_length=0;
            return false;
        }
        if(m_lock_ram){
            if(mlock(m_data_pointer,m_length)==-1){
                _s_notify="mlock() failed to lock to RAM.";
                //TODO: maybe add a memory profiler at some point so we dont need to guess WTF went wrong!!
            }
        }
        //crash if still nothing interesting happened
        assert(m_data_pointer);
        // memory mapping succesded ... GG WP!
        m_entities=(Gdb_N_t)_entities;
        return true;
    }

    // member for doing a relock
    bool relockMemory(){
        if(!m_lock_ram) return true;
        if(mlock(m_data_pointer,m_length)!=-1) return true;
    }

    // Deallocate and reset the memory created by allocateMemory() call
    void deAllocateMemory(GdbString &_s_notify){
        // Case where nothing to deallocate
        if(!m_data_pointer) return;

        if(global_b_head_proces){
            int r=munmap(m_data_pointer,m_length);
            if(r){
                _s_notify="munmap() failed";
            }
        }
        m_data_pointer=NULL;
        m_length=0;
        m_entities=0;
    }

    void deAllocateMemory(){
        // Case where nothing to deallocate
        if(!m_data_pointer) return;

        munmap(m_data_pointer,m_length);
        m_data_pointer=NULL;
        m_length=0;
        m_entities=0;
    }

    /*
        Overloading [] will enable us to refer emelents like this : example: x[5]
    */
    inline const T & operator[](Gdb_N_t _index){
        assert(_index>=0);
        assert(_index<m_entities);
        return m_data_pointer[_index];
    }


    /*
        a function thats going to return the beginning of the memory block
    */
    T * getBlockHeadPtr() const{
        return m_data_pointer;
    }

    /*
        Function to check if GdbSharedBuffer instance is and empty buffer
    */
    bool isEmpty(){
        return m_data_pointer==NULL;
    }

    /*
        Some basic getter
    */
    Gdb_N_t getLength() const{
        return m_length;
    }

    Gdb_N_t getNumOfEntities() const{
        return m_entities;
    }


protected:
    T* m_data_pointer; // data to be stored
    Gdb_N_t m_length; // data length in bytes
    Gdb_N_t m_entities; // number of entities (total memory length + entities)
    bool m_lock_ram; // should we lock the data to the RAM
};





/*
    Create an implementation for a mutex for threads.
    The mutex should be able tu survive a fork()

    GdbSharedBuffer
*/
class GdbSharedMutex{
    protected:
        pthread_mutex_t *m_mutex_ptr;
        GdbSharedBuffer<BYTE> m_storage_buff;
        GdbString m_err_str;
    public:
        /* 
            Grabs the mutex and locks it. Others will block!
        */
        void lock() const;
        /* 
            Unlock the mutex!
        */
        void unlock() const;
        explicit GdbSharedMutex(int _size_plus=0);
        ~GdbSharedMutex();
        const char* getError() const;
};



/*
    A singleton tempplate for thoseobjects that really need it like the primary logger 
*/
template <class T>
    class Singleton
    {
    public:
      static T* Instance() {
          if(!m_pInstance) m_pInstance = new T;
          assert(m_pInstance != NULL);
          return m_pInstance;
      }
    protected:
      Singleton();
      ~Singleton();
    private:
      Singleton(Singleton const&);
      Singleton& operator=(Singleton const&);
      static T* m_pInstance;
    };
    
    template <class T> T* Singleton<T>::m_pInstance=NULL;

/*
    A custom mchanizm for logging. The base class is abstract for safety. 
*/
class GdbLoggerBase{
private:
    
    time_t m_rawtime;
    struct tm * m_timeinfo;
protected:
    GdbSharedMutex m_write_mutex;
    bool m_has_error;
    const char* getTimeStamp() {
        time (&m_rawtime);
        m_timeinfo = localtime (&m_rawtime);
        char * res_time = asctime(m_timeinfo);
        res_time[strlen(res_time)-1] = '\0';
        return res_time;
    }

public:
    virtual void Log(const char*) = 0;
};

class GdbLoggerFile : public GdbLoggerBase{
private:
    FILE *m_log_file;
public:
    void Log(const char*_msg);
    GdbLoggerFile(); // GDB_LOG_FILE
};


class GdbLoggerEvent : public GdbLoggerBase{
    public:
        // Standard ctor will create m_messages vector and output to stdout
        GdbLoggerEvent();

        void Log(const char*);
        
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

/*
    The singleton instance of the primary logger
    Thinking about whether to lock a mutex within the logger or let the calling classes handle it
*/

#ifndef LOGGER
    typedef Singleton<GdbLoggerEvent> GdbLoggerMain;
#elif LOGGER == FILE
    typedef Singleton<GdbLoggerFile> GdbLoggerMain;
#elif LOGGER == EVENT
    typedef Singleton<GdbLoggerEvent> GdbLoggerMain;
#else
    typedef Singleton<GdbLoggerEvent> GdbLoggerMain;
#endif






void createPID();
void removePID();







/*
    An interval map that can also accept intervals.
    The std::map should be used but hashing f(n) sholuld be as simple as possible for speed
    This bitch will run in a loop it needs to be fast!
    K = Key type 
    V = value type
*/
template <class K, class V>
    class GdbIntervalMap{
        private:
            std::map<K,V> m_map;
        public:
            // we need a constructotr here !!!!
            GdbIntervalMap( V const& val){
                m_map.insert(m_map.begin(),std::make_pair(std::numeric_limits<K>::lowest(),val));
            };

            void assign(K const& key, const V& val){
                m_map.insert( std::make_pair(key,val) );
            }

            /*
                Member to assign an interval to the hashmap!
                O(n) worst case! 
                TODO: log(n) would be perfect.
            */
            void assign_interval(K const&key_begin, K const&key_end, const V& val){
                if( !(key_begin < key_end)) return; // silly case begin should always be smaller
                V val_default = (--m_map.cend())->second; // get the value of the last element - needed in order to know the infinity val
                typename std::map<K,V>::iterator it_pred = m_map.lower_bound(key_begin);
                if(it_pred!= m_map.cbegin()){
                    //if interval does NOT overlap first element in map - in this case check that the interval is cannonical
                    assert((--it_pred)->second!=val); //the requested val shouldn't be the same as first element being overlapped
                    m_map.insert(std::make_pair(key_begin,val));
                }
                else{
                    // interval startes from the beginning - just add it
                    (m_map.begin())->second=val; // WARNING - Len(map) must be > 0 or this will be a disaster!!!
                }
                // walk over the interval now and clear values until upper bounds
                it_pred = m_map.lower_bound(key_begin);
                typename std::map<K,V>::iterator it_succ = m_map.upper_bound(key_end);
                while (it_pred!=it_succ){
                    if(it_pred->first < key_end){
                        it_pred->second=val; //set the element
                    }
                    it_pred++;
                }
                // Handle case where the key_end index is actually the biggest index, in which case add the default element so
                // we dont loose track od what was default!!!
                // TODO: figure out why exactly we need this check ... but sounds interesting!
                if(it_succ==m_map.cend()){
                    m_map.insert(std::make_pair(key_end,val_default));
                }
            }
      

#ifdef DEBUG
            /*
                A display function for the interval map which is available only if 
                compiled in DEBUG mode (with defined macro name)
                *** WARNING: Only works if key is a integer and value id char!!!
            */
            void display(){
                int i;
                for(i=0;i<20;i++){
                    printf("M[%d] = %c\n",i,operator[](i)); 
                }
            }

            // display elements by traversing map with pointers instead of 
            // indexes in order to check for key ordering!!!
            // CONFIRMED - traversal by iterator gives ordered keys!
            // Self balancing binary tree!
            void displayUsingPointers(){
                // get a iterator (fancy pointer) to head
                typename std::map<K,V>::iterator head = m_map.begin();
                while(head!=m_map.cend()){
                    printf("Map[%d] = %c\n",head->first,head->second);
                    //increment the pointer --- IDIOT!!!
                    head++;
                }
            }

#endif

            // we cant use the std::map [] operator need to modify it a bit to accomodate an interval implementation
            V const& operator[](K const& key) const {
                return (--m_map.upper_bound(key))->second;
            }
    };









/*
    Hashing definitions 
*/
inline Gdb_N_t Hash(const char* _string){return std::hash<std::string>()(_string);} 
inline Gdb_N_t Hash(const GdbString& _string){return ::Hash(_string.cstr());} 



/* 
    THREADS STUFF
*/



/* 
    Wrapper for a mutex
*/
class GdbMutex
{
public:
    GdbMutex () : m_is_init ( false ) {}
    ~GdbMutex () { assert ( !m_is_init ); }

    bool init ();
    bool finished ();
    bool lock ();
    bool unlock ();

protected:
    bool m_is_init;
    // coundlnt decide wheather c++11 or POSIX, but 
    // then went posix since forums say its better
    pthread_mutex_t m_mutex;
};


















/*
    For super global variables
*/
class GdbStaticMutex : public GdbMutex
{
public:
    GdbStaticMutex()
    {
        assert ( init() );
    }

    ~GdbStaticMutex()
    {
        finished();
    }
};












/* 
    Wrapper for read/write lock
*/
class GdbLock
{
public:
    GdbLock ();
    ~GdbLock () {}

    bool init ();
    bool done ();
    bool readLock ();
    bool writeLock ();
    bool unlock ();
private:
    pthread_rwlock_t    m_lock;
};













/* 
    An interface that prevents deriving classes from being copied 
    by making the copy constructor private
*/
class GdbNoCopy{
public:
    GdbNoCopy () {}
private:
    GdbNoCopy ( const GdbNoCopy & ) {}
    const GdbNoCopy& operator = ( const GdbNoCopy & ) { return *this; }
};
















/*
    defining custom thread type in case we decide to change the thread type at some point
*/
typedef pthread_t GdbThread_t;
typedef pthread_key_t GdbThreadKey_t;

// define a thread function
#define GDB_THREAD_FN void *



/*
    Thread key creator
*/
bool gdbThreadKeyCreate ( GdbThreadKey_t * _p_key );


/*
    Initialize a thread
*/
void * gdbThreadInit ( bool _detached=false );

/*
    Function to create a thread. Thread callback should be available!
*/
bool gdbThreadCreate(GdbThread_t *_thread, void (*_threaFn)(GDB_THREAD_FN), void *_arg, bool _is_detached );

#endif