#include "bobutils.h"

int main() {
    Bobstring_t *str = (Bobstring_t*)malloc(sizeof(Bobstring_t));
    bobstring_init(str,"Hello Танер");
    printf("This is the string %s\n", str->str);
    bobstring_concat(str, ". Youre the best!");
    printf("This is the string %s\n", str->str);
    SafeFree(str->str);
    return 0;
}