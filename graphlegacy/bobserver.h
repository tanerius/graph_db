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