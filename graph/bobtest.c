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

void test_shared_buffer() {
    printf("\n\nThe size of Test_type is %lu \n",sizeof(Test_type));
    Shared_buffer_t *sb = (Shared_buffer_t*)malloc(sizeof(Test_type));
    sb->data_ptr = NULL;
    sb->total_entities = 10;
    sb->entity_size = sizeof(Test_type);
    sb->lock_ram = FALSE;
    int res = shared_buffer_init(sb);
    if (res == 0) {
        printf("We have 10 Test_type \n");

        Test_type *new_element  = (Test_type*)malloc(sizeof(Test_type));
        new_element->bar = 123;
        new_element->foo = 567;
        new_element->yesno = TRUE;


        res = munmap(sb->data_ptr, sizeof(Test_type) * 10);
        if(res != 0) {
            printf("We were not able to munmap \n");
        }
    }
    else {
        printf("We got an error in gettiong a shared buffer \n");
    }
}

int main() {
    test_bobstring();
    test_shared_buffer();

    return 0;
}