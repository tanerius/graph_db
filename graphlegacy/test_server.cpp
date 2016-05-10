#include "gdbserver.h"
  
#define TRUE   1
#define FALSE  0
#define PORT 8888
 
int main(int argc , char *argv[])
{
    GdbServer server;
    server.startDebug();
    return 0;
} 