/*gdbserver.h*/
#ifndef _GDBSERVER_H_
#define _GDBSERVER_H_

#ifndef _CONFIG_H_
#include "config.h"
#endif
#ifndef _GRAPHUTILS_H_
#include "graphutils.h"
#endif

/
int gdbCreateUnixSocket ( const char* );
int gdbCreateInetSocket ( DWORD, int );

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
    This is the class definition of the server
*/
class GdbServer{
    public:

    private:
        GdbLogger m_logger;
        // Array of client sockets
        int client_socket[GDB_MAX_CLIENTS];
};

#endif