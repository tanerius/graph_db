/*config.h*/
#ifndef _CONFIG_H_
#define _CONFIG_H_

#define LOGGER FILE

#ifndef GDB_NETWORK

#ifndef GDB_SOCKET
#define GDB_SOCKET             1                /* 0 - Unix socket, 1 - TCP */
#endif

#define GDB_LIST_PORT       6600                /* 6600 default TCP listening port */
#define GDB_LIST_ADDR   "0.0.0.0"               /* 0.0.0.0 means listen to all interfaces */
#define GDB_CTO_MS          3000                /* Timeout and close idle connections in 3 seconds */
#define GDB_BUFFER_SIZE     8192                /* 8KB IO buffer default - play around to get optimal value */
#define GDB_ADDRESS_SIZE    sizeof("000.000.000.000")
#define GDB_MAX_CLIENTS      100                // maximum number of clients
#define GDB_CONN_Q_BUFFER     10                // maximum number connections to queue to buffer if main socket is busy
#endif

#define GDB_LOG_FILE   "/var/log/gdb.log"               /* graphdb logger filename */
#define GDB_PID_FILE   "/var/run/gdbd.pid"              /* graphdb pid filename */
#define GDB_SOCK_FILE  "/var/run/gdbd.socket"           /* graphdb socket filename in case of unix sockets*/
#define GDB_LOCAL_LOG_FILE "./gdb.log"


typedef int Gdb_socket_desc_t;

#endif