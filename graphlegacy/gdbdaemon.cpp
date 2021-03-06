#include "gdbserver.h"

volatile sig_atomic_t g_terminate;

void testjson(){
    picojson::value v;

    // parse the input
    std::cin >> v;
    std::string err = picojson::get_last_error();
    if (! err.empty()) {
        std::cerr << err << std::endl;
        exit(1);
    }

    // check if the type of the value is "object"
    if (! v.is<picojson::object>()) {
        std::cerr << "JSON is not an object" << std::endl;
        exit(2);
    }

    // obtain a const reference to the map, and print the contents
    const picojson::value::object& obj = v.get<picojson::object>();
    for (picojson::value::object::const_iterator i = obj.begin();
    i != obj.end();
    ++i) {
        std::cout << i->first << ": " << i->second.to_str() << std::endl;
    }
}

/*
    This is the user signal handler. Be careful this can handle only a number of funcitons.
*/
void signalHandler(int signum){
    g_terminate = 1;
}

int main(int argc, char *argv[]){

    // Check to make sure args were not requested
    if(argc > 1){
        if((strcmp(argv[1],"-v") == 0)||(strcmp(argv[1],"--version") == 0)){
            printf("\nGraphDB Server v%d.%d.%d\n\nCopyright 2016 Taner Selim <tanerius@gmail.com>\n",VERSION_MAJOR,VERSION_MINOR,VERSION_REV);
            printf("Version %d.%d Revision %d\n",VERSION_MAJOR,VERSION_MINOR,VERSION_REV);
            printf("Defaults:\n   Socket type: ");
            if(GDB_SOCKET == 0){
                printf("Unix socket on <%s>\n",GDB_SOCK_FILE);
            }
            else{
                printf("TCP socket on %s:%d\n",GDB_LIST_ADDR,GDB_LIST_PORT);
            }
            printf("   Payload buffer size: %d bytes\n", GDB_BUFFER_SIZE);
            printf("   Connection Q buffer size: %d\n", GDB_CONN_Q_BUFFER);
            printf("   Max clients: %d \n", GDB_MAX_CLIENTS);
            printf("   PID File: %s \n", GDB_PID_FILE);
            printf("   Log File: %s \n", GDB_LOG_FILE);
        }
    }


    struct sigaction action;
    memset(&action,0,sizeof(struct sigaction));
    action.sa_handler = signalHandler;
    sigaction(SIGTERM,&action,NULL);
    sigaction(SIGQUIT,&action,NULL);

    /* Our process ID and Session ID */
    pid_t pid, sid;

    /* Fork off the parent process */
    pid = fork();

    if (pid < 0) {
        perror("Cannot fork()");
        exit(1);
    }

    /* If we got a good PID, then
    we can exit the parent process. */
    if (pid > 0) {
        exit(0);
    }

    // Here the child process continues. Now daemonize!
    createPID(); // create the PID file or assert if a daemon is already running
    g_terminate = 0;

    /* Change the file mode mask */
    umask(0);

    /* Opening threadsafe logs */
    GdbLoggerMain::Instance()->Log("[OK] Initialize gdbd session...");

    /* Create a session ID for the process */
    sid = setsid();
    if (sid < 0) {
        /* Log the failure */
        GdbLoggerMain::Instance()->Log("[ERROR] Cannot setsid(). Exitting. ");
        exit(1);
    }

    if ((chdir("/")) < 0) {
        GdbLoggerMain::Instance()->Log("[ERROR] Changing working dir.");
        exit(1);
    }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    GdbLoggerMain::Instance()->Log("[OK] Starting the server.");
    GdbLoggerMain::Instance()->Log("[OK] Starting the server.");

    while(g_terminate==0){
        sleep(5); // sleep for 5 seconds
        GdbLoggerMain::Instance()->Log("[OK] No termination. Continuing");
    }
    //GdbServer main_server;
    //main_server.run();

    GdbLoggerMain::Instance()->Log("[OK] Received terminate.");
    
    removePID();
    GdbLoggerMain::Instance()->Log("[OK] Shutting down gdbd.");
    
    return 0;
}