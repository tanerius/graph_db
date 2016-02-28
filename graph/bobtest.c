#include "bobutils.h"
#include "bobipc.h"
#include "bobprocess.h"

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

void test_shared_buffer() {
    printf("\n\nThe size of Test_type is %lu \n",sizeof(Test_type));
}


int main() {
    test_bobstring();
    

    return 0;
}