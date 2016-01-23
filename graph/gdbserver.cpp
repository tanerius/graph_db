/*gdbserver.cpp*/
#include "gdbserver.h"


int gdbCreateInetSocket ( DWORD u_addr, int i_port )
{
    char s_address[GDB_ADDRESS_SIZE];
    gdbFormatIP ( s_address, GDB_ADDRESS_SIZE, u_addr );

    if ( u_addr==htonl ( INADDR_ANY ) )
        printf ( "listening on all interfaces, port=%d", i_port );
    else
        printf ( "listening on %s:%d", s_address, i_port );

    static struct sockaddr_in iaddr;
    memset ( &iaddr, 0, sizeof(iaddr) );
    iaddr.sin_family = AF_INET;
    iaddr.sin_addr.s_addr = u_addr;
    iaddr.sin_port = htons ( (short)i_port );

    int i_sock = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( i_sock==-1 )
        printf ( "failed to create TCP socket" );

    int i_on = 1;
    if ( setsockopt ( i_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&i_on, sizeof(i_on) ) )
        printf ( "setsockopt() failed" );

    int iTries = 12;
    int iRes;
    do
    {
        iRes = bind ( i_sock, (struct sockaddr *)&iaddr, sizeof(iaddr) );
        if ( iRes==0 )
            break;

        printf ( "bind() failed on %s, retrying...", s_address );
        // TODO: SLEEP A BIT - several seconds
        sleep(2000); // 2 seconds
    } while ( --iTries>0 );
    if ( iRes )
        printf ( "bind() failed on %s", s_address );

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
    memcpy ( uaddr.sun_path, sPath, len + 1 );

    int iSock = socket ( AF_UNIX, SOCK_STREAM, 0 );
    if ( iSock==-1 )
        printf ( "failed to create UNIX socket ";

    if ( unlink ( sPath )==-1 )
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
