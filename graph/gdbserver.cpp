/*gdbserver.cpp*/
#include "gdbserver.h"


int gdbCreateInetSocket ( DWORD u_addr, int i_port ){
    char s_address[GDB_ADDRESS_SIZE];
    gdbFormatIP ( s_address, GDB_ADDRESS_SIZE, u_addr );

    /*
    if ( u_addr==htonl ( INADDR_ANY ) )
        printf ( "listening on all interfaces, port=%d", i_port );
    else
        printf ( "listening on %s:%d", s_address, i_port );
    */

    static struct sockaddr_in iaddr;
    memset ( &iaddr, 0, sizeof(iaddr) );
    iaddr.sin_family = AF_INET;
    iaddr.sin_addr.s_addr = u_addr;
    iaddr.sin_port = htons ( (short)i_port );

    int i_sock = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( i_sock==-1 )
        GdbLoggerMain::Instance()->log("failed to create TCP socket");


    //set master socket to allow multiple connections , this is just a good habit, it will work without this
    int i_on = 1;
    if ( setsockopt ( i_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&i_on, sizeof(i_on) ) )
        GdbLoggerMain::Instance()->log("setsockopt() failed");


    int iTries = 12;
    int iRes;
    do
    {
        iRes = bind ( i_sock, (struct sockaddr *)&iaddr, sizeof(iaddr) );
        if ( iRes==0 )
            break;

        //printf ( "bind() failed on %s, retrying...", s_address );
        GdbLoggerMain::Instance()->log("bind() failed. Retrying...");
        // TODO: SLEEP A BIT - several seconds
        sleep(2000); // 2 seconds
    } while ( --iTries>0 );
    if ( iRes )
        GdbLoggerMain::Instance()->log("bind() failed.");

    return i_sock;
}

int gdbCreateUnixSocket ( const char * sock_file )
{
    static struct sockaddr_un uaddr;
    size_t len = strlen ( sock_file );

    if ( len + 1 > sizeof( uaddr.sun_path ) )
        printf ( "UNIX socket path is too long (len=%d)", (int)len );

    printf ( "listening on UNIX socket %s", sock_file );

    memset ( &uaddr, 0, sizeof(uaddr) );
    uaddr.sun_family = AF_UNIX;
    memcpy ( uaddr.sun_path, sock_file, len + 1 );

    int iSock = socket ( AF_UNIX, SOCK_STREAM, 0 );
    if ( iSock==-1 )
        printf ( "failed to create UNIX socket ");

    if ( unlink ( sock_file )==-1 )
    {
        if ( errno!=ENOENT )
            printf ( "unlink() on UNIX socket file failed" );
    }

    int iMask = umask ( 0 );
    if ( bind ( iSock, (struct sockaddr *)&uaddr, sizeof(uaddr) )!=0 )
        printf ( "bind() on UNIX socket failed " );
    umask ( iMask );

    return iSock;
}



/*
    Format an IP address received in BIG endian to char*
*/
char * gdbFormatIP ( char * str_ip, int i_buff_size, DWORD u_address )
{
    const BYTE *a = (const BYTE *)&u_address;
    snprintf ( str_ip, i_buff_size, "%u.%u.%u.%u", a[0], a[1], a[2], a[3] );
    return str_ip;
}


/*
    Server default constructor
*/
GdbServer::GdbServer(){
    if(m_log_mutex.state() == MUTEX_IDLE){
        m_log_mutex.lock();
        GdbLoggerMain::Instance()->info("GraphServer: Initializing");
        m_server_init_ok=true;
        m_log_mutex.unlock();
        m_client_greeting = "GraphDB TCP Server v1.0.\n\n";
    }
    else{
        m_server_init_ok=false;
    }
}

void GdbServer::initClientSockets(){
    // initialise all m_client_sockets[] to 0 so they dont get checked
    for (int i=0; i<GDB_MAX_CLIENTS; i++){
        m_client_sockets[i] = 0;
    }
}

int GdbServer::serverLoop(){
    //clear the socket set
    FD_ZERO(&readfds);

    //add master socket to set
    FD_SET(m_master_socket, &readfds);
    m_max_sd = m_master_socket;
     
    //add child sockets to set
    for (i=0; i<GDB_MAX_CLIENTS ; i++) 
    {
        //socket descriptor
        sd = m_client_sockets[i];
         
        //if valid socket descriptor then add to read list
        if(sd > 0)
            FD_SET( sd , &readfds);
         
        //highest file descriptor number, need it for the select function
        if(sd > m_max_sd)
            m_max_sd = sd;
    }

    activity = select( m_max_sd + 1 , &readfds , NULL , NULL , NULL);

    if ((activity < 0) && (errno!=EINTR)) 
    {
        GdbLoggerMain::Instance()->log("GraphServer: select() error");
    }
      
    //If something happened on the master socket , then its an incoming connection
    if (FD_ISSET(m_master_socket, &readfds)) 
    {
        GdbLoggerMain::Instance()->info("GraphServer: New connection via master socket.");
        if ((new_socket = accept(m_master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            GdbLoggerMain::Instance()->log("GraphServer: accept() error");
            return 3;
        }
      
        //inform user of socket number - used in send and receive commands
        printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
    
        //send new connection greeting message
        if( send(new_socket, m_client_greeting.cstr(), m_client_greeting.length(), 0) != m_client_greeting.length() ) 
        {
            GdbLoggerMain::Instance()->log("GraphServer: send() error");
        }
          
        GdbLoggerMain::Instance()->info("GraphServer: Welcome message sent.");
          
        //add new socket to array of sockets
        for (i = 0; i < GDB_MAX_CLIENTS; i++) 
        {
            //if position is empty
            if( m_client_sockets[i] == 0 )
            {
                m_client_sockets[i] = new_socket;
                printf("Adding to list of sockets as %d\n" , i);
                 
                break;
            }
        }
    }
    
    // also check for IO operation on some other socket :)
    for (i = 0; i < GDB_MAX_CLIENTS; i++) 
    {
        
        sd = m_client_sockets[i];
          
        if (FD_ISSET( sd , &readfds)) 
        {
            //Check if it was for closing , and also read the incoming message
            if ((ret = read( sd , m_buffer, GDB_BUFFER_SIZE)) == 0)
            {
                //Somebody disconnected , get his details and print
                getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                  
                //Close the socket and mark as 0 in list for reuse
                close( sd );
                m_client_sockets[i] = 0;
            }
              
            //Echo back the message that came in
            else
            {
                GdbLoggerMain::Instance()->info("GraphServer: send an echo.");
                //set the string terminating NULL byte on the end of the data read
                m_buffer[ret] = '\0';
                printf("The client sent: %s\n",m_buffer);
                //echo
                send(sd , m_buffer , strlen(m_buffer) , 0 );
            }
        }
    }
    return 0; // all is good
}

#ifdef DEBUG
int GdbServer::startDebug(){
    //set of socket descriptors
    fd_set readfds;
    if(!m_server_init_ok) return 1; // server in the wrong state
    m_message="GraphServer: GdbServer Test ECHO Server \r\n";
    
    // Aquire a mutex lock here since a lot of shit can happen
    GdbLoggerMain::Instance()->info(m_message.cstr());
        
    m_message = "GraphServer: Listening on port ";
    GdbNumeric a_number = GDB_LIST_PORT;

    initClientSockets();

    //create the master socket which will listen to TCP
    m_master_socket = gdbCreateInetSocket ( htonl( INADDR_ANY ), GDB_LIST_PORT );

    // write some info
    m_message+=a_number.cstr();
    GdbLoggerMain::Instance()->info(m_message.cstr());
    
         
    //try to specify maximum of GDB_CONN_Q_BUFFER pending connections for the master socket
    if (listen(m_master_socket, GDB_CONN_Q_BUFFER) < 0){
        GdbLoggerMain::Instance()->log("GraphServer: Listen failed.");
        m_log_mutex.unlock();
        return 2;
    }
    m_log_mutex.unlock();

    //accept the incoming connection
    int addrlen = sizeof(address);
    GdbLoggerMain::Instance()->info("GraphServer: Waiting for connections / socket activity ...");

    Gdb_socket_desc_t sd, new_socket; // declare a temporary socket descriptor
    int i,activity,ret; // declare common vars outside loop so they created only once

    while(TRUE){
        int r = serverLoop()
        
    }


    return 0;
}
#endif
