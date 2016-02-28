#include "bobutils.h"
#include "bobipc.h"
#include "bobprocess.h"


typedef struct Test_s {
    long int bar;
    int foo;
    bool yesno;
} Test_type;


void fn_proc_producer(void* ipc) {
    Shared_msg_t* shared_space = (Shared_msg_t*)ipc;
    printf("This is a producer process \n");
    strcpy(shared_space->msg, "Hi from Producer \n");
    sleep(10);
    printf("Shared var from producer: %s", shared_space->msg);
}

void fn_proc_consumer(void* ipc) {
    Shared_msg_t* shared_space = (Shared_msg_t*)ipc;
    printf("This is a consumer process  \n");
    sleep(5); // sleep 10 seconds
    printf("Shared var from consumer: %s", shared_space->msg);
    strcpy(shared_space->msg, "Hi back \n");
}


void test_bobstring() {
    Bobstring_t *str = (Bobstring_t*)malloc(sizeof(Bobstring_t));
    bobstring_init(str,"Hello Танер");
    printf("This is the string %s\n", str->str);
    bobstring_concat(str, ". Youre the best!");
    printf("This is the string %s\n", str->str);
    SafeFree(str->str);
}


void test_shared_buffer() {
    printf("\n\nThe size of Test_type is %lu \n",sizeof(Test_type));
}


int main() {
    test_bobstring();
    Shared_msg_t* shared_space = create_mmap(sizeof(Shared_msg_t));
    shared_space->count = 0;
    pid_t child_procs[2];
    child_procs[0] = fork_child(fn_proc_producer, shared_space);
    child_procs[1] = fork_child(fn_proc_consumer, shared_space);
    wait_for_children(child_procs,2);
    printf("All exited unmapping memory\n");
    delete_mmap(shared_space);

    return 0;
}