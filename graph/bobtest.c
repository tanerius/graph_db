#include "bobutils.h"
#include "bobipc.h"

typedef struct Test_s {
    long int bar;
    int foo;
    bool yesno;
} Test_type;

void test_bobstring() {
    Bobstring_t *str = (Bobstring_t*)malloc(sizeof(Bobstring_t));
    bobstring_init(str,"Hello Танер");
    printf("This is the string %s\n", str->str);
    bobstring_concat(str, ". Youre the best!");
    printf("This is the string %s\n", str->str);
    SafeFree(str->str);
}

Shared_buffer_t* test_shared_buffer() {
    printf("\n\nThe size of Test_type is %lu \n",sizeof(Test_type));
    Shared_buffer_t *sb = (Shared_buffer_t*)malloc(sizeof(Test_type));
    sb->data_ptr = NULL;
    sb->total_elements = 10;
    sb->total_element_size = sizeof(Test_type);
    sb->lock_ram = FALSE;
    int res = shared_buffer_init(sb);
    if (res == 0) {
        printf("We have 10 Test_type \n");
        return sb;
        /*
        Test_type *new_element  = (Test_type*)malloc(sizeof(Test_type));
        new_element->bar = 123;
        new_element->foo = 567;
        new_element->yesno = TRUE;

    

        res = munmap(sb->data_ptr, sizeof(Test_type) * 10);
        if(res != 0) {
            printf("We were not able to munmap \n");
        }
        */
    }
    else {
        printf("We got an error in gettiong a shared buffer \n");
        return NULL;
    }
}


int main() {
    test_bobstring();
    Shared_buffer_t *sb = test_shared_buffer();

    // fork some processes and have them pass stuff 
    /* Our process ID and Session ID */
    pid_t children[2];

    /* Fork off the parent process */
    printf("Forking p1 ... \n");
    pid1 = fork();

    if (pid1 < 0) {
        printf("Error forking p1 ... \n");
        exit(1);
    }

    /* If we got a good PID, then
    we can exit the parent process. */
    if (pid1 > 0) {
        printf("Forking p2 ... \n");
        pid2 = fork();
        if (pid2 > 0) {
            exit(0);
        }
        else{
            p2(sb);
        }
    }
    else{
        p1(sb);
    }

    return 0;
}