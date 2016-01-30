/*gdbserver.h*/
#ifndef _GDBSERVER_H_
#define _GDBSERVER_H_

#ifndef _CONFIG_H_
#include "config.h"
#endif
#ifndef _GRAPHUTILS_H_
#include "graphutils.h"
#endif


#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h> // for sockaddr_un
#include <sys/un.h> // for sockaddr_un
#include <sys/stat.h> // for umask
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

/*
    Create a socket IPC using unix socks - OS specific for now
*/
int gdbCreateUnixSocket ( const char* );

/*
    Create a socket for network comms
*/
int gdbCreateInetSocket ( DWORD, int );

/*
    Format an IP address received in BIG endian to char*
*/
char * gdbFormatIP ( char*, int, DWORD );

/*
    This is the class definition of the server
*/
class GdbServer{
    public:
        GdbServer(); // default ctor
        void run();
        
#ifdef DEBUG
        int startDebug(); // return != 0 is bad
#endif
        int serverLoop();

    private:
        GdbMutex m_log_mutex;
        // Array of client sockets
        Gdb_socket_desc_t m_client_sockets[GDB_MAX_CLIENTS];
        char m_buffer[GDB_BUFFER_SIZE];  //data buffer of 1K
        Gdb_socket_desc_t m_master_socket; // this is the master socket on which incomming connections will be handles
        Gdb_socket_desc_t m_max_sd; // maximum (highest) socket descriptor
        bool m_server_init_ok; // did init succeed?
        //set of socket descriptors
        fd_set readfds;
        struct sockaddr_in address; // address for incomming traffic
        GdbString m_client_greeting;
        GdbString m_message; // a variable for messages 
        pid_t m_process_ID; // process id of the 

    private: // methods
        void initClientSockets();


};


class GdbClientHandler{
    private:
        char m_buffer[GDB_BUFFER_SIZE];  //data buffer
        picojson::value json_value; // container for parsed json after a full packet has been received!
        Gdb_socket_desc_t client_socket;
        GdbString m_current_command;
    public:
        void parseCommand();
};

#endif