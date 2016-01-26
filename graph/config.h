/*config.h*/
#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef GDB_NETWORK
#define GDB_SOCKET             1                /* 0 - Unix socket, 1 - TCP */
#define GDB_LIST_PORT       6600                /* 6600 default TCP listening port */
#define GDB_LIST_ADDR   "0.0.0.0"               /* 0.0.0.0 means listen to all interfaces */
#define GDB_CTO_MS          3000                /* Timeout and close idle connections in 3 seconds */
#define GDB_BUFFER_SIZE     8192                /* 8KB IO buffer default - play around to get optimal value */
#define GDB_ADDRESS_SIZE    sizeof("000.000.000.000")
#define GDB_MAX_CLIENTS      100                // maximum number of clients
#define GDB_CONN_Q_BUFFER     10                // maximum number connections to queue to buffer if main socket is busy
#endif

typedef int Gdb_socket_desc_t;

#endif